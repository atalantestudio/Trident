# Trident

Trident is a 2D/3D game engine developed by Atalante Studio, intended for technical teams. It features a set of low-level APIs that allow developers to create high performance games, UIs and applications. It is currently available on Windows, with Linux support already planned.



## Features

### Engine

- Window abstraction with Win32 adapter (X11 and/or Wayland to be added)
- IO (byte/bit streams, readers/writers, encoders/decoders)
- FBX, PNG, QOI and TTF parsing
- Low-level graphics abstraction with OpenGL, Vulkan and DirectX 12 adapters
- Immediate-mode 2D GUI API with automatic layout distribution
- Sparse ECS
- 2D physics engine with support for convex hulls and spheres
- 3D physics engine with support for convex hulls
- Asset builder
- Lightmapper
- SDF glyph map generator
- Custom binary file formats for font, geometry, image and scene assets
- Custom shader language (compiler, LSP server and completion features)

### Client

- Many rendering samples for demonstrating basic use-cases
- Single pass mip level generataor
- Many GUI samples
- 2D physics simulation
- 3D Forward renderer
- 3D Visibility renderer



## Dependencies

Apart from the C++ standard, Trident only uses libraries managed by Atalante. This is mainly for fast iterations and bug fixing on isolated parts of the engine.
On Windows, Premake and `glslc` are embedded as executables and used during the build steps.

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

| Property | Type |
| - | - |
| `GraphicsAPI` | String ("DirectX12", "OpenGL" or "Vulkan") |
| `PreferIntegratedGPU` | Boolean |
| `Window.Width` | Unsigned integer |
| `Window.Height` | Unsigned integer |
| `Window.Maximized` | Boolean |
| `VerticalSync` | Boolean |

> [!NOTE]
> Some samples define their own properties. Make sure these are included in the property file before launching the application.



## Resources

See [RESOURCES.md](https://github.com/atalantestudio/Trident/blob/main/RESOURCES.md) for a list of websites, articles, videos, slides, specifications and more that helped develop the engine.