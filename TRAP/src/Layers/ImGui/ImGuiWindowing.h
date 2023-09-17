/*
The MIT License(MIT)

Copyright(c) 2014 - 2023 Omar Cornut

Permission is hereby granted, free of charge, to any person obtaining a copy
of this softwareand associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Modified by: Jan "GamesTrap" Schuerkamp
*/

#ifndef TRAP_IMGUIWINDOWING_H
#define TRAP_IMGUIWINDOWING_H

#ifndef TRAP_HEADLESS_MODE

#include <imgui.h>

#include "Core/PlatformDetection.h"
#include "Window/WindowingAPI.h"

namespace TRAP::Graphics
{
	enum class RenderAPI;
}

namespace TRAP::INTERNAL
{
	/// <summary>
	/// Interfacing class between ImGui and the TRAP::INTERNAL::WindowingAPI.
	/// </summary>
	class ImGuiWindowing
	{
	public:
		/// <summary>
		/// Initialize the ImGui TRAP::INTERNAL::WindowingAPI interface.
		/// </summary>
		/// <param name="window">Handle to the internal main window.</param>
		/// <param name="installCallbacks">Whether to set ImGui callbacks or not.</param>
		/// <param name="renderAPI">RenderAPI to be used by ImGui.</param>
		/// <returns>True on successful initialization, false otherwise.</returns>
		[[nodiscard]] static bool Init(WindowingAPI::InternalWindow* window, bool installCallbacks,
		                               Graphics::RenderAPI renderAPI);
		/// <summary>
		/// Shutdown the ImGui TRAP::INTERNAL::WindowingAPI interface.
		/// </summary>
		static void Shutdown();
		/// <summary>
		/// Starts a new ImGui frame.
		/// </summary>
		static void NewFrame();
		/// <summary>
		/// Set a custom cursor for ImGui.
		/// Note: This will make ImGui the owner of the cursor!
		/// </summary>
		/// <param name="cursor">Custom cursor to use.</param>
		static void SetCustomCursor(WindowingAPI::InternalCursor* cursor);
		/// <summary>
		/// Set to true to enable chaining installed callbacks for all windows
		/// (including secondary viewports created y backends or by user).
		///
		/// Note: This is false by default meaning we only chain callbacks for the main viewport.
		///       We cannot set this to true by default because user callbacks code may not be testing
		///       the window parameter of their callback.
		///       If you set this to true your user callback code will need to make sure you are testing
		///       the window parameter.
		/// </summary>
		/// <param name="chainForAllWindows">Whether to enable callback chaining or not.</param>
		static void SetCallbacksChainForAllWindows(bool chainForAllWindows);

		/// <summary>
		/// Install ImGui callbacks.
		/// </summary>
		/// <param name="window">Window to set callbacks for.</param>
		static void InstallCallbacks(WindowingAPI::InternalWindow* window);
		/// <summary>
		/// Restore old callbacks.
		/// </summary>
		/// <param name="window">Window to restore callbacks for.</param>
		static void RestoreCallbacks(WindowingAPI::InternalWindow* window);

		/// <summary>
		/// Check if the given window should chain callbacks.
		/// </summary>
		/// <param name="window">Window to check for chaining.</param>
		/// <returns>True or false.</returns>
		static bool ShouldChainCallback(const WindowingAPI::InternalWindow* window);
	};
}

#endif /*TRAP_HEADLESS_MODE*/

#endif /*TRAP_IMGUIWINDOWING_H*/