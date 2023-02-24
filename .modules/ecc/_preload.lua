local p = premake

newaction {
	trigger         = "ecc",
	shortname       = "Export compile commands",
	description     = "Export compile_commands.json for language server",
	toolset         = "gcc",

	valid_kinds     = { "ConsoleApp", "WindowedApp", "StaticLib", "SharedLib" },
	valid_languages = { "C", "C++" },
	valid_tools     = {
		cc     = { "clang", "gcc" }
	},

	onStart = function()
		p.indent("  ")
	end,

	execute = function()
		p.modules.ecc.onExecute()
	end
}

return function(cfg)
	return (_ACTION == "ecc")
end
