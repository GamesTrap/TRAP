
local p = premake

p.modules.ecc = {}
local m = p.modules.ecc

local workspace = p.workspace
local project = p.project

m._VERSION = "1.0.0-alpha"

newoption {
	trigger = "config",
	value = "CFG",
	description = "Select config for export compile_commands.json"
}

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

	-- onStart = function()
	-- 	p.indent("  ")
	-- end,

	execute = function()
		m.onExecute()
	end
}

function m.getCommonFlags(prj, cfg)
	-- Some tools that consumes compile_commands.json have problems with relative include paths
	local relative = project.getrelative
	project.getrelative = function(prj, dir) return dir end

	local toolset = m.getToolSet(cfg)
	local tool = iif(project.iscpp(prj), "cxx", "cc")
	local toolname = iif(cfg.prefix, toolset.gettoolname(cfg, tool), toolset.tools[tool])
	local flags = toolname
	flags = table.join(flags, toolset.getcppflags(cfg))
	flags = table.join(flags, toolset.getdefines(cfg.defines))
	flags = table.join(flags, toolset.getundefines(cfg.undefines))
	flags = table.join(flags, toolset.getincludedirs(cfg, cfg.includedirs, cfg.sysincludedirs))
	flags = table.join(flags, toolset.getforceincludes(cfg))
	if (project.iscpp(prj)) then
		flags = table.join(flags, toolset.getcxxflags(cfg))
	elseif (project.isc(prj)) then
		flags = table.join(flags, toolset.getcflags(cfg))
	end
	flags = table.join(flags, cfg.buildoptions)
	project.getrelative = relative
	return flags
end

function m.getObjectPath(prj, cfg, node)
	return path.join(cfg.objdir, path.appendExtension(node.objname, '.o'))
end

function m.getDependenciesPath(prj, cfg, node)
	return path.join(cfg.objdir, path.appendExtension(node.objname, '.d'))
end

function m.getFileFlags(prj, cfg, node)
	return table.join(m.getCommonFlags(prj, cfg), {
		'-o', m.getObjectPath(prj, cfg, node),
		'-MF', m.getDependenciesPath(prj, cfg, node),
		'-c', node.abspath
	})
end

function m.getToolSet(cfg)
	local default = iif(cfg.system == p.MACOSX, "clang", "gcc")
	local toolset = p.tools[_OPTIONS.cc or cfg.toolset or default]
	if not toolset then
		error("Invalid toolset '" .. cfg.toolset .. "'")
	end
	return toolset
end

function m.getToolName(prj)
	local toolset = m.getToolSet(cfg)
	local tool = iif(project.iscpp(prj), "cxx", "cc")
	local toolname = iif(cfg.prefix, toolset.gettoolname(cfg, tool), toolset.tools[tool])
	return toolname
end

function m.generateCompileCommand(prj, cfg, node)
	return {
		directory = prj.location,
		file = node.abspath,
		command = table.concat(m.getFileFlags(prj, cfg, node), ' ')
	}
end

function m.includeFile(prj, node, depth)
	return path.iscppfile(node.abspath) or path.iscfile(node.abspath)
end

function m.getProjectCommands(prj, cfg)
	local tr = project.getsourcetree(prj)
	local cmds = {}
	p.tree.traverse(tr, {
		onleaf = function(node, depth)
			if not m.includeFile(prj, node, depth) then
				return
			end
			table.insert(cmds, m.generateCompileCommand(prj, cfg, node))
		end
	})
	return cmds
end

function m.onExecute()
	for wks in p.global.eachWorkspace() do
		local cfgCmds
		for prj in workspace.eachproject(wks) do
			local cfg = project.getconfig(prj, "Debug")
			if not cfg then
				error("No debug configuration for project '" .. prj.name .. "'")
			else
				cfgCmds = table.join(cfgCmds, m.getProjectCommands(prj, cfg))
			end
		end
		local outfile = "compile_commands.json"
		p.generate(wks, outfile, function(wks)
			p.push('[')
			for i = 1, #cfgCmds do
				local item = cfgCmds[i]
				p.push('{')
				p.x('"directory": "%s",', item.directory)
				p.x('"file": "%s",', item.file)
				p.w('"command": "%s"', item.command)
				if i ~= #cfgCmds then
					p.pop('},')
				else
					p.pop('}')
				end
			end
			p.pop(']')
		end
		)
	end
end

return m