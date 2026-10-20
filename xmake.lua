set_project("hremu")
set_languages("c17")

includes("lib")

add_requires(
	"zlog 1.2.18",
	"argtable3 3.2.2"
	--"zlog 1.2.18",
	--"stumpless-git",
	--"agar 1.7.1"
	--"libagar-git"
)

add_requires("libagar-git", {configs = {shared = true}})

add_rules("plugin.compile_commands.autoupdate", {outputdir = "$(builddir)"})
add_rules("mode.debug", "mode.release")

if is_mode("debug") then
	add_defines("DEBUG_BUILD")
end

includes("src")
includes("tests")
