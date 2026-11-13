# Julia Set Explorer
<img width="460" height="377" alt="image" src="https://github.com/user-attachments/assets/f65f8260-6280-4652-90d3-64efd1a45221" />

An interactive Julia set viewer written in C++ with [SFML 3](https://www.sfml-dev.org/) and a GLSL fragment shader. The fractal is rendered on the GPU, so you can zoom, pan, and change the Julia seed in real time.

## Features

- Real-time Julia set rendering in a fragment shader
- Smooth zooming and click-and-drag panning
- Live control over the Julia seed (the `c` constant)
- Adjustable iteration count, which also scales automatically with zoom level
- "Double emulation": zoom and offset values are passed to the GPU as a float plus a remainder, to extend precision beyond what a single 32-bit float allows
- Console output of FPS, iterations, zoom, and offset once per second

## Controls

| Input | Action |
|-------|--------|
| `W` / `S` | Zoom in / out |
| `A` / `D` | Decrease / increase the seed's real component |
| `Q` / `E` | Decrease / increase the seed's imaginary component |
| `R` / `F` | Increase / decrease the base iteration count |
| Left mouse drag | Pan |

Only one key is handled per frame, so you can't zoom and change the seed at the same time.

## Requirements

- A C++17 compiler (MSVC, GCC, or Clang)
- **SFML 3.0 or newer** (SFML 2.x will not compile this code)
- A GPU and driver with OpenGL support
- A `shader.frag` file in the working directory

### Shader uniforms

`shader.frag` must declare the following uniforms, which `main.cpp` sets every frame:

| Uniform | Type | Purpose |
|---------|------|---------|
| `u_resolution` | `vec2` | Window size in pixels |
| `numIterations` | `int` | Iteration limit |
| `u_seed` | `vec2` | Julia set constant |
| `e_zoomFactor` | `vec2` | Zoom as (float, remainder) |
| `e_offsetX` | `vec2` | X offset as (float, remainder) |
| `e_offsetY` | `vec2` | Y offset as (float, remainder) |
| `u_time` | `float` | Time of the last frame in seconds |

## Building

### Windows (Visual Studio + vcpkg)

1. Install [vcpkg](https://github.com/microsoft/vcpkg) and run `vcpkg integrate install`.
2. Install SFML for your architecture:
   ```powershell
   vcpkg install sfml:x64-windows
   # or, on Windows on ARM:
   vcpkg install sfml:arm64-windows
   ```
3. Open the solution in Visual Studio and set the platform to **x64** or **ARM64** to match.
4. Set **C/C++ → Language → C++ Language Standard** to C++17 or newer.
5. Set **Debugging → Working Directory** to `$(ProjectDir)` so `shader.frag` is found.
6. Build and run with **Ctrl+F5**.

### CMake (Windows, Linux, macOS)

```cmake
cmake_minimum_required(VERSION 3.16)
project(JuliaSet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(julia main.cpp)
target_link_libraries(julia PRIVATE SFML::Graphics SFML::Window SFML::System)

add_custom_command(TARGET julia POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/shader.frag $<TARGET_FILE_DIR:julia>/shader.frag)
```

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cd build && ./julia
```

If CMake can't find SFML, pass `-DCMAKE_PREFIX_PATH=/path/to/sfml`, or with vcpkg, `-DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake`.

### Linux / WSL

Most distributions ship SFML 2.x, so build SFML 3 from source:

```bash
sudo apt install build-essential cmake git pkg-config \
  libxrandr-dev libxcursor-dev libxi-dev libudev-dev \
  libfreetype-dev libopenal-dev libflac-dev libvorbis-dev \
  libgl1-mesa-dev libegl1-mesa-dev

git clone --branch 3.0.0 --depth 1 https://github.com/SFML/SFML.git
cd SFML
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
sudo cmake --install build
```

Then build the project with the CMake steps above. On WSL, GUI windows require WSLg (Windows 11 or recent Windows 10). Keep the project inside the Linux filesystem for faster builds.

## Troubleshooting

- **`SHADER ERROR: loading shader failed`**: `shader.frag` is missing or not in the working directory.
- **`Cannot open include file SFML/Graphics.hpp`**: SFML isn't installed for your platform/architecture, or the IDE isn't using vcpkg.
- **Compile errors on `VideoMode` or `std::optional`**: you are using SFML 2.x or a language standard below C++17.
- **Shader compile errors**: check the `#version` line in `shader.frag`. Some drivers (especially on ARM) support a lower GLSL version.
- **Slow rendering under WSL**: run `glxinfo -B`. If the renderer is `llvmpipe`, you are on software rendering, so update your Windows GPU driver.

## Project structure

```
.
├── main.cpp        # Window, input handling, uniform updates
├── shader.frag     # GLSL fragment shader that draws the Julia set
├── LICENSE
└── README.md
```

## License

This project is released under the [MIT License](LICENSE).

SFML is a separate project distributed under the zlib/libpng license, which is compatible with MIT. SFML is not included in this repository.

## Acknowledgments

- [SFML](https://www.sfml-dev.org/) for windowing, input, and shader support
- This README and the build and setup instructions were written with help from [Claude](https://claude.ai), an AI assistant made by Anthropic.
