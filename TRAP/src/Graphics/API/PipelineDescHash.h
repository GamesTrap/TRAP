#ifndef _TRAP_PIPELINEDESCHASH_H_
#define _TRAP_PIPELINEDESCHASH_H_

#include "RendererAPI.h"
#include "Graphics/Shaders/Shader.h"
#include "Utils/Utils.h"

namespace std
{
	template<> struct hash<TRAP::Graphics::RendererAPI::PipelineDesc>
	{
		std::size_t operator()(const TRAP::Graphics::RendererAPI::PipelineDesc& p) const noexcept
		{
			std::size_t hash = 0;

			TRAP::Utils::HashCombine(hash, p.Type, p.PipelineExtensions, p.PipelineExtensionCount, p.Name);

			if (std::holds_alternative<TRAP::Graphics::RendererAPI::ComputePipelineDesc>(p.Pipeline))
			{
				const TRAP::Graphics::RendererAPI::ComputePipelineDesc& c = std::get<TRAP::Graphics::RendererAPI::ComputePipelineDesc>(p.Pipeline);

				TRAP::Utils::HashCombine(hash, c.ShaderProgram, c.RootSignature);
			}

			if (std::holds_alternative<TRAP::Graphics::RendererAPI::GraphicsPipelineDesc>(p.Pipeline))
			{
				const TRAP::Graphics::RendererAPI::GraphicsPipelineDesc& g = std::get<TRAP::Graphics::RendererAPI::GraphicsPipelineDesc>(p.Pipeline);

				TRAP::Utils::HashCombine(hash,
					g.ShaderProgram,
					g.RootSignature,
					g.RenderTargetCount,
					g.SampleCount,
					g.SampleQuality,
					g.DepthStencilFormat,
					g.PrimitiveTopology,
					g.SupportIndirectCommandBuffer);

				if (g.VertexLayout)
				{
					TRAP::Utils::HashCombine(hash, g.VertexLayout->AttributeCount);

					for(uint32_t i = 0; i < g.VertexLayout->AttributeCount; ++i)
					{
						TRAP::Utils::HashCombine(hash, g.VertexLayout->Attributes[i].Binding, g.VertexLayout->Attributes[i].Location,
							g.VertexLayout->Attributes[i].Offset, g.VertexLayout->Attributes[i].Format, g.VertexLayout->Attributes[i].Rate);
					}
				}

				TRAP::Utils::HashCombine(hash, g.BlendState->RenderTargetMask, g.BlendState->IndependentBlend);
				for(uint32_t i = 0; i < 8; ++i)
				{
					TRAP::Utils::HashCombine(hash, g.BlendState->SrcFactors[i], g.BlendState->DstFactors[i],
						g.BlendState->SrcAlphaFactors[i], g.BlendState->DstAlphaFactors[i], g.BlendState->BlendModes[i],
						g.BlendState->BlendAlphaModes[i], g.BlendState->Masks[i]);
				}
				
				TRAP::Utils::HashCombine(hash, g.DepthState->DepthTest, g.DepthState->DepthWrite, g.DepthState->DepthFunc,
					g.DepthState->StencilTest, g.DepthState->StencilReadMask, g.DepthState->StencilWriteMask,
					g.DepthState->StencilFrontFunc, g.DepthState->StencilFrontFail, g.DepthState->DepthFrontFail,
					g.DepthState->StencilFrontPass, g.DepthState->StencilBackFunc, g.DepthState->StencilBackFail,
					g.DepthState->DepthBackFail, g.DepthState->StencilBackPass);
				
				TRAP::Utils::HashCombine(hash, g.RasterizerState->CullMode, g.RasterizerState->DepthBias,
					g.RasterizerState->SlopeScaledDepthBias, g.RasterizerState->FillMode,
					g.RasterizerState->FrontFace, g.RasterizerState->DepthClampEnable);

				for (const TRAP::Graphics::RendererAPI::ImageFormat i : g.ColorFormats)
					TRAP::Utils::HashCombine(hash, i);
			}

			if (std::holds_alternative<TRAP::Graphics::RendererAPI::RayTracingPipelineDesc>(p.Pipeline))
			{
				const TRAP::Graphics::RendererAPI::RayTracingPipelineDesc& r = std::get<TRAP::Graphics::RendererAPI::RayTracingPipelineDesc>(p.Pipeline);

				//TODO Implement When RayTracing is implemented
			}

			return hash;
		}
	};
}

#endif /*_TRAP_PIPELINEDESCHASH_H_*/