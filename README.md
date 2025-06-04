# ProjectA

C++ 3D game engine (rendering, physics and audio) with integrated math and GUI APIs.

## Dependencies

- [Lyah](https://github.com/debaze/lyah)
- [Premake](https://github.com/premake/premake-core)

## Install (Windows)

Clone the repository, then run `scripts/Windows/premake5.bat`. This will clone the submodules, copy the necessary files from the OpenGL Registry and build the project.
It also adds the repository to the list of Git safe directories.

Make sure that the Vulkan SDK is installed (the engine will search for the `VULKAN_SDK` environment variable).

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