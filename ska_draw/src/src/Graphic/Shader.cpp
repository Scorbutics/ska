#include "Graphic/__internalConfig/LoggerConfig.h"
#include "Shader.h"
#include <cstring>
#include <fstream>
#include <iterator>
#include "Core/SDLLibrary.h"
#include <sstream>
#ifdef __EMSCRIPTEN__
#include <GLES2/gl2.h>
#endif

ska::Shader::Shader(std::string path, bool hasUniforms) :
	m_path(std::move(path)),
	m_hasUniforms(hasUniforms) {
}

ska::Shader::~Shader() {
	release();
}

void ska::Shader::release() const {
	const auto& sdl = SDLLibrary::get();
	if (m_state != nullptr) {
		sdl.destroyGPURenderState(m_state);
		m_state = nullptr;
	}
	if (m_shader != nullptr && m_device != nullptr) {
		sdl.releaseGPUShader(*m_device, m_shader);
		m_shader = nullptr;
	}
}

void ska::Shader::setUniforms(const void* data, std::size_t size) {
	m_uniforms.resize(size);
	std::memcpy(m_uniforms.data(), data, size);
	m_uniformsChanged = true;
}

bool ska::Shader::load(SDL_Renderer& renderer) const {
	const auto& sdl = SDLLibrary::get();
	m_renderer = &renderer;
	m_device = sdl.gpuRendererDevice(renderer);
	if (m_device == nullptr) {
		SLOG(LogLevel::Warn) << "The shader \"" << m_path << "\" cannot be used : the renderer is not the \"gpu\" one (" << sdl.rendererName(renderer) << ")";
		return false;
	}

	// The compiled shader of the graphics API of the device
	const auto formats = sdl.gpuShaderFormats(*m_device);
	auto info = SDL_GPUShaderCreateInfo{};
	auto extension = std::string{};
	if (formats & SDL_GPU_SHADERFORMAT_SPIRV) {
		info.format = SDL_GPU_SHADERFORMAT_SPIRV;
		info.entrypoint = "main";
		extension = ".frag.spv";
	} else if (formats & SDL_GPU_SHADERFORMAT_MSL) {
		info.format = SDL_GPU_SHADERFORMAT_MSL;
		info.entrypoint = "main0";
		extension = ".frag.msl";
	} else if (formats & SDL_GPU_SHADERFORMAT_DXIL) {
		info.format = SDL_GPU_SHADERFORMAT_DXIL;
		info.entrypoint = "main";
		extension = ".frag.dxil";
	} else {
		SLOG(LogLevel::Warn) << "The shader \"" << m_path << "\" cannot be used : no shader format known by the device";
		return false;
	}

	auto file = std::ifstream{ m_path + extension, std::ios::binary };
	if (!file) {
		SLOG(LogLevel::Error) << "The shader \"" << m_path << extension << "\" cannot be read";
		return false;
	}
	const auto code = std::vector<char>{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

	info.code = reinterpret_cast<const Uint8*>(code.data());
	info.code_size = code.size();
	info.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
	info.num_samplers = 1;
	info.num_uniform_buffers = m_hasUniforms ? 1 : 0;
	m_shader = sdl.createGPUShader(*m_device, info);
	if (m_shader == nullptr) {
		SLOG(LogLevel::Error) << "The shader \"" << m_path << extension << "\" cannot be created : " << sdl.getError();
		return false;
	}

	auto stateInfo = SDL_GPURenderStateCreateInfo{};
	stateInfo.fragment_shader = m_shader;
	m_state = sdl.createGPURenderState(renderer, stateInfo);
	if (m_state == nullptr) {
		SLOG(LogLevel::Error) << "The render state of the shader \"" << m_path << "\" cannot be created : " << sdl.getError();
		return false;
	}
	m_uniformsChanged = true;
	SLOG(LogLevel::Info) << "Shader \"" << m_path << extension << "\" loaded";
	return true;
}

SDL_GPURenderState* ska::Shader::use(SDL_Renderer& renderer) const {
	if (m_failed) {
		return nullptr;
	}
	if (m_state == nullptr || m_renderer != &renderer) {
		release();
		if (!load(renderer)) {
			release();
			m_failed = true;
			return nullptr;
		}
	}
	if (m_uniformsChanged && m_hasUniforms && !m_uniforms.empty()) {
		SDLLibrary::get().setGPURenderStateFragmentUniforms(*m_state, 0, m_uniforms.data(), static_cast<Uint32>(m_uniforms.size()));
		m_uniformsChanged = false;
	}
	return m_state;
}

SDL_GPURenderState* ska::Shader::use(SDL_Renderer& renderer, const void* uniforms, std::size_t size) const {
	auto* state = use(renderer);
	if (state != nullptr) {
		// SDL draws the drawings already given with the previous uniforms first
		SDLLibrary::get().setGPURenderStateFragmentUniforms(*state, 0, uniforms, static_cast<Uint32>(size));
		// The uniforms of the shader are given again to its next drawing without its own ones
		m_uniformsChanged = true;
	}
	return state;
}

#ifdef __EMSCRIPTEN__
namespace {
	// The vertices of the renderer (SDLRenderer::drawThroughWeb) : the attributes of SDL (0 : position, 2 : texture
	// coordinates), the color of the drawing white
	const char* WebVertexShader = R"(#version 100
attribute vec2 a_position;
attribute vec2 a_uv;
varying vec2 v_uv;
varying vec4 v_color;
void main() {
	v_uv = a_uv;
	v_color = vec4(1.0);
	gl_Position = vec4(a_position, 0.0, 1.0);
}
)";

	GLuint Compile(GLenum type, const std::string& source, const std::string& name) {
		const auto shader = glCreateShader(type);
		const char* text = source.c_str();
		glShaderSource(shader, 1, &text, nullptr);
		glCompileShader(shader);
		GLint ok = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
		if (!ok) {
			char log[1024] = {};
			glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
			SLOG_STATIC(ska::LogLevel::Error, ska::Shader) << "The shader \"" << name << "\" does not compile : " << log;
			glDeleteShader(shader);
			return 0;
		}
		return shader;
	}
}

bool ska::Shader::loadWeb() const {
	auto file = std::ifstream{ m_path + ".frag.es" };
	if (!file) {
		SLOG(LogLevel::Error) << "The shader \"" << m_path << ".frag.es\" cannot be read";
		return false;
	}
	const auto source = std::string{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
	const auto vertex = Compile(GL_VERTEX_SHADER, WebVertexShader, "vertex of " + m_path);
	const auto fragment = Compile(GL_FRAGMENT_SHADER, source, m_path + ".frag.es");
	if (vertex == 0 || fragment == 0) {
		return false;
	}
	const auto program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glBindAttribLocation(program, 0, "a_position");
	glBindAttribLocation(program, 2, "a_uv");
	glLinkProgram(program);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	GLint ok = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &ok);
	if (!ok) {
		char log[1024] = {};
		glGetProgramInfoLog(program, sizeof(log), nullptr, log);
		SLOG(LogLevel::Error) << "The shader \"" << m_path << ".frag.es\" does not link : " << log;
		glDeleteProgram(program);
		return false;
	}
	m_webProgram = program;
	m_webTexture = glGetUniformLocation(program, "u_texture");
	m_webTextureSize = glGetUniformLocation(program, "u_textureSize");
	// The members of the uniform block, from the first line ("// ska-uniforms vec4 u_a vec4 u_b") : their std140 offsets
	auto words = std::istringstream{ source.substr(0, source.find('\n')) };
	auto word = std::string{};
	words >> word >> word;
	auto offset = std::size_t{ 0 };
	auto type = std::string{};
	auto name = std::string{};
	while (words >> type >> name) {
		const auto floats = type == "vec4" ? 4 : type == "vec3" ? 3 : type == "vec2" ? 2 : 1;
		const auto align = std::size_t{ floats == 1 ? 4u : floats == 2 ? 8u : 16u };
		offset = (offset + align - 1) / align * align;
		m_webUniforms.push_back({ glGetUniformLocation(program, name.c_str()), type == "int" ? 0 : floats, offset });
		offset += static_cast<std::size_t>(floats) * 4;
	}
	SLOG(LogLevel::Info) << "Shader \"" << m_path << ".frag.es\" loaded (WebGL)";
	return true;
}

unsigned int ska::Shader::webProgram() const {
	if (m_failed) {
		return 0;
	}
	if (m_webProgram == 0 && !loadWeb()) {
		m_failed = true;
		return 0;
	}
	return m_webProgram;
}

void ska::Shader::applyWebUniforms(float textureWidth, float textureHeight) const {
	glUniform1i(m_webTexture, 0);
	glUniform2f(m_webTextureSize, textureWidth, textureHeight);
	for (const auto& u : m_webUniforms) {
		if (u.location < 0 || u.offset + 4 * static_cast<std::size_t>(u.floats == 0 ? 1 : u.floats) > m_uniforms.size()) {
			continue;
		}
		const auto* data = m_uniforms.data() + u.offset;
		if (u.floats == 0) {
			auto value = 0;
			std::memcpy(&value, data, 4);
			glUniform1i(u.location, value);
			continue;
		}
		float values[4] = {};
		std::memcpy(values, data, static_cast<std::size_t>(u.floats) * 4);
		switch (u.floats) {
		case 1: glUniform1fv(u.location, 1, values); break;
		case 2: glUniform2fv(u.location, 1, values); break;
		case 3: glUniform3fv(u.location, 1, values); break;
		default: glUniform4fv(u.location, 1, values); break;
		}
	}
}
#else
unsigned int ska::Shader::webProgram() const {
	return 0;
}

void ska::Shader::applyWebUniforms(float, float) const {
}

bool ska::Shader::loadWeb() const {
	return false;
}
#endif
