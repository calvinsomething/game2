# game2

My second exploration into 3D game engine programming with C++.

## Overview

Renders a scene with multiple animated models, a controllable character and camera, a skybox, and shadow mapping.

## Features
- Win32 API
- DirectX 11 rendering pipeline
- Skeletal animation (FBX model support via Assimp)
- Shadow map pass
- HDR skybox
- Instanced rendering
- Structured buffers for materials and bone transforms
- Collision system (floor triangle detection)
- 60 FPS cap
- Full-screen window


## BUILDING

## Requirements
- Windows
- DirectX 11
- A compiler supporting C++17 or later (MSVC recommended)
- cmake 
- ninja (unless you want to use another build tool and stray from these build instructions)

### Assimp
Create a directory for Assimp anywhere you'd like, then change to that directory and run:
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
