# Installation

Clone the repository, preferably with the `--recurse-submodules` option:

```sh
git clone https://github.com/atalantestudio/Trident.git --recurse-submodules
```

Or use the command below to manually fetch the dependencies:

```sh
git submodule update --init --recursive
```

> [!TIP]
> It is recommended to use either Visual Studio or Visual Studio Code when opening Trident.
> Custom tasks and launch configurations have been written for VS Code to help speed up the development.



## Building

Trident comes packaged with Premake 5.0 beta 8.

Trident can be compiled on Windows with Clang/GCC (through `mingw32-make`) and MSVC 2019/2022/2026.

> [!WARNING]
> GCC support is broken, although the project compiles and links fine. See https://github.com/atalantestudio/Trident/issues/59.

### Clang

Run [scripts/premake5_clang.bat](https://github.com/atalantestudio/Trident/tree/main/scripts/premake5_clang.bat) to generate the project files.

### MSVC

To generate the project files, run the script according to your version of the build tools:
- If using the v142 build tools, run [scripts/premake5_vs2019.bat](https://github.com/atalantestudio/Trident/tree/main/scripts/premake5_vs2019.bat).
- If using the v143 build tools, run [scripts/premake5_vs2022.bat](https://github.com/atalantestudio/Trident/tree/main/scripts/premake5_vs2022.bat).
- If using the v145 build tools, run [scripts/premake5_vs2026.bat](https://github.com/atalantestudio/Trident/tree/main/scripts/premake5_vs2026.bat).



## Compiling

### Clang

Make sure [MinGW](https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release) is installed and that `mingw32-make` is exposed in your PATH.

Navigate to either [scripts/Debug](https://github.com/atalantestudio/Trident/tree/main/scripts/Debug), [scripts/Profile](https://github.com/atalantestudio/Trident/tree/main/scripts/Profile) or [scripts/Release](https://github.com/atalantestudio/Trident/tree/main/scripts/Release) depending on your desired configuration.
Then run [mingw32-make.bat](https://github.com/atalantestudio/Trident/tree/main/scripts/Debug/mingw32-make.bat).

### MSVC (CLI)

Make sure `MSBuild` is exposed in your PATH.

Navigate to either [scripts/Debug](https://github.com/atalantestudio/Trident/tree/main/scripts/Debug), [scripts/Profile](https://github.com/atalantestudio/Trident/tree/main/scripts/Profile) or [scripts/Release](https://github.com/atalantestudio/Trident/tree/main/scripts/Release) depending on your desired configuration.
Then run the script according to your version of the build tools:
- If using the v142 build tools, run [vs2019.bat](https://github.com/atalantestudio/Trident/tree/main/scripts/Debug/vs2019.bat).
- If using the v143 build tools, run [vs2022.bat](https://github.com/atalantestudio/Trident/tree/main/scripts/Debug/vs2022.bat).
- If using the v145 build tools, run [vs2026.bat](https://github.com/atalantestudio/Trident/tree/main/scripts/Debug/vs2026.bat).

### MSVC (Visual Studio)

Open the solution file (`.sln` or `.slnx` depending on your version of the build tools), then select your desired configuration.

Right-click on the project you want to run and select `Set as Startup Project`.
Type `Ctrl` + `Shift` + `B` to build the project, then type `F5` to start debugging.



## Compiling shaders

<mark>**TODO**</mark>

<!-- If using the Vulkan adapter (or the OpenGL adapter with `GL_SOURCE_FROM_SPIRV`), you will need to compile the GLSL shaders to SPIR-V binaries.

The engine features several `CompileShaders.bat` scripts that compile a batch of GLSL shaders at once. They can be found in the following directories:
- `Trident/src/Asset/Shader/GUI`: subcomponents/glyph subcomponents
- `AssetBuilder/Asset/Shader`: vector glyph rasterization and distance field generation
- `Samples/Asset/Shader/Base`: generic shaders
- `Samples/Asset/Shader/Physics2D`: physics demo
- `Samples/Asset/Shader/Sample`: rendering samples
- `Samples/Asset/Shader/Visibility`: Visibility renderer

Then, run the project. The property file will automatically be created, and you should see a window open with the selected sample.

If not, create the `SampleGroup` property in the .tpr file and try again. -->



## Cleanup

Run [scripts/Clean.bat](https://github.com/atalantestudio/Trident/tree/main/scripts/Clean.bat) to remove the project files and configuration data. This also includes runtime data such as assets and property files.



***



## Visual Studio Code integration

The engine provides Visual Studio Code integration through [tasks](https://code.visualstudio.com/docs/debugtest/tasks) and [launch configurations](https://code.visualstudio.com/docs/debugtest/debugging-configuration).

You may want to install the [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) language support extension.

Make sure that whatever build tool you're using is exposed in your PATH.

Type `F6` to open the task menu. After compiling, go to the Run and Debug panel and select a launch option according to your desired project and configuration, then type `F5` to start debugging.