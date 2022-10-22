#ifndef TRAP_IMGUILAYER_H
#define TRAP_IMGUILAYER_H

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif
//ImGUI
#include <imgui.h>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#include "Graphics/API/Vulkan/Utils/VulkanLoader.h"
#include "Core/Base.h"
#include "Layers/Layer.h"

namespace TRAP
{
	namespace Graphics
	{
		class PipelineCache;
		class Texture;
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
		/// Set the MSAA sample count.
		/// </summary>
		/// <param name="sampleCount">Sample count to use.</param>
		void SetMSAASamples(uint32_t sampleCount);

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

		/// <summary>
		/// Sets a dark theme for ImGui.
		/// </summary>
		static void SetDarkThemeColors();
	private:
		bool m_blockEvents;
		float m_time = 0.0f;

		std::string m_imguiIniPath;

		TRAP::Ref<TRAP::Graphics::PipelineCache> m_imguiPipelineCache;
		VkDescriptorPool m_imguiDescriptorPool;

		const std::vector<VkDescriptorPoolSize> m_descriptorPoolSizes =
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
	/// <summary>
	/// Draw an image with ImGui.
	/// </summary>
	/// <param name="image">Image to draw.</param>
	/// <param name="sampler">Sampler to use.</param>
	/// <param name="size">Size for the image.</param>
	/// <param name="uv0">UV0 coordinates.</param>
	/// <param name="uv1">UV1 coordinates.</param>
	/// <param name="tint_col">Tint color.</param>
	/// <param name="border_col">Border color.</param>
	void Image(TRAP::Ref<TRAP::Graphics::Texture> image, const TRAP::Graphics::Sampler* sampler,
	           const ImVec2& size,
	           const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1),
			   const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
			   const ImVec4& border_col = ImVec4(0, 0, 0, 0));
	/// <summary>
	/// Draw an image with ImGui.
	/// </summary>
	/// <param name="image">Image to draw.</param>
	/// <param name="size">Size for the image.</param>
	/// <param name="uv0">UV0 coordinates.</param>
	/// <param name="uv1">UV1 coordinates.</param>
	/// <param name="tint_col">Tint color.</param>
	/// <param name="border_col">Border color.</param>
	void Image(TRAP::Ref<TRAP::Graphics::Texture> image, const ImVec2& size,
	           const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1),
			   const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
			   const ImVec4& border_col = ImVec4(0, 0, 0, 0));

	/// <summary>
	/// Draw an image button with ImGui
	/// </summary>
	/// <param name="image">Image to draw.</param>
	/// <param name="size">Size for the image.</param>
	/// <param name="uv0">UV0 coordinates.</param>
	/// <param name="uv1">UV1 coordinates.</param>
	/// <param name="frame_padding">Frame padding.</param>
	/// <param name="bg_col">Background color.</param>
	/// <param name="tint_col">Tint color.</param>
	/// <returns>True if image button is pressed, false otherwise.</returns>
    bool ImageButton(TRAP::Ref<TRAP::Graphics::Texture> image, const ImVec2& size,
	                 const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1,1),
					 int frame_padding = -1, const ImVec4& bg_col = ImVec4(0,0,0,0),
					 const ImVec4& tint_col = ImVec4(1,1,1,1));

	/// <summary>
	/// Add a TTF font to ImGui from file.
	/// </summary>
	/// <param name="filename">Path to TTF file.</param>
	/// <param name="sizePixels">Font size in pixels.</param>
	/// <param name="fontCfgTemplate">Font configuration template.</param>
	/// <param name="glyphRanges">Glyph ranges.</param>
	/// <returns>Pointer to the new font.</returns>
	ImFont* AddFontFromFileTTF(const std::string_view filename, float sizePixels,
		                       const ImFontConfig* fontCfgTemplate = nullptr,
						       const ImWchar* glyphRanges = nullptr);
	/// <summary>
	/// Add a TTF font to ImGui from memory.
	/// </summary>
	/// <param name="fontData">Pointer to TTF data.</param>
	/// <param name="fontSize">Size of the TTF data.</param>
	/// <param name="sizePixels">Font size in pixels.</param>
	/// <param name="fontCfg">Font configuration.</param>
	/// <param name="glyphRanges">Glyph ranges.</param>
	/// <returns>Pointer to the new font.</returns>
	ImFont* AddFontFromMemoryTTF(void* fontData, int32_t fontSize, float sizePixels,
							     const ImFontConfig* fontCfg = nullptr,
							     const ImWchar* glyphRanges = nullptr);

	/// <summary>
	/// Retrieve whether ImGui input is enabled or not.
	/// </summary>
	/// <returns>True if input is enabled, false otherwise.</returns>
	bool IsInputEnabled();
	/// <summary>
	/// Enable or disable ImGui input.
	/// </summary>
	/// <param name="enable">Enable or disable.</param>
	void SetInputEnabled(bool enable);
}

#endif /*TRAP_IMGUILAYER_H*/