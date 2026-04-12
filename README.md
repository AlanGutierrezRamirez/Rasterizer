# Rasterizer

A software rasterizer built from scratch in C++ using SDL3 for window and input management.

The goal of this project is to implement the full graphics pipeline manually — without OpenGL, Vulkan, or any rendering API. Every stage from 3D vertex to screen pixel is written by hand.

## What is a rasterizer?

A rasterizer is the process that takes a 3D object defined by vertices and converts it into the pixels you see on screen. This is exactly what OpenGL does under the hood. In this project, that entire pipeline is implemented in software:

```
3D Vertex → World Space → Camera Space → Clip Space → Screen Space → Pixel
```

## Pipeline stages (work in progress)

- [x] Window creation with SDL3
- [x] Triangle drawing
- [ ] Vertex transformation (model, view, projection matrices)
- [ ] Perspective divide
- [ ] Backface culling
- [ ] Scanline rasterization
- [ ] Depth buffer (z-buffer)
- [ ] Texture mapping
- [ ] Lighting

## Built with

- **C++17**
- **SDL3** — window creation and input handling
- **clang++** — compiler
- **Make** — build system

## Project structure

```
Rasterizer/
├── src/
│   └── main.cpp
├── include/
├── libs/
├── dependencies/
│   └── SDL3.xcframework/
└── Makefile
```

## Getting started

### Requirements

- macOS (Apple Silicon or Intel)
- SDL3 framework
- clang++ (included with Xcode Command Line Tools)

### Build and run

```bash
# Remove quarantine from SDL3 (first time only)
xattr -cr ./dependencies/SDL3.xcframework

# Build
make

# Run
./Rasterizer
```

### Clean build

```bash
make clean
```

## Platform

Developed on macOS with Apple Silicon (M4). SDL3 xcframework includes both arm64 and x86_64 slices.
