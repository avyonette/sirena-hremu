target("hremu")
	set_kind("binary")

	add_files(
		"*.cpp",
		"*.mpp"
	)

	add_includedirs(".")

	add_packages("spdlog")
target_end()
