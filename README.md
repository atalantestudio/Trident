# ProjectA

C++ 3D game engine (rendering, physics and audio) with integrated math and GUI APIs.

## Dependencies

- [Lyah](https://github.com/atalantestudio/lyah)
- [Premake](https://github.com/premake/premake-core)

## Install (Windows)

Clone the repository, then run [scripts/Windows/premake5_vs2022.bat](https://github.com/atalantestudio/ProjectA/tree/main/scripts/Windows/premake5_vs2022.bat). This will clone the Lyah submodule, copy the necessary files from the OpenGL Registry and create a Visual Studio solution file (.sln) for the project.
It also adds the repository to the list of Git safe directories.

Make sure that the Vulkan SDK is installed (the engine will search for the `VULKAN_SDK` environment variable).

At the time of writing, some resources are not present in the repository:
- `ProjectA/src/Asset/Font/**`: Test fonts for the GUI and text renderers.
- `Editor/Editor.tpr`: Configuration file for the Editor project.
- `Editor/Samples.tpr`: Configuration file for the Samples project.

### Visual Studio

Open the solution file, then type `Ctrl + Shift + B` to build.

### Visual Studio Code

Install the [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) extension.

Make sure that `MSBuild.exe` is included in your `PATH` or that you have launched VS Code with the Developer Command Prompt.

To build the project, run the Build Task. This can be done by either typing `Ctrl + Shift + B` or opening the command menu (`Ctrl + Shift + P`), then selecting "Tasks: Run Build Task".  
Select the task corresponding to your configuration.

## Property file

A property file is a text file containing information for running a project.
Property files have the name of their parent project and end with the .tpr extension.

Here is the default structure for a property file:
```txt
GraphicsAPI: 1
WindowWidth: 800
WindowHeight: 600
VSync: true
```

## Resources

### Conservative Advancement

| Title | Author(s) | Date | Links |
| - | - | - | - |
| *Continuous Collision - GDC 2013* | Erin Catto | 2013 | [Slides (54 pages)](https://box2d.org/files/ErinCatto_ContinuousCollision_GDC2013.pdf)<br />[Slides (67 pages)](https://ubm-twvideo01.s3.amazonaws.com/o1/vault/gdc2013/slides/824737Catto_Erin_PhysicsForGame.pdf)<br />[Video](https://www.youtube.com/watch?v=7_nKOET6zwI) |
| *Video Game Physics Tutorial - Part II: Collision Detection for Solid Objects* | Nilson Souto | - | [Article](https://www.toptal.com/game/video-game-physics-part-ii-collision-detection-for-solid-objects) |

### Entity Component System

| Title | Author(s) | Date | Links |
| - | - | - | - |
| *A Simple Entity Component System (ECS) \[C++\]* | Austin Morlan | Jun 25, 2019 | [Article](https://austinmorlan.com/posts/entity_component_system) |

### GJK

| Title | Author(s) | Date | Links |
| - | - | - | - |
| *Real-Time Collision Detection* | Christer Ericson | December 22, 2004 | [Book](https://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf) |
| *GJK (Gilbert–Johnson–Keerthi)* | William Bittle | April 13, 2010 | [Article](https://dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi) |
| *GJK, Distance, Closest Points* | William Bittle | April 26, 2010 | [Article](https://dyn4j.org/2010/04/gjk-distance-closest-points) |
| *Computing Distance using GJK - GDC 2010* | Erin Catto | 2010 | [Slides](https://box2d.org/files/ErinCatto_GJK_GDC2010.pdf) |
| *Implementing GJK (2006)* | Casey Muratori | May 12, 2016 | [Article](https://caseymuratori.com/blog_0003)<br />[Video](https://youtu.be/Qupqu1xe7Io) |
| *GJK: Collision detection algorithm in 2D/3D* | Winterdev | August 29, 2020 | [Article](https://winter.dev/articles/gjk-algorithm)<br />[Video](https://youtu.be/MDusDn8oTSE) |
| *A Strange But Elegant Approach to a Surprisingly Hard Problem (GJK Algorithm)* | Reducible | Mar 24, 2021 | [Video](https://youtu.be/ajv46BSqcK4) |

### OpenGL

| Title | Author(s) | Date | Links |
| - | - | - | - |
| *A Guide to Modern OpenGL Functions* | fendevel | Oct 3, 2016 | [Article](https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions) |
| *Best Practices for Modern OpenGL* | Jake Ryan | Jan, 2022 | [Article](https://juandiegomontoya.github.io/modern_opengl.html) |

### PNG

| Title | Author(s) | Date | Links |
| - | - | - | - |
| *Portable Network Graphics (PNG) Specification (Second Edition)* | W3C | November 10, 2003 | [Website](https://www.w3.org/TR/2003/REC-PNG-20031110) |
| *Implementing a Basic PNG reader the handmade way* | S | May 13, 2017 | [Website](https://handmade.network/forums/articles/t/2363-implementing_a_basic_png_reader_the_handmade_way) |
| *Each step of decoding a PNG* | loops | Mar 11, 2022 | [Article](https://iter.ca/post/png) |
| *PNG file chunk inspector* | Nayuki | April 12, 2023 | [Website](https://www.nayuki.io/page/png-file-chunk-inspector) |

### Signed Distance Field font rendering

| Title | Author(s) | Date | Links |
| - | - | - | - |
| *Improved Alpha-Tested Magnification for Vector Textures and Special Effects* | Chris Green | 2007 | [Paper](https://cdn.fastly.steamstatic.com/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf) |
| *Distance field fonts* | libGDX | - | [Article](https://libgdx.com/wiki/graphics/2d/fonts/distance-field-fonts) |

### TTF

| Title | Author(s) | Date | Links |
| - | - | - | - |
| *TrueType Reference Manual* | Apple | - | [Website](https://developer.apple.com/fonts/TrueType-Reference-Manual) |
| *True Type Fonts* | Gabor Makes Games | - | [Article](https://gabormakesgames.com/blog_fonts.html) |
| *FontDrop!* | Viktor Nübel<br />Clemens Nübel | - | [Website](https://fontdrop.info) |

<mark><b>TODO</b></mark>