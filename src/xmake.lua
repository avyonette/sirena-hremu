target("hremu")
	set_kind("binary")

	add_files(
		"main.c",
		"mem/*.c",
		"cpu/*.c"
	)

	add_includedirs(".")

	add_packages("zlog")
target_end()
