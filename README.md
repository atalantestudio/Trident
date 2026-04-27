# Trident

Trident is a 2D/3D game engine developed by Atalante Studio, intended for technical teams. It features a set of low-level APIs that allow developers to create high performance games, UIs and applications. It is currently available on Windows, with Linux support already planned.



## Features

### Engine

- Window abstraction with Win32 adapter (X11 and/or Wayland to be added)
- IO (byte/bit streams, readers/writers, encoders/decoders)
- FBX, PNG, QOI and TTF parsing
- Low-level graphics abstraction with OpenGL, Vulkan and DirectX 12 adapters
- SDF text rendering
- Immediate-mode 2D GUI API with automatic layout distribution
- Sparse ECS
- 2D physics API inspired by Box2D-Lite
- Asset builder
- Custom binary file formats for font, geometry, image and scene assets
- Custom shader language (compiler, LSP server and completion features)

### Client

- Many rendering samples for demonstrating basic use-cases
- Many GUI samples
- 2D physics simulation
- 3D Forward renderer
- 3D Visibility renderer



## Dependencies

Most of the dependencies are managed by Atalante Studio. This allows fast iterations and bug fixing on isolated parts of the engine.

- [Premake 5.0.0 beta 8](https://github.com/premake/premake-core/releases/tag/v5.0.0-beta8)
- glslc.exe (taken from Vulkan SDK 1.4.335.0)
- [Lyah 1.2.0+dev](https://github.com/atalantestudio/lyah)
- [Scroll 1.0.5+dev](https://github.com/atalantestudio/scroll)
- [Module base](https://github.com/atalantestudio/ModuleBase)
- [Asset pack for geometries, images and shaders](https://github.com/atalantestudio/SampleAssetPack)



## Installation

See [INSTALL.md](https://github.com/atalantestudio/Trident/tree/main/INSTALL.md).



## Property files

Property files are text files containing information for running a project.
They share the project's name and end with `.tpr`. The application automatically recreates missing property files.

Below is the list of currently used properties and their possible values.

| Property | Values |
| - | - |
| `GraphicsAPI` | "OpenGL", "Vulkan" or "DirectX12" |
| `PreferIntegratedGPU` | 0 or 1 |
| `WindowWidth` | Any unsigned integer |
| `WindowHeight` | Any unsigned integer |
| `VerticalSync` | 0 or 1 |
| `SampleGroupName` | See [Samples/src/main.cpp](https://github.com/atalantestudio/Trident/blob/main/Samples/src/main.cpp) |
| `SampleName` | See [Samples/src/main.cpp](https://github.com/atalantestudio/Trident/blob/main/Samples/src/main.cpp) |

> [!NOTE]
> Some samples may also require the `AssetName` property, for example to load an image or a scene. For these samples, the value of `AssetName` must be the name of the asset file to load, without the extension.



## Resources

See [RESOURCES.md](https://github.com/atalantestudio/Trident/blob/main/RESOURCES.md) for a list of websites, articles, videos, slides, specifications and more that helped develop the engine.