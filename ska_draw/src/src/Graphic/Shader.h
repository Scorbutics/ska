#pragma once
#include <cstddef>
#include <string>
#include <vector>

extern "C" {
	typedef struct SDL_Renderer SDL_Renderer;
	typedef struct SDL_GPUDevice SDL_GPUDevice;
	typedef struct SDL_GPUShader SDL_GPUShader;
	typedef struct SDL_GPURenderState SDL_GPURenderState;
}

namespace ska {
	/**
	 * \brief A fragment shader, run on the drawings (RenderEffect::shader) or on the whole screen (Renderer::setScreenShader).
	 *
	 * The shader is compiled before the game runs (cmake/SkaShaders.cmake : ska_compile_shaders, from GLSL) : "path" is
	 * its file without extension, the renderer loads the format of its graphics API ("path.frag.spv" for Vulkan, ".msl"
	 * for Metal, ".dxil" for Direct3D 12). It needs the "gpu" render driver : with another one, the drawings are done
	 * without it (a warning is logged once).
	 *
	 * The GLSL shader receives the color of the drawing (location 0) and its texture coordinates (location 1), the texture
	 * drawn (set 2, binding 0), and the uniforms given here (set 3, binding 0) when it has some. The uniforms can change
	 * between two drawings (SDL draws the drawings already given with the previous ones first).
	 */
	class Shader {
	public:
		explicit Shader(std::string path, bool hasUniforms = false);
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;
		~Shader();

		// The uniforms of the next drawings (copied). Their layout is the uniform block of the shader (std140)
		void setUniforms(const void* data, std::size_t size);

		template <class T>
		void setUniforms(const T& data) {
			setUniforms(&data, sizeof(T));
		}

		const std::string& path() const { return m_path; }

		// The renderer : the state to draw with (created at the first use), nullptr when the shader cannot be used
		SDL_GPURenderState* use(SDL_Renderer& renderer) const;
		// The same, with uniforms for one drawing (RenderEffect::shaderParameters)
		SDL_GPURenderState* use(SDL_Renderer& renderer, const void* uniforms, std::size_t size) const;

	private:
		bool load(SDL_Renderer& renderer) const;
		void release() const;

		std::string m_path;
		bool m_hasUniforms;
		std::vector<std::byte> m_uniforms;
		mutable bool m_uniformsChanged = false;
		mutable bool m_failed = false;
		mutable SDL_Renderer* m_renderer = nullptr;
		mutable SDL_GPUDevice* m_device = nullptr;
		mutable SDL_GPUShader* m_shader = nullptr;
		mutable SDL_GPURenderState* m_state = nullptr;
	};
}
