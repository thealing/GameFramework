# Game Framework

A high-performance, cross-platform 2D game framework written in C99, featuring a custom physics engine and a modular architecture.

## Getting Started

To initialize a game, follow the established configuration and event-driven sequence:

1. **Configure Application**: Before creating the window, set key application metadata using the `config` system.
    ```c
    config_set_value(CONFIG_KEY_WINDOW_TITLE, "My Game");
    config_set_value(CONFIG_KEY_WINDOW_CLASS, "MyGameClass");
    config_set_value(CONFIG_KEY_FOLDER_NAME, "MyGameAssets");
    ```

2. **Initialize Window**: Call `window_create(width, height)` to start the platform-specific initialization.

3. **Main Event Loop**: Process system events and manage the graphics context.
    ```c
    while (window_is_open()) {
        Window_Event event;
        while (window_poll_event(&event)) {
            if (event.type == WINDOW_EVENT_WINDOW_CREATED) {
                // Initialize graphics context and load initial assets
                graphics_init(event.state_event.window);
                texture_create_from_file(&my_texture, "asset.png");
            }
            // Handle input events...
        }

        // Logic Update
        input_update();
        physics_world_step(world, delta_time, true, true);

        // Rendering
        graphics_clear(&clear_color);
        // draw calls...
        graphics_display();
    }
    ```

### Android Deployment
Before deploying to Android, update the project files in `platform/android/`:
- Change the `package` attribute in `AndroidManifest.xml`.
- Change the `app_name` value in `res/values/strings.xml`.
- Replace the provided `game.keystore` with your own keystore.
- Enter the signing credentials in `build.xml` (key.* properties).

## Overview
The framework is designed with a strict separation between core engine logic and platform-specific backends. This allows for seamless cross-platform development while maintaining efficiency and a minimal footprint without any 3rd party library dependencies.

### Project Structure
- **`source/game/`**: High-level application code utilizing the engine's API.
- **`source/engine/`**: Platform-independent core modules for physics, graphics, geometry, input, and memory management.
- **`platform/`**: Native implementations for Windows and Android.

## Core Features

### 2D Physics Engine
A custom rigid-body physics simulator built from scratch:
- **Solver**: Dual-stage impulse-based velocity solver and position-based correction. Features **Warm Starting** for improved stability across frames.
- **Broad Phase**: Efficient **Sweep and Prune** algorithm and **AABB Test**.
- **Narrow Phase**: Robust collision detection using the **Separating Axis Theorem (SAT)** for polygons and specialized routines for circles and segments.
- **Body Types**: Supports **Dynamic** (simulated), **Kinematic** (user controlled), and **Static** (unmovable) bodies.
- **Joints**: **Fixed** and **Pin** joints with stability correction.
- **Automatic Mass**: Physical properties (centroid, linear mass, inertia) are automatically calculated based on collider geometry and density.
- **Filtering**: Mask and group-based collision filtering with sensor support (callbacks without physical resolution).

### Graphics & Rendering
A specialized 2D rendering API supporting hardware acceleration:
- **Windows Backend**: Desktop **OpenGL** using fixed-function pipeline for maximum compatibility.
- **Android Backend**: **OpenGL ES 1.1** utilizing vertex arrays.
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

## Systems Architecture

### Window System
The framework employs a unified event-driven windowing system that abstracts platform-specific lifecycles:
- **Event Loop**: Use `window_poll_event` to handle a variety of events including touch/mouse input, key presses, and window state changes (Resumed, Paused, Created, Destroyed).
- **Unified Input**: Mouse clicks on Windows are mapped to `WINDOW_EVENT_TOUCH_*` events, providing a consistent interface for both desktop and mobile platforms.
- **Platform Backends**: 
    - **Windows**: Managed via a standard Win32 `WNDCLASS` and `window_proc` callback.
    - **Android**: Integrated with `NativeActivity`, handling the asynchronous nature of mobile app lifecycles (e.g., surface creation/destruction).

### Asset System
Assets are managed through a platform-agnostic interface with specialized loading strategies:
- **Resource Extraction (Windows)**: On Windows, the framework automatically extracts embedded resources into a local folder (configured via `CONFIG_KEY_FOLDER_NAME`) to ensure assets are accessible via standard file I/O.
- **Native Loading (Android)**: Leverages `AAssetManager` to read assets directly from the APK, supporting efficient streaming of audio and image data.
- **Unified Image Loading**: Provides `texture_create_from_file` which uses platform-specific decoders to populate hardware textures from any image format.

## Technical Philosophy
- **Zero Dependencies**: No 3rd party libraries are used, only platform-native APIs and standard C99.
- **Encapsulation**: Platform-specific headers (like `windows.h` or `SLES/OpenSLES.h`) are hidden within the platform layer to ensure fast compilation of game code.
