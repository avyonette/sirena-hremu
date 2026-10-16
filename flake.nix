{
	inputs = {
		nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
		flake-utils.url = "github:numtide/flake-utils";
	};

	outputs = { self, nixpkgs, flake-utils }:
		flake-utils.lib.eachSystem [ "x86_64-linux" ] (system:
			let
				pkgs = import nixpkgs {
					inherit system;

					config = {
						allowUnfree = true;
						allowUnsupportedSystem = true;
					};
				};
			in {
				devShells.default = pkgs.mkShell rec {
					name = "native";

					hardeningDisable = ["fortify" "fortify3"];

					nativeBuildInputs = with pkgs; [
						xmake
						ninja
						pkg-config
						cmake
						git
						
						llvmPackages_22.clang-tools
						llvmPackages_22.clang
						llvmPackages_22.lldb
					];

					buildInputs = with pkgs; [
						llvmPackages_22.clang
						libcxx
					];

					LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath buildInputs;

					shellHook = ''
						export CC=$(which clang)
						export CXX=$(which clang++)
						
						if [ -f ".vscode/settings.json" ]; then
							echo "(i) switching vscode build configuration"
							jq ".\"clangd.arguments\" = [\"--compile-commands-dir=build\", \"--query-driver=$CC,$CXX\"]" .vscode/settings.json | sponge .vscode/settings.json
						fi
					'';
				};
			}
		);
}
