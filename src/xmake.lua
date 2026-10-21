target("hremu")
	set_kind("binary")

	add_files(
		"*.c",
		"cpu/*.c",
		"dbg/*.c",
		"gui/*.c",
		"isa/*.c",
		"mem/*.c"
	)

	add_includedirs(".")

	add_packages(
		"argtable3",
		"libagar-git",
		"iniparser"
	)

	-- add libagar rpath to executable
	on_load(function(target)
		local pkg = target:pkg("libagar-git")
		if pkg then
			target:add("rpathdirs", pkg:installdir("lib") .. "/lib")
		end
	end)
target_end()
