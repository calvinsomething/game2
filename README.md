# game2
My second exploration into 3D game engine programming with C++.

## Overview
Renders a scene with multiple animated models, a controllable character and camera, a skybox, and shadow mapping.

## Toolchain
- Win32 API
- DirectX 11 rendering pipeline
- Custom _STL-compatible_ block memory allocator: [calvinsomething/memory_allocator](https://github.com/calvinsomething/memory_allocator);
- [assimp/assimp](https://github.com/assimp/assimp) asset loading library
- [nothings/stb](https://github.com/nothings/stb) image loader (for panoramic skybox .hdr image)

## Features
- 3D models loaded from .fbx, .obj, .mtl, and .dds files
- Skeletal animation
- Shadow map pass
- Skybox (equirectangle-to-cubemap projection via compute shader)
- Instanced rendering
- Structured buffers for materials and bone transforms
- Collision system (floor triangle detection)
- Steady frame rate and synced movement speed
- Error and debug message system using IDXGIDebug and ID3D11Debug

## Retrospective
### What Worked
- My custom allocator is fast and has no noticeable bugs at this point.
- The error system works well, is easy to use and provides valuable feedback in a convenient message box.
- The controls and the input->controller->entity pipeline is clean and easy to understand. I think it's a good foundation for a more sophisticated controls system.
- The collision system is minimal, but is still over-engineered for this scene. It efficiently stores terrain triangles in a sparse grid, depending on their slope, for cheap floor collision queries. Adding dynamic, 3D collision, would be a significant addition, but I think the current design is a strong starting point.
- The designs of Model, Mesh, Material, Animation, and Bone classes have some redeeming aspects. Referencing the vertices, indices, textures and bone transforms, rather than storing them internally, was a successful data oriented design choice.

### What I Would Improve
- I would definitely add a rendering layer and attempt a render graph. The main loop should only be calling the most high level functions.
- The Model class should not be templated on the vertex type, and I would probably implement something more along the lines of a component system to group varying Mesh types under the same Model.
- I would define systems and manager classes to support further data oriented design, encapsulating the details of data loading and access.
- Assimp data should be copied entirely, freeing the `aiScene` afterwards.
- By implementing the shadow pass, I learned how an SoA design can he useful, and I would separate the vertex properties into their own buffer slots. This way the positions and bone data could be front-loaded and many vertex layouts could share the same shadow map vertex shader.
- Structured buffers would be cleaner than instancing for the purposes of this scene. I just wanted to implement instancing.
- The light source's projective matrix should be dynamic, and fitted around the camera's frustum. For this small scene I get away with a static transform, but for a larger area I would need a cascaded shadow map.
- Writing shader permutations would reduce the amount of HLSL, and make it easier to create and maintain shaders.

## BUILDING
### Requirements
- Windows SDK
- MSVC (version >= 19.15) and dxc
- cmake 
- ninja (unless you want to use another build tool and stray from these build instructions)

If compiling shaders outside of Visual Studio, from your terminal you should execute the file `vcvars64.bat` located in a directory similar to this (depending on the version of Visual Studio): _C:\Program Files\Microsoft Visual Studio\\<year>\Community\VC\Auxiliary\Build_. This will configure several environment variables Windows requires to run _dxc_ from the command line.

### Assimp
```
$ git clone https://github.com/assimp/assimp.git --branch v6.0.4
$ mkdir assimp\build && cd assimp\build
$ cmake -GNinja ..
$ ninja
$ xcopy lib <project-root>\vendor\lib\assimp /i
$ xcopy bin <project-root>\vendor\bin\assimp /i
$ xcopy ..\include\assimp <project-root>\vendor\include\assimp /s /i
$ xcopy include\assimp <project-root>\vendor\include\assimp /y
```
