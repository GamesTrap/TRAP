#ifndef TRAP_IMGUILAYER_H
#define TRAP_IMGUILAYER_H

#ifndef TRAP_HEADLESS_MODE

#include <imgui.h>

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

	/// @remark @headless This class is not available in headless mode.
	class ImGuiLayer final : public Layer
	{
	public:
		/// @brief Constructor.
		constexpr ImGuiLayer();

		/// @brief Copy constructor.
		consteval ImGuiLayer(const ImGuiLayer&) noexcept = delete;
		/// @brief Copy assignment operator.
		consteval ImGuiLayer& operator=(const ImGuiLayer&) noexcept = delete;
		/// @brief Move constructor.
		consteval ImGuiLayer(ImGuiLayer&&) noexcept = delete;
		/// @brief Move assignment operator.
		consteval ImGuiLayer& operator=(ImGuiLayer&&) noexcept = delete;

		/// @brief Destructor
		~ImGuiLayer() override = default;

		/// @brief Called when pushed to a layer stack.
		void OnAttach() override;
		/// @brief Called when popped from a layer stack.
		void OnDetach() override;
		/// @brief Called when an event occurs.
		/// @param event Event that occurred.
		void OnEvent(Events::Event& event) override;

		/// @brief Begin a new ImGui frame.
		static void Begin();
		/// @brief End an ImGui frame.
		static void End();

		/// @brief Block ImGui event handling.
		/// @param block Whether to block events or not
		constexpr void BlockEvents(bool block) noexcept;

		/// @brief Sets a dark theme for ImGui.
		static void SetDarkThemeColors();
		/// @brief Sets the style for ImGuizmo.
		static void SetImGuizmoStyle();
	private:
		bool m_blockEvents = true;

		std::string m_imguiIniPath = "imgui.ini";

		TRAP::Ref<TRAP::Graphics::PipelineCache> m_imguiPipelineCache = nullptr;
	};
}

namespace ImGui
{
	/// @brief Draw an image with ImGui.
	/// @param image Image to draw.
	/// @param sampler Sampler to use.
	/// @param size Size for the image.
	/// @param uv0 UV0 coordinates.
	/// @param uv1 UV1 coordinates.
	/// @param tint_col Tint color.
	/// @param border_col Border color.
	/// @remark @headless This function is not available in headless mode.
	void Image(const TRAP::Ref<TRAP::Graphics::Texture>& image,
	           const TRAP::Ref<TRAP::Graphics::Sampler>& sampler,
	           const ImVec2& size,
	           const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1),
			   const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
			   const ImVec4& border_col = ImVec4(0, 0, 0, 0));
	/// @brief Draw an image with ImGui.
	/// @param image Image to draw.
	/// @param size Size for the image.
	/// @param uv0 UV0 coordinates.
	/// @param uv1 UV1 coordinates.
	/// @param tint_col Tint color.
	/// @param border_col Border color.
	/// @remark @headless This function is not available in headless mode.
	void Image(const TRAP::Ref<TRAP::Graphics::Texture>& image, const ImVec2& size,
	           const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1),
			   const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
			   const ImVec4& border_col = ImVec4(0, 0, 0, 0));

	/// @brief Draw an image button with ImGui.
	/// @param image Image to draw.
	/// @param size Size for the image.
	/// @param uv0 UV0 coordinates.
	/// @param uv1 UV1 coordinates.
	/// @param bg_col Background color.
	/// @param tint_col Tint color.
	/// @return True if image button is pressed, false otherwise.
	/// @remark @headless This function is not available in headless mode.
    bool ImageButton(const TRAP::Ref<TRAP::Graphics::Texture>& image, const ImVec2& size,
	                 const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1,1),
					 const ImVec4& bg_col = ImVec4(0,0,0,0),
					 const ImVec4& tint_col = ImVec4(1,1,1,1));

	/// @brief Draw an input text field with ImGui.
	/// @param label Label for the text field.
	/// @param str String to store input to.
	/// @param flags Additional flags for the input field.
	/// @param callback Callback to implement additional functionality.
	/// @param userData Pointer to provide user data.
	/// @return True if input text changed, false otherwise.
	/// @remark @headless This function is not available in headless mode.
	bool InputText(std::string_view label, std::string& str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* userData = nullptr);
	/// @brief Draw an multi-line input text field with ImGui.
	/// @param label Label for the text field.
	/// @param str String to store input to.
	/// @param size Size for the text field.
	/// @param flags Additional flags for the input field.
	/// @param callback Callback to implement additional functionality.
	/// @param userData Pointer to provide user data.
	/// @return True if input text changed, false otherwise.
	/// @remark @headless This function is not available in headless mode.
	bool InputTextMultiline(std::string_view label, std::string& str, const ImVec2& size = ImVec2(0.0f, 0.0f), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* userData = nullptr);
	/// @brief Draw an input text field with an additional hint with ImGui.
	/// @param label Label for the text field.
	/// @param hint Hint to display.
	/// @param str String to store input to.
	/// @param flags Additional flags for the input field.
	/// @param callback Callback to implement additional functionality.
	/// @param userData Pointer to provide user data.
	/// @return True if input text changed, false otherwise.
	/// @remark @headless This function is not available in headless mode.
	bool InputTextWithHint(std::string_view label, std::string_view hint, std::string& str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* userData = nullptr);

	/// @brief Add a TTF font to ImGui from file.
	/// @param filename Path to TTF file.
	/// @param sizePixels Font size in pixels.
	/// @param fontCfgTemplate Font configuration template.
	/// @param glyphRanges Glyph ranges.
	/// @return Pointer to the new font.
	/// @remark @headless This function is not available in headless mode.
	ImFont* AddFontFromFileTTF(std::string_view filename, f32 sizePixels,
		                       const ImFontConfig* fontCfgTemplate = nullptr,
						       const ImWchar* glyphRanges = nullptr);
	/// @brief Add a TTF font to ImGui from memory.
	/// @param fontData Pointer to TTF data.
	/// @param sizePixels Font size in pixels.
	/// @param fontCfg Font configuration.
	/// @param glyphRanges Glyph ranges.
	/// @return Pointer to the new font.
	/// @remark @headless This function is not available in headless mode.
	ImFont* AddFontFromMemoryTTF(std::span<u8> fontData, f32 sizePixels,
							     const ImFontConfig* fontCfg = nullptr,
							     const ImWchar* glyphRanges = nullptr);

	/// @brief Retrieve whether ImGui input is enabled or not.
	/// @return True if input is enabled, false otherwise.
	/// @remark @headless This function is not available in headless mode.
	[[nodiscard]] bool IsInputEnabled();
	/// @brief Enable or disable ImGui input.
	/// @param enable Enable or disable.
	/// @remark @headless This function is not available in headless mode.
	void SetInputEnabled(bool enable);
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr TRAP::ImGuiLayer::ImGuiLayer()
	: Layer("ImGuiLayer")
{
}

//-------------------------------------------------------------------------------------------------------------------//

constexpr void TRAP::ImGuiLayer::BlockEvents(const bool block) noexcept
{
	m_blockEvents = block;
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_IMGUILAYER_H*/
