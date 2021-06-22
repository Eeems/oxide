# Warning, this will not work on 2.6 yet. If you have updated you'll have to wait until toltec has released a solution that supports the new OS.

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/e22602d27686420e9a71e67cf8d049fd)](https://app.codacy.com/manual/Eeems/oxide?utm_source=github.com&utm_medium=referral&utm_content=Eeems/oxide&utm_campaign=Badge_Grade_Settings)
[![Maintainability](https://api.codeclimate.com/v1/badges/db8574df9b0b8a1100bc/maintainability)](https://codeclimate.com/github/Eeems/oxide/maintainability)
[![rm1](https://img.shields.io/badge/rM1-supported-green)](https://remarkable.com/store/remarkable)
[![rm2](https://img.shields.io/badge/rM2-partial%20support-orange)](https://remarkable.com/store/remarkable-2)
[![opkg](https://img.shields.io/badge/OPKG-oxide-blue)](https://github.com/matteodelabre/toltec)
[![Discord](https://img.shields.io/discord/385916768696139794.svg?label=reMarkable&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/ATqQGfu)

# Oxide
A launcher application for the [reMarkable tablet](https://remarkable.com/).

Head over to the [releases](https://github.com/Eeems/oxide/releases) page for more information on the latest release. You can also see some (likely outdated) [screenshots here](https://github.com/Eeems/oxide/wiki/Screenshots).

Here is a video of it in action: 
[![Oxide v2.0-beta](https://i.imgur.com/1Q9A4NF.png)](https://youtu.be/rIRKgqy21L0 "Oxide v2.0-beta")

## Building

Install the reMarkable toolchain and then run `make release`. It will produce a folder named `release` containing all the output.

### Nix
Works on x86_64-linux or macOS via [nix-docker](https://github.com/LnL7/nix-docker).

```ShellSession
$ nix-build --argstr system 'x86_64-linux'
```
