# GameFramework

This is a high-performance, cross-platform 2D game framework written in C99, featuring a custom physics engine and a modular architecture.

## Overview
The framework is designed with a strict separation between core engine logic and platform-specific backends. This allows for seamless cross-platform development while maintaining efficiency and a minimal footprint without any 3rd party library dependencies.

### Project Structure
- **`source/engine/`**: Platform-independent core modules for physics, graphics, geometry, input, and memory management.
- **`platform/`**: Native implementations for Windows (Win32/DirectShow/OpenGL) and Android (NativeActivity/OpenSL ES/OpenGL ES).
- **`source/game/`**: High-level application code utilizing the engine's API.

## Core Features

### 2D Physics Engine
A custom rigid-body physics simulation built from scratch:
- **Solver**: Dual-stage impulse-based velocity solver and position-based correction. Features **Warm Starting** for improved stability across frames.
- **Broad Phase**: Efficient **Sweep and Prune** algorithm and **AABB Test**.
- **Narrow Phase**: Robust collision detection using the **Separating Axis Theorem (SAT)** for polygons and specialized routines for circles and segments.
- **Body Types**: Supports **Dynamic** (simulated), **Kinematic** (user controlled), and **Static** (unmovable) bodies.
- **Joints**: **Fixed** and **Pin** joints with stability correction.
- **Automatic Mass**: Physical properties (centroid, linear mass, inertia) are automatically calculated based on collider geometry and density.
- **Filtering**: Mask and group-based collision filtering with sensor support (callbacks without physical resolution).

### Graphics & Rendering
A specialized 2D rendering API supporting hardware acceleration:
- **Windows Backend**: Desktop OpenGL using fixed-function pipeline for maximum compatibility.
- **Android Backend**: OpenGL ES 1.1 utilizing vertex arrays.
- **Primitives**: High-level support for segments, circles, polygons, and rectangles with fill/outline options.
- **Transform Stack**: Full support for hierarchical transformations (Translate, Rotate, Scale) using internal matrix management.
- **State Stack**: Push/pop graphics states including colors, textures, and line properties.
- **Typography**: Sprite-font system with alignment and formatted string support.

### Audio System
Multi-platform audio engine for low-latency playback:
- **Windows**: Implemented via **DirectShow** (Graph Builder) supporting modern formats.
- **Android**: Native integration with **OpenSL ES** for optimized mobile performance.
- **Features**: Volume control, pausing, resuming, and seeking functionality.

### Utilities & Memory
- **Memory Pools**: High-performance, object-pool based free-list allocators for engine-critical objects (Bodies, Colliders, Joints).
- **Data Structures**: Implementations of linked lists, maps, and geometric primitives (Vectors, Transforms, Shapes).
- **Security**: Platform-native cryptographically secure random number generation.

## Platform Implementations

### Windows
- **Display**: Native Win32 window management.
- **Image Loading**: Utilizes **Windows Imaging Component (WIC)** for PNG, JPG, and BMP support.
- **Audio**: DirectShow Filter Graph.
- **Time**: High-resolution performance counters.

### Android
- **Lifecycle**: Managed via `NativeActivity` and the Android NDK.
- **Assets**: Integrated with `AAssetManager` for direct access to package resources.
- **Audio**: OpenSL ES with Android File Descriptor support.

## Getting Started
The framework follows a simple lifecycle:
1. Initialize the system with `window_create()`.
2. In the main loop, call `window_update()` to handle input and events.
3. Perform game logic and step the physics world with `physics_world_step()`.
4. Render the scene using `graphics_*` functions and swap buffers with `window_display()`.

## Technical Philosophy
- **Zero Dependencies**: No 3rd party libraries are used, only platform-native APIs and standard C99.
- **Encapsulation**: Platform-specific headers (like `windows.h` or `SLES/OpenSLES.h`) are hidden within the platform layer to ensure fast compilation of game code.
