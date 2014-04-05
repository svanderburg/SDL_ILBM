{ nixpkgs ? <nixpkgs>
, systems ? [ "i686-linux" "x86_64-linux" ]
, libiffJobset ? import ../libiff/release.nix { inherit nixpkgs systems officialRelease; }
, libilbmJobset ? import ../libilbm/release.nix { inherit nixpkgs systems officialRelease; }
, libamivideoJobset ? import ../libamivideo/release.nix { inherit nixpkgs systems officialRelease; }
, SDL_ILBM ? { outPath = ./.; rev = 1234; }
, officialRelease ? false

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

        buildInputs = [ pkgconfig libiff libilbm libamivideo SDL ];
      };
      
    build =
      pkgs.lib.genAttrs systems (system:
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
          buildInputs = [ pkgconfig libiff libilbm libamivideo SDL ];
        }
      );
  };
in
jobs
