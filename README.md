### BUILDING

## Prerequisites
- cmake 
- ninja (unless you want to use another build tool and stray from these build instructions)

## Assimp
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
