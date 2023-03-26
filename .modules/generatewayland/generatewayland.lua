premake.modules.generatewayland = {}
local m = premake.modules.generatewayland

local p = premake

function m.GenerateWayland()
    if os.target() ~= "linux" then
        return
    end

    local files = {};
	files["https://gitlab.freedesktop.org/wayland/wayland/-/raw/main/protocol/wayland.xml"] = "../Dependencies/Wayland/wayland-client-protocol"
	files["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/stable/xdg-shell/xdg-shell.xml"] = "../Dependencies/Wayland/wayland-xdg-shell-client-protocol"
	files["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml"] = "../Dependencies/Wayland/wayland-xdg-decoration-client-protocol"
	files["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/stable/viewporter/viewporter.xml"] = "../Dependencies/Wayland/wayland-viewporter-client-protocol"
	files["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/unstable/relative-pointer/relative-pointer-unstable-v1.xml"] = "../Dependencies/Wayland/wayland-relative-pointer-unstable-v1-client-protocol"
	files["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml"] = "../Dependencies/Wayland/wayland-pointer-constraints-unstable-v1-client-protocol"
	files["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/unstable/idle-inhibit/idle-inhibit-unstable-v1.xml"] = "../Dependencies/Wayland/wayland-idle-inhibit-unstable-v1-client-protocol"
	files["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/staging/xdg-activation/xdg-activation-v1.xml"] = "../Dependencies/Wayland/wayland-xdg-activation-v1-client-protocol"
	files["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/staging/content-type/content-type-v1.xml"] = "../Dependencies/Wayland/wayland-content-type-v1-client-protocol"

	local result, errorCode = os.outputof("wayland-scanner --version")
	if errorCode ~= 0 then
		term.setTextColor(term.errorColor)
		print("Unable to call wayland-scanner from shell!")
		term.setTextColor(nil)
		return
	end

	if not os.isdir("../Dependencies/Wayland") then
		os.mkdir("../Dependencies/Wayland")
	end

	for protocolURL,outputFile in pairs(files) do
		if not os.isfile(outputFile .. ".h") or not os.isfile(outputFile .. "-code.h") then
			local result, responseCode = http.download(protocolURL, "temp.xml");

			if result == "OK" then
				local res, errorCode = os.outputof("wayland-scanner client-header \"temp.xml\" \"" .. outputFile .. ".h\"");
				if errorCode ~= 0 then
					term.setTextColor(term.errorColor)
					print("Failed to generate " .. outputFile .. ".h")
					term.setTextColor(nil)
				else
					term.setTextColor(term.infoColor)
					print("Successfully generated " .. outputFile .. ".h")
					term.setTextColor(nil)
				end
				local res, errorCode = os.outputof("wayland-scanner private-code \"temp.xml\" \"" .. outputFile .. "-code.h\"");
				if errorCode ~= 0 then
					term.setTextColor(term.errorColor)
					print("Failed to generate " .. outputFile .. "-code.h")
					term.setTextColor(nil)
				else
					term.setTextColor(term.infoColor)
					print("Successfully generated " .. outputFile .. "-code.h")
					term.setTextColor(nil)
				end

				os.remove("temp.xml");
			else
				term.setTextColor(term.errorColor)
				print("Failed to download " .. protocolURL)
				term.setTextColor(nil)
			end
		end
	end
end

return m