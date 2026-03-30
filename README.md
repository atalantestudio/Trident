# Trident

Trident is a game engine made from the ground up, intended for technical teams. It features a set of low-level APIs that allow developers to create high performance games, UIs and applications.

The engine only supports Windows, but making it run on Linux shouldn't be too hard to add due to the cross-platform nature of the dependencies.

Trident can only be built on C++17 or later, with the v145 platform toolset for MSBuild. However, it can be opened and compiled on both Visual Studio and Visual Studio Code.



## Features

### Engine

- Low-level graphics abstraction with OpenGL, Vulkan and DirectX 12 adapters
- Window abstraction with Win32 adapter
- Sparse Entity-Component System
- Process support (non-graphics executable script)
- Asset pre-building process with single SDF font generator
- Flexible 2D GUI API with automatic layout distribution
- 2D/3D physics API with implementations for GJK, EPA, SAT and other algorithms (<mark>3D is not fully supported yet</mark>)
- Filesystem abstraction (data streams, readers, writers and file decoders/encoders)
- Custom file formats for font, geometry and image assets
- Custom shader language with compiler, LSP server and completion features

### External

- Many rendering samples for demonstrating basic use-cases
- Several GUI samples
- 2D physics simulation
- 3D forward renderer
- 3D Visibility renderer



## Dependencies

Apart from Premake, the dependencies are managed by Atalante Studio. This allows fast iterations and bug fixing on isolated parts of the engine.

- [Premake 5.0.0 (beta 8)](https://github.com/premake/premake-core/releases/tag/v5.0.0-beta8)
- [Lyah 1.2.0](https://github.com/atalantestudio/lyah)
- [Scroll 1.0.5](https://github.com/atalantestudio/scroll)
- [Module base](https://github.com/atalantestudio/ModuleBase)
- [Asset pack for geometries, images and shaders](https://github.com/atalantestudio/SampleAssetPack)



## Install (Windows)

> [!IMPORTANT]
> 
> Make sure that the Vulkan SDK is installed.
> The engine will search for the `VULKAN_SDK` environment variable.

Clone the repository, preferably with the `--recurse-submodules` option:

```sh
git clone git@github.com:atalantestudio/Trident --recurse-submodules
```

Or use the command below to manually fetch the dependencies:

```sh
git submodule update --init --recursive
```

Run [scripts/Windows/premake5_vs2026.bat](https://github.com/atalantestudio/Trident/tree/main/scripts/Windows/premake5_vs2026.bat) to create a Visual Studio 2026 solution file (.slnx).

If using the Vulkan adapter (or the OpenGL adapter with `GL_SOURCE_FROM_SPIRV`), you will need to compile the GLSL shaders to SPIR-V binaries.

The engine features several `CompileShaders.bat` scripts that compile a batch of GLSL shaders at once. They can be found in the following directories:
- `Trident/src/Asset/Shader/GUI`: subcomponents/glyph subcomponents
- `AssetBuilder/Asset/Shader`: vector glyph rasterization and distance field generation
- `Samples/Asset/Shader/Base`: generic shaders
- `Samples/Asset/Shader/Physics2D`: physics demo
- `Samples/Asset/Shader/Sample`: rendering samples
- `Samples/Asset/Shader/Visibility`: Visibility renderer

Then, run the project. The property file will automatically be created, and you should see a window open with the selected sample.

If not, create the `SampleGroup` property in the .tpr file and try again.



## Usage

### Visual Studio

Open the solution file, then select a configuration.

Right-click on the project you want to run and select `Set as Startup Project`.

Type `Ctrl` + `Shift` + `B` to build.

Then, type `F5` to start debugging.

### Visual Studio Code

> [!IMPORTANT]
> 
> Make sure that `MSBuild.exe` is included in your `PATH` or that you have launched VS Code with the Developer Command Prompt.
> For Visual Studio 2026, it should be located in `C:/Program Files/Microsoft Visual Studio/18/Community/MSBuild/Current/Bin/MSBuild.exe`.

Install the [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) language support extension.

Open the Task Menu using `Ctrl` + `F6`. Choose `Clean`, then `Generate Windows Project Files (Visual Studio 2026)`, then a `Build (*)` task according to your desired configuration.

In the Run and Debug panel, select a `Run * (*)` launch option according to your desired project and configuration.

Then, type `F5` to start debugging.



## Property files

A property file is a text file containing information for running a project.
Property files share the name of their parent project and end with .tpr.

Missing property files will be automatically recreated by the application at runtime.

Below is the list of supported properties and their acceptable values.

| Property | Values |
| - | - |
| `GraphicsAPI` | "OpenGL", "Vulkan" or "DirectX12" |
| `PreferIntegratedGPU` | 0 or 1 |
| `WindowWidth` | Any unsigned integer |
| `WindowHeight` | Any unsigned integer |
| `VerticalSync` | 0 or 1 |
| `SampleGroupName` | See [Samples/src/main.cpp](https://github.com/atalantestudio/Trident/blob/main/Samples/src/main.cpp) |
| `SampleName` | See [Samples/src/main.cpp](https://github.com/atalantestudio/Trident/blob/main/Samples/src/main.cpp) |

Some samples may require additional properties, e.g. the name of an asset.



## Resources

See [RESOURCES.md](https://github.com/atalantestudio/Trident/blob/main/RESOURCES.md) for a list of websites, articles, videos, slides, specifications and more that helped develop the engine.