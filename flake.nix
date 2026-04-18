{
  description = "Luminosity Weather Watchface";

  inputs = {
    pebble.url = "git+file:/home/casey/Repos/pebble.nix/";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    { pebble, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system: {
      devShell = pebble.pebbleEnv.${system} { };
    });
}
