# Building and Running the 3D Model Viewer

This document outlines the steps required to build and run the 3D Model Viewer on your system.

## Prerequisites

Before you begin, ensure that you have the following dependencies installed:

- CMake (minimum version: 3.10)
- OpenGL development libraries
- Assimp library

## Building the Project

1. Clone the repository.

   ```sh
   git clone https://github.com/SusheelThapa/House-Modeling-CG.git
   ```

2. Move to the Project Directory

   ```sh
   cd House-Modeling-CG
   ```

### Setting up Libraries

1. `assimp`

   - Clone the `assimp` from its official source

     ```bash
     git clone https://github.com/assimp/assimp.git
     ```

2. `glfw`

   - Clone the `glfw` from its official source

     ```bash
     git clone https://github.com/glfw/glfw.git
     ```

3. `glm`

   - Clone the `glm` from its official source

     ```bash
     git clone https://github.com/g-truc/glm.git
     ```

4. `imgui`

   - Download the `imgui` folder from [here](https://drive.google.com/drive/folders/1FZn9a0Ud0LNCRrs_HPmCfizxk3lFqF3y?usp=sharing).

5. `glad`

   - Head over this [url](https://glad.dav1d.de/#language=c&specification=gl&api=gl%3D4.0&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&profile=core&extensions=GL_INTEL_blackhole_render&extensions=GL_INTEL_conservative_rasterization&extensions=GL_INTEL_fragment_shader_ordering&extensions=GL_INTEL_framebuffer_CMAA&extensions=GL_INTEL_map_texture&extensions=GL_INTEL_parallel_arrays&extensions=GL_INTEL_performance_query&loader=on)
   - Click over **Generate** button
   - Then download `glad.zip` file
   - The file structure inside `glad.zip` will be

     ```
     ├── include
     │   ├── glad
     │   │   └── glad.h
     │   └── KHR
     │       └── khrplatform.h
     └── src
         └── glad.c

     4 directories, 4 files
     ```

   - Copy `glad` and `KHR` folder inside `projectlearn/include/` directory
   - Copy `glad.c` inside `projectlearn/src/`

6. `stb_image.h`

   - Download `std_image.h` files from [here](https://github.com/nothings/stb/blob/master/stb_image.h).
   - Move downloaded `stb_image.h` into `projectlearn/include/` directory

### Setting up Models

1. Download the `models` folder from [here](https://drive.google.com/drive/folders/1FZn9a0Ud0LNCRrs_HPmCfizxk3lFqF3y?usp=sharing)

2. Move downloaded `models` folder inside `projectlearn/res/`

### Builds Steps

1. Execute `cmake build .` to build the library we have cloned.

   ```terminal
   # For Linux User
   cmake build .

   # For Window User
   cmake -G "Unix Makefiles" build .
   ```

   _Note: When you first time execute this command, it make take some time_

2. Compile the project using `make`

   ```terminal
   make
   ```

## Executing Project

```terminal
./projectlearn/src/MyProject
```

   <i>
   Note:

It may take 3 to 10 minutes to load depending upon your RAM and Graphics Card

You might be ask to **close** the program but chose **wait**.
</i>

## Troubleshooting

If you encounter any issues during the building or running process, please contact us [here](./README.md#contact)
