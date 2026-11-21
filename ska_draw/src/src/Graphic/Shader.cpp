#include "Graphic/__internalConfig/LoggerConfig.h"
#include "Shader.h"
#include <cstring>
#include <fstream>
#include <iterator>
#include "Core/SDLLibrary.h"

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
