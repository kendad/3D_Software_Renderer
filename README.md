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
    *Model->View->Projection pipeline
    *An FPS camera system
    *Texture are supported
    *Camera Frustum Clipping
    *Persective Correct Interpolation
    *Depth Buffer
    *Per Pixel lighting

# PBR Lighting
![PBR Lighting](/assets/pbr_demo.gif)

