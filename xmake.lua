set_project("hremu")
set_languages("c17", "cxx26")

add_requires(
	"spdlog 1.17.0"
)

add_rules("plugin.compile_commands.autoupdate", {outputdir = "$(builddir)"})
add_rules("mode.debug", "mode.release")

if is_mode("debug") then
	add_defines("DEBUG_BUILD")
end

includes("src")
includes("tests")
