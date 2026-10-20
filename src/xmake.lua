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
		"stumpless",
		"libagar"
	)
target_end()
