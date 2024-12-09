{
  description = "Development environment with bear, OpenGL, SDL, and clangd";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            # C/C++ Development Tools
            bear
            # cmake
            # gnumake
            clang-tools
            clang
            pkg-config
            #llvmPackages_19.clangUseLLVM

            # Graphics and Multimedia Libraries
            SDL2
            SDL2_ttf
            SDL2_image
            SDL2_mixer

            # OpenGL Support
            mesa
          ];

          # Set up environment variables for OpenGL and SDL
          shellHook = ''
            export PS1="\[\033[1;32m\][nix-dev:\w]\$ \[\033[0m\]"
            echo "> Development Environment Activated"
          '';

          # Ensure proper library paths
          LD_LIBRARY_PATH = "${pkgs.libGL}/lib:${pkgs.mesa}/lib";
        };
      }
    );
}
