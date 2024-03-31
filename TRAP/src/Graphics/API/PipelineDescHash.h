#ifndef TRAP_PIPELINEDESCHASH_H
#define TRAP_PIPELINEDESCHASH_H

#include "Graphics/API/Objects/RenderTarget.h"
#include "Graphics/Shaders/Shader.h"
#include "RendererAPI.h"

namespace std
{
	template<>
    struct hash<TRAP::Graphics::RendererAPI::PipelineDesc>
	{
		[[nodiscard]] constexpr usize operator()(const TRAP::Graphics::RendererAPI::PipelineDesc& p) const noexcept
		{
			usize res = 0;

			TRAP::Utils::HashCombine
            (
                res,
                p.Type,
                //p.PipelineExtensions,
                p.PipelineExtensionCount
            );

			if (std::holds_alternative<TRAP::Graphics::RendererAPI::ComputePipelineDesc>(p.Pipeline))
			{
				const auto& c = std::get<TRAP::Graphics::RendererAPI::ComputePipelineDesc>(p.Pipeline);

				TRAP::Utils::HashCombine
                (
                    res,
                    c.ShaderProgram->GetID()
                );
			}

			if (std::holds_alternative<TRAP::Graphics::RendererAPI::GraphicsPipelineDesc>(p.Pipeline))
			{
				const auto& g = std::get<TRAP::Graphics::RendererAPI::GraphicsPipelineDesc>(p.Pipeline);

				TRAP::Utils::HashCombine
                (
                    res,
					g.ShaderProgram->GetID(),
					g.RenderTargetCount,
					g.SampleCount,
					g.SampleQuality,
					g.DepthStencilFormat,
					g.PrimitiveTopology,
                    g.ShadingRate,
                    std::get<0>(g.ShadingRateCombiners),
                    std::get<1>(g.ShadingRateCombiners)
                );

                if(g.ShadingRateTexture)
                {
                    TRAP::Utils::HashCombine
                    (
                        res,
                        g.ShadingRateTexture->GetImageFormat(),
                        g.ShadingRateTexture->GetWidth(),
                        g.ShadingRateTexture->GetHeight()
                    );
                }

				if (g.VertexLayout)
				{
					TRAP::Utils::HashCombine
                    (
                        res,
                        g.VertexLayout->AttributeCount
                    );

					for(u32 i = 0; i < g.VertexLayout->AttributeCount; ++i)
					{
						TRAP::Utils::HashCombine
                        (
                            res,
                            g.VertexLayout->Attributes[i].Binding,
                            g.VertexLayout->Attributes[i].Location,
							g.VertexLayout->Attributes[i].Offset,
                            g.VertexLayout->Attributes[i].Format,
                            g.VertexLayout->Attributes[i].Rate
                        );
					}
				}

				TRAP::Utils::HashCombine
                (
                    res,
                    g.BlendState->RenderTargetMask,
                    g.BlendState->IndependentBlend
                );
				for(u32 i = 0; i < 8; ++i)
				{
					TRAP::Utils::HashCombine
                    (
                        res,
                        g.BlendState->SrcFactors[i],
                        g.BlendState->DstFactors[i],
						g.BlendState->SrcAlphaFactors[i],
                        g.BlendState->DstAlphaFactors[i],
                        g.BlendState->BlendModes[i],
						g.BlendState->BlendAlphaModes[i],
                        g.BlendState->Masks[i]
                    );
				}

				TRAP::Utils::HashCombine
                (
                    res,
                    g.DepthState->DepthTest,
                    g.DepthState->DepthWrite,
                    g.DepthState->DepthFunc,
					g.DepthState->StencilTest,
                    g.DepthState->StencilReadMask,
                    g.DepthState->StencilWriteMask,
					g.DepthState->StencilFrontFunc,
                    g.DepthState->StencilFrontFail,
                    g.DepthState->DepthFrontFail,
					g.DepthState->StencilFrontPass,
                    g.DepthState->StencilBackFunc,
                    g.DepthState->StencilBackFail,
					g.DepthState->DepthBackFail,
                    g.DepthState->StencilBackPass
                );

				TRAP::Utils::HashCombine
                (
                    res,
                    g.RasterizerState->CullMode,
                    g.RasterizerState->DepthBias,
					g.RasterizerState->SlopeScaledDepthBias,
                    g.RasterizerState->FillMode,
					g.RasterizerState->FrontFace,
                    g.RasterizerState->DepthClampEnable
                );

				for (const TRAP::Graphics::API::ImageFormat i : g.ColorFormats)
					TRAP::Utils::HashCombine(res, i);
			}

			//if (std::holds_alternative<TRAP::Graphics::RendererAPI::RayTracingPipelineDesc>(p.Pipeline))
			//{
			//	const auto& r = std::get<TRAP::Graphics::RendererAPI::RayTracingPipelineDesc>(p.Pipeline);

			//	//TODO Implement When RayTracing is implemented
			//}

			return res;
		}
	};
}

#endif /*TRAP_PIPELINEDESCHASH_H*/