target("hremu")
	set_kind("binary")

	add_files(
		"main.cpp",
		"mem/*.cpp",
		"cpu/*.cpp"
	)

	add_includedirs(".")

	add_packages("spdlog")
target_end()
