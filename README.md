# A Software Renderer written in plain C

A CPU/Software implementation of fixed GPU pipeline that supports **Physically AccurateRendering[PBR] Rendering** and also uses multithreading using **pthreads** in order to increase the renderer's performance. Its written in plain **C** with as minimal overhead as possible.
The only two external library used are **SDL2** for **Windowing/Inputs** and **stb_image.h** for loading images with a well commented codebase.

# Features
###
* Complete custom math library from scratch that aids in the vector and matrix maths
* Textured 3D Mesh supported using OBJ file format only
* Blinn-Phong and Physically Based Rendering Lighting supported
* A Parallel Triangle Rasterization technique based on (https://fgiesen.wordpress.com/2013/02/10/optimizing-the-basic-rasterizer/) , that supports multithreading with pthreads that helps increases performance on a multi-core system.
* Supports all the basic steps involved in a Graphics pipeline
    * Model->View->Projection pipeline
    * FPS camera system
    * Texture
    * Camera Frustum Clipping
    * Persective Correct Interpolation
    * Depth Buffer
    * Cubemaps/Skyboxes
    * Per Pixel lighting

# PBR Lighting
_Based on the PBR Chapters of the book **Real Time Rendering**_
![PBR Lighting](/assets/pbr_demo.gif)
# Blinn Phong Lighting
![Blinn Phong Lighting](assets/blinn_phong_demo.gif)

# Build and Run the Project
```
# Clone the repo
git clone 
cd 3D_Software_Renderer

# Create the build directory
mkdir build && cd build

# Configure and build the file
cmake ..
cmake --build .

# this will create an executable called "renderer"
./renderer

```
