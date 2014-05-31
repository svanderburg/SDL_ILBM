{ nixpkgs ? <nixpkgs>
, systems ? [ "i686-linux" "x86_64-linux" ]
, libiffJobset ? import ../libiff/release.nix { inherit nixpkgs systems officialRelease buildForWindows; }
, libilbmJobset ? import ../libilbm/release.nix { inherit nixpkgs systems officialRelease buildForWindows; }
, libamivideoJobset ? import ../libamivideo/release.nix { inherit nixpkgs systems officialRelease buildForWindows; }
, SDL_ILBM ? { outPath = ./.; rev = 1234; }
, officialRelease ? false
, buildForWindows ? false
}:

let
  pkgs = import nixpkgs {};
  
  version = builtins.readFile ./version;
  
  jobs = rec {
    tarball =
      with pkgs;

      let
        libiff = builtins.getAttr (builtins.currentSystem) (libiffJobset.build);
        libilbm = builtins.getAttr (builtins.currentSystem) (libilbmJobset.build);
        libamivideo = builtins.getAttr (builtins.currentSystem) (libamivideoJobset.build);
      in
      releaseTools.sourceTarball {
        name = "SDL_ILBM-tarball";
        src = SDL_ILBM;
        inherit version officialRelease;

        buildInputs = [ pkgconfig libiff libilbm libamivideo SDL2 ];
      };
      
    build =
      (pkgs.lib.genAttrs systems (system:
        with import nixpkgs { inherit system; };
        
        let
          libiff = builtins.getAttr system (libiffJobset.build);
          libilbm = builtins.getAttr system (libilbmJobset.build);
          libamivideo = builtins.getAttr system (libamivideoJobset.build);
        in
        releaseTools.nixBuild {
          name = "SDL_ILBM";
          inherit version;
          src = tarball;
          buildInputs = [ pkgconfig libiff libilbm libamivideo SDL2 ];
          CFLAGS = "-ansi -pedantic -Wall";
        }
      )) //
      (pkgs.lib.optionalAttrs (buildForWindows) { i686-windows =
         let
           libiff = libiffJobset.build.i686-windows;
           libilbm = libilbmJobset.build.i686-windows;
           libamivideo = libamivideoJobset.build.i686-windows;
           SDL2devel = pkgs.stdenv.mkDerivation {
             name = "SDL2-devel-2.0.3";
             src = pkgs.fetchurl {
               url = http://www.libsdl.org/release/SDL2-devel-2.0.3-VC.zip;
               sha256 = "0q6fs678i59xycjlw7blp949dl0p2f1y914prpbs1cspz98x3pld";
             };
             buildInputs = [ pkgs.unzip ];
             installPhase = ''
               mkdir -p $out
               mv * $out
             '';
             dontStrip = true;
           };
         in
           pkgs.dotnetenv.buildSolution {
             name = "SDL_ILBM";
             src = ./.;
             baseDir = "src";
             slnFile = "SDL_ILBM.sln";
             preBuild = ''
               export msBuildOpts="/p:libiffIncludePath=\"$(cygpath --windows ${libiff}/include)\" /p:libiffLibPath=\"$(cygpath --windows ${libiff})\""
               export msBuildOpts="$msBuildOpts /p:libilbmIncludePath=\"$(cygpath --windows ${libilbm}/include)\" /p:libilbmLibPath=\"$(cygpath --windows ${libilbm})\""
               export msBuildOpts="$msBuildOpts /p:libamivideoIncludePath=\"$(cygpath --windows ${libamivideo}/include)\" /p:libamivideoLibPath=\"$(cygpath --windows ${libamivideo})\""
               export msBuildOpts="$msBuildOpts /p:SDL2IncludePath=\"$(cygpath --windows ${SDL2devel}/include)\" /p:SDL2LibPath=\"$(cygpath --windows ${SDL2devel}/lib/x86)\""
             '';
             postInstall = ''
               mkdir -p $out/include/SDL_ILBM
               cp -v SDL_ILBM/*.h $out/include/SDL_ILBM
             '';
           };
        });
  };
in
jobs
