# ProjectA

C++ 3D game engine (rendering, physics and audio) with integrated math and GUI APIs.

## Dependencies

- [Lyah](https://github.com/atalantestudio/lyah)
- [SampleAssetPack](https://github.com/atalantestudio/SampleAssetPack)
- [Premake](https://github.com/premake/premake-core)

## Install (Windows)

Clone the repository, then run [scripts/Windows/premake5_vs2022.bat](https://github.com/atalantestudio/ProjectA/tree/main/scripts/Windows/premake5_vs2022.bat). This will clone the Lyah submodule, copy the necessary files from the OpenGL Registry and create a Visual Studio solution file (.sln) for the project.
It also adds the repository to the list of Git safe directories.

Make sure that the Vulkan SDK is installed (the engine will search for the `VULKAN_SDK` environment variable).

At the time of writing, some resources are not present in the repository:
- `ProjectA/src/Asset/Font/**`: Test fonts for the GUI and text renderers.
- `AssetBuilder/AssetBuilder.tpr`: Configuration file for the AssetBuilder project.
- `Samples/Samples.tpr`: Configuration file for the Samples project.

### Visual Studio

Open the solution file, then type `Ctrl + Shift + B` to build.

### Visual Studio Code

Install the [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) extension.

Make sure that `MSBuild.exe` is included in your `PATH` or that you have launched VS Code with the Developer Command Prompt.

To build the project, run the Build Task. This can be done by either typing `Ctrl + Shift + B` or opening the command menu (`Ctrl + Shift + P`), then selecting "Tasks: Run Build Task".  
Select the task corresponding to your configuration.

## Property file

A property file is a text file containing information for running a project.
Property files share the name of their parent project and end with the .tpr extension.

Here is the default structure for a property file:
```txt
GraphicsAPI: 1
WindowWidth: 800
WindowHeight: 600
VSync: true
```

## Resources

See [RESOURCES.md](https://github.com/atalantestudio/ProjectA/blob/main/RESOURCES.md) for a list of websites, articles, videos, slides, books, specifications and papers that helped me develop this project.