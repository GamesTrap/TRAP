premake.modules.generatewayland = {}
local m = premake.modules.generatewayland

local p = premake

function DownloadProgress(total, current)
	local ratio = current / total;
	ratio = math.min(math.max(ratio, 0), 1);
	local percent = math.floor(ratio * 100);

	local downloadedStr = ("%d/%d"):format(current, total)
	local progressBar = ("[" .. ("="):rep(math.floor(percent / 2)) .. (" "):rep(50 - math.floor(percent / 2)) .. "]")

	io.write(("\rProgress: %s %s %.2f%%"):format(progressBar, downloadedStr, percent))
	io.flush()
end

function DownloadProtocolFile(url)
	if os.isfile("temp.xml") then
		os.remove("temp.xml")
	end

	local options =
	{
		progress = DownloadProgress,
		timeout = 15
	}
	local result, responseCode = http.download(url, "temp.xml", options);
	print()

	if result == "OK" then
		term.setTextColor(term.infoColor)
		print("Successfully downloaded " .. url)
		term.setTextColor(nil)
		return true
	else
		term.setTextColor(term.errorColor)
		print("Failed to download " .. url)
		term.setTextColor(nil)
		return false
	end
end

function GenerateProtocol(outputFile)
	if os.isfile("temp.xml") and not os.isfile(outputFile .. ".h") or not os.isfile(outputFile .. "-code.h") then
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

		os.remove("temp.xml")
	end
end

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

	local mirrorURLs = {};
	mirrorURLs["https://gitlab.freedesktop.org/wayland/wayland/-/raw/main/protocol/wayland.xml"] = "https://cgit.freedesktop.org/wayland/wayland/plain/protocol/wayland.xml"
	mirrorURLs["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/stable/xdg-shell/xdg-shell.xml"] = "https://cgit.freedesktop.org/wayland/wayland-protocols/plain/stable/xdg-shell/xdg-shell.xml"
	mirrorURLs["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml"] = "https://cgit.freedesktop.org/wayland/wayland-protocols/plain/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml"
	mirrorURLs["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/stable/viewporter/viewporter.xml"] = "https://cgit.freedesktop.org/wayland/wayland-protocols/plain/stable/viewporter/viewporter.xml"
	mirrorURLs["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/unstable/relative-pointer/relative-pointer-unstable-v1.xml"] = "https://cgit.freedesktop.org/wayland/wayland-protocols/plain/unstable/relative-pointer/relative-pointer-unstable-v1.xml"
	mirrorURLs["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml"] = "https://cgit.freedesktop.org/wayland/wayland-protocols/plain/unstable/pointer-constraints/pointer-constraints-unstable-v1.xml"
	mirrorURLs["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/unstable/idle-inhibit/idle-inhibit-unstable-v1.xml"] = "https://cgit.freedesktop.org/wayland/wayland-protocols/plain/unstable/idle-inhibit/idle-inhibit-unstable-v1.xml"
	mirrorURLs["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/staging/xdg-activation/xdg-activation-v1.xml"] = "https://cgit.freedesktop.org/wayland/wayland-protocols/plain/staging/xdg-activation/xdg-activation-v1.xml"
	mirrorURLs["https://gitlab.freedesktop.org/wayland/wayland-protocols/-/raw/main/staging/content-type/content-type-v1.xml"] = "https://cgit.freedesktop.org/wayland/wayland-protocols/plain/staging/content-type/content-type-v1.xml"

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
			local downloaded = false
			if DownloadProtocolFile(protocolURL) == false then
				term.setTextColor(term.warningColor)
				print("Retrying download using mirror " .. mirrorURLs[protocolURL])
				term.setTextColor(nil)

				downloaded = DownloadProtocolFile(mirrorURLs[protocolURL])
			else
				downloaded = true
			end

			if downloaded then
				GenerateProtocol(outputFile)
			end
		end
	end
end

return m