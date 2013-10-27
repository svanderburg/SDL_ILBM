{ nixpkgs ? <nixpkgs>
, systems ? [ "i686-linux" "x86_64-linux" ]
}:

let
  pkgs = import nixpkgs {};
  
  version = builtins.readFile ./version;
  
  jobs = rec {
    tarball =
      { SDL_ILBM ? {outPath = ./.; rev = 1234;}
      , officialRelease ? false
      , libiff ? builtins.getAttr (builtins.currentSystem) ((import ../libiff/release.nix {}).build {})
      , libilbm ? builtins.getAttr (builtins.currentSystem) ((import ../libilbm/release.nix {}).build {}) {}
      , libamivideo ? builtins.getAttr (builtins.currentSystem) ((import ../libamivideo/release.nix {}).build {})
      }:

      with pkgs;

      releaseTools.sourceTarball {
        name = "SDL_ILBM-tarball";
        src = SDL_ILBM;
        inherit version officialRelease;

        buildInputs = [ pkgconfig libiff libilbm libamivideo SDL ];
      };
      
    build =
      { tarball ? jobs.tarball {} }:
      
      pkgs.lib.genAttrs systems (system:
        { libiff ? builtins.getAttr system ((import ../libiff/release.nix {}).build {})
        , libilbm ? builtins.getAttr system ((import ../libilbm/release.nix {}).build {}) {}
        , libamivideo ? builtins.getAttr system ((import ../libamivideo/release.nix {}).build {})
        }:
        
        with import nixpkgs { inherit system; };
        
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
