# digilogic

This is the C version of (digilogic)[https://github.com/rj45/digilogic].

** THIS VERSION IS NO LONGER BEING DEVELOPED **

We switched to Rust from C. Mainly because building C is hard (especially when you support multiple platforms), and dependency management was taking a lot of time (especially getting things to build). Rust is annoyingly complex, but `cargo` is so damn good, the complexity seems worth it.

## Building

Choose your own adventure: `Zig` or `cmake`. The zig build will statically compile the latest freetype, so if it works, that is probably the best option. If you don't want to install zig, and you have cmake, you can try that.

### Using Zig

You will need rust installed via rustup, zig, and on windows you'll need MSVC.

#### Mac

```sh
git clone --recurse-submodules https://github.com/rj45/digilogic.git
cd digilogic
zig build --release
open zig-out/digilogic.app
```

You can move the app in `zig-out` to your `Applications` folder if you want to.

#### Windows

```sh
git clone --recurse-submodules https://github.com/rj45/digilogic.git
cd digilogic
zig build --release
zig-out\digilogic.exe
```

If you get a crash immediately, try disabling MSAA:

```sh
zig build --release "-Dmsaa_sample_count=1"
```

#### Linux

Only the X11 mode works in the zig build. If you need wayland, it's highly recommended to use Xwayland, but if you want native wayland support, cmake is your best bet (see below). You can try `zig build -Dwayland=true` and see if you can get it to work.

```sh
git clone --recurse-submodules https://github.com/rj45/digilogic.git
cd digilogic
zig build --release
zig-out\digilogic
```

### Using cmake

You'll need rust via rustup, and cmake. Windows you'll need MSVC. You need FreeType too.

#### Mac

(You can also follow the linux instructions to avoid XCode and use make.)

```sh
git clone --recurse-submodules https://github.com/rj45/digilogic.git
cd digilogic
mkdir build
cd build
cmake .. -G Xcode -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

#### Windows

You will need cmake and MSVC installed.

You you can also either use vcpkg or install freetype and pass the info to cmake.

vcpkg is shown below, assuming vcpkg is installed at `c:\Users\User\Documents\vcpkg`.


```sh
git clone --recurse-submodules https://github.com/rj45/digilogic.git
cd digilogic
cmake -B build -DCMAKE_BUILD_TYPE=Release "-DCMAKE_TOOLCHAIN_FILE=c:/Users/User/Documents/vcpkg/scripts/buildsystems/vcpkg.cmake" "-DVCPKG_TARGET_TRIPLET=x64-windows-static-md"
cmake --build build --config Release
build\Release\digilogic.exe
```

#### Linux X11 (Xorg)

You will need FreeType, cmake and build essentials installed. You may also need dev libraries for X11.

Note: Older versions of FreeType may cause font rendering issues, need to investigate if there is a fix.

```sh
git clone --recurse-submodules https://github.com/rj45/digilogic.git
cd digilogic
mkdir build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
./digilogic
```

#### Linux Wayland

Note: For Gnome, I recommend using Xwayland and compiling for X11 rather than native Wayland. This will give you window decorations. If you don't use Gnome, then you won't have this problem and Wayland will work fine for you. I welcome contributions to get libdecor working for Gnome+Wayland.

You will need FreeType, cmake and build essentials installed. You may also need dev libraries for Wayland.

```sh
git clone --recurse-submodules https://github.com/rj45/digilogic.git
cd digilogic
mkdir build
cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_WAYLAND=1
make
./digilogic
```

## License

Apache 2.0, see [NOTICE](./NOTICE) and [LICENSE](./LICENSE).