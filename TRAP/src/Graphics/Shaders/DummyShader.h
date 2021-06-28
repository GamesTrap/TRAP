#ifndef _TRAP_DUMMYSHADER_H_
#define _TRAP_DUMMYSHADER_H_

#include "Shader.h"

namespace TRAP::Graphics
{
	class DummyShader final : public Shader
	{
	public:
		DummyShader(const std::string& name, const std::string& filepath);
		~DummyShader() override = default;

		void Use(Window* window = nullptr) override;
		void UseTexture(uint32_t binding, const TRAP::Graphics::Texture* texture) override;
	};
}

#endif /*_TRAP_DUMMYSHADER_H_*/