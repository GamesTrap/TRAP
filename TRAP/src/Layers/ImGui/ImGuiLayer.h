#ifndef TRAP_IMGUILAYER_H
#define TRAP_IMGUILAYER_H

#include "Layers/Layer.h"

#include "TRAPPCH.h"

namespace TRAP
{
	namespace Graphics
	{
		class PipelineCache;
		class Texture2D;
		class TextureBase;
		class Sampler;
	}

	class ImGuiLayer final : public Layer
	{
	public:
		/// <summary>
		/// Constructor.
		/// </summary>
		ImGuiLayer();

		/// <summary>
		/// Called when pushed to a layer stack.
		/// </summary>
		void OnAttach() override;
		/// <summary>
		/// Called when popped from a layer stack.
		/// </summary>
		void OnDetach() override;
		/// <summary>
		/// Called when an event occurs.
		/// </summary>
		/// <param name="event">Event that occurred.</param>
		void OnEvent(Events::Event& event) override;

		/// <summary>
		/// Begin a new ImGui frame.
		/// </summary>
		static void Begin();
		/// <summary>
		/// End an ImGui frame.
		/// </summary>
		static void End();

		/// <summary>
		/// Block ImGui event handling.
		/// </summary>
		/// <param name="block">Whether to block events or not</param>
		void BlockEvents(bool block);

		static void SetDarkThemeColors();
	private:
		bool m_blockEvents;
		float m_time = 0.0f;

		std::string m_imguiIniPath;

		TRAP::Ref<TRAP::Graphics::PipelineCache> m_imguiPipelineCache;
		VkDescriptorPool m_imguiDescriptorPool;

		std::vector<VkDescriptorPoolSize> m_descriptorPoolSizes =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
	};
}

namespace ImGui
{
	void Image(TRAP::Graphics::Texture2D* image, TRAP::Graphics::Sampler* sampler,
	           const ImVec2& size,
	           const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1),
			   const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
			   const ImVec4& border_col = ImVec4(0, 0, 0, 0));
	void Image(TRAP::Graphics::Texture2D* image, const ImVec2& size,
	           const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1),
			   const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
			   const ImVec4& border_col = ImVec4(0, 0, 0, 0));

	void Image(TRAP::Graphics::TextureBase* image, TRAP::Graphics::Sampler* sampler,
	           const ImVec2& size,
	           const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1),
			   const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
			   const ImVec4& border_col = ImVec4(0, 0, 0, 0));
	void Image(TRAP::Graphics::TextureBase* image, const ImVec2& size,
	           const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1),
			   const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
			   const ImVec4& border_col = ImVec4(0, 0, 0, 0));

	ImFont* AddFontFromFileTTF(const std::string_view filename, float sizePixels,
		                       const ImFontConfig* fontCfgTemplate = nullptr,
						       const ImWchar* glyphRanges = nullptr);
	ImFont* AddFontFromMemoryTTF(void* fontData, int32_t fontSize, float sizePixels,
							     const ImFontConfig* fontCfg = nullptr,
							     const ImWchar* glyphRanges = nullptr);
}

#endif /*TRAP_IMGUILAYER_H*/