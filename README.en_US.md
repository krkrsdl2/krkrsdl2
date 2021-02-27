# Kirikiri SDL2

Kirikiri SDL2 is a port of [Kirikiri Z](https://krkrz.github.io/) that can be run on platforms supporting [SDL2](https://www.libsdl.org/), such as macOS and Linux.  

This program can be used alongside a modified version of KAG (Kirikiri Adventure Game) 3.  
For more details, please see the following location: https://github.com/krkrsdl2/kag3  

Kirikiri SDL2 can also be compiled to [WebAssembly](https://webassembly.org/) with [Emscripten](https://emscripten.org/).  
A demonstration project playable in your web browser is available here: https://krkrsdl2.github.io/krkrsdl2/  

## Download

The following builds are automatically built from the latest source code by Github Actions.  

* [Web build](https://github.com/krkrsdl2/krkrsdl2/releases/download/latest/krkrsdl2-web.zip)
* [macOS build](https://github.com/krkrsdl2/krkrsdl2/releases/download/latest/krkrsdl2-macos.zip)
* [macOS Application Bundle build](https://github.com/krkrsdl2/krkrsdl2/releases/download/latest/krkrsdl2-macos-appbundle.zip)
* [Ubuntu build](https://github.com/krkrsdl2/krkrsdl2/releases/download/latest/krkrsdl2-ubuntu.zip)

## Usage

To use the program, execute it in the command line in the same directory as `startup.tjs`:
```bash
/path/to/krkrsdl2
```

A startup directory or archive can be specified on the command line:
```bash
/path/to/krkrsdl2 /path/to/startup/directory
```

Command line arguments can be specified on the command line:
```bash
/path/to/krkrsdl2 -drawthread=4
```

For the web version, place the files from the Web build and `data.xp3` created by [Releaser](https://krkrz.github.io/krkr2doc/kr2doc/contents/Releaser.html) onto a web server. Once that is done, the project can be accessed from a web browser.  

## Cloning

To clone the repository, please use the following command in a terminal:

```bash
git clone --recursive https://github.com/krkrsdl2/krkrsdl2
```
If you do not use the exact command above, source files will be missing files since the project uses Git submodules.

## Building

This project can be built by using the Meson build system.  
For more information about the system, please visit the following location: https://mesonbuild.com/  

Meson toolchain files can be used to cross compile to different platforms, such as when using [Emscripten](https://emscripten.org/).  
For your convenience, Meson toolchain files are located here: https://github.com/krkrsdl2/meson_toolchains  

## Running

Once you have built this project, change directory to one containing `startup.tjs`.  
After that is done, execute the program: `/path/to/krkrsdl2`  

## Original project

Code from this project is based on the following projects:
* [Kirikiri 2](https://github.com/krkrz/krkr2)
* [Kirikiri Z](https://github.com/krkrz/krkrz) dev_multi_platform branch
* [KAGParser](https://github.com/krkrz/KAGParser)
* [SamplePlugin](https://github.com/krkrz/SamplePlugin)
* [wuvorbis](https://github.com/krkrz/wuvorbis)
* [ncbind](https://github.com/wtnbgo/ncbind)
* [fstat](https://github.com/wtnbgo/fstat)
* [json](https://github.com/wtnbgo/json)
* [varfile](https://github.com/wtnbgo/varfile)
* [simde](https://github.com/simd-everywhere/simde)
* [SDL](https://github.com/libsdl-org/SDL)

## IRC Channel

Members of the Kirikiri SDL2 project can be found in the [#krkrsdl2 channel on freenode](https://webchat.freenode.net/?channel=#krkrsdl2).

## License

The code of the Kirikiri SDL2 source (inside the `src` directory) is licensed under the MIT license. Please read `LICENSE` for more information.  
This project contains third-party components. Please view the license file in each component for more information.
