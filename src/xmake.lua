target("hremu")
	set_kind("binary")

	add_files(
		"main.c",
		"cpu/*.c",
		"dbg/*.c",
		"isa/*.c",
		"mem/*.c"
	)

	add_includedirs(".")

	add_packages("zlog", "argtable3")
target_end()
