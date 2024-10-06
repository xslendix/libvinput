let
  nixpkgs = fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-unstable";
  pkgs = import nixpkgs { config = {}; overlays = []; };
in

pkgs.mkShellNoCC {
  packages = with pkgs; [
		clang-tools
		clang
		gnumake
		pkg-config

		xdo
		xdotool
		xorg.libXtst
		xorg.libX11
		xorg.libxcb
		xorg.xinput
		xorg.libXi
		libevdev
		libxkbcommon
  ];
}
