# C Game Framework

This is a **C99 Windows** game framework with **Graphics, Sound, Mouse And Keyboard Input, Time, Random Generator** and a **Physics Engine**. For example, a game like [Hill Climb Racing](https://play.google.com/store/apps/details?id=com.fingersoft.hillclimb&hl=en) can be written in it.

An example main.c along with a gcc build command are included as a starting point. A game written using most of the engine features: [Fast Crash](https://github.com/thealing/FastCrash).

# How To Use
- Call window_create() before using the graphics module or loading images
- Before each game update, call window_update()
- After each game render (using graphics_* functions), call window_display()
# Features
## Graphics
- Load PNG, JPG, BMP...
- Set camera (viewport)
- Set drawing color
- Draw image textures
  - Set size, center and scale
  - Draw at a given position and angle
  - Draw in a given rectangle or quad
  - Draw only a part of a texture (sprite sheet)
- Draw shapes
  - Segment
  - Circle
  - Polygon
  - Rectangle
- Draw text
  - Load any installed font
      <ol type="1">
        <li>Create a font sheet using <a href="https://github.com/thealing/FontCreator">Font Creator</a></li>
        <li>Load a font sheet as a texture</li>
        <li>Set the font texture as the font</li>
      </ol>
  - Set horizontal and vertical alignment
  - Draw in a rectangle
  - Draw at the current transform (height is 1 without scaling)
- Transform operations
  - Translate
  - Rotate
  - Scale
  - Save and restore
## Sound
- Load MP3, MP2, WAV...
- Play, pause, resume, stop
- Adjust volume (0: muted 1: normal)
- Go to position (in seconds)
## Input
- Get mouse position (relative to the current camera and transform!)
- Query if mouse button or keys
  - are currently down
  - have just been pressed
  - have just been released
## Time
- Get time (microsecond precise)
- Sleep (millisecond precise)
## Random Generator
- Generate cryptographically random
  - integer in range
  - double in range

# Physics Engine Features
## World
  - Gravity
  - Collision callbacks
## Colliders
  - Convex shapes
    - Segment
    - Circle
    - Polygon
  - Density
  - Restitution
  - Static and dynamic friction
  - Collision filter
  - Can be a sensor (no collision handling, only callback)
## Bodies
  - Types
    - Dynamic: moved by user and physics
    - Kinematic: moved by user (collisions don't change it's velocity)
    - Static: cannot be moved (velocity and force is zero)
  - Position
  - Velocity
  - Forces can be applied
  - Impulses can be applied
  - Mass automatically calculated from colliders
## Joints
  - Types
    - Fixed: fixed distance and angle
    - Pin: fixed distance
  - Connects two bodies at specified anchor points  
## Solver
  - Broad phase
    - Sweep and prune algorithm
    - Bounding rectangle test
  - Narrow phase
    - Distance test
    - Separating axis test
  - Velocity based simulation
  - Collision resolution
  - Friction resolution
    - Coulomb's law
  - Stability achieved by the usage of correction velocities

# About
- No 3rd party libraries
- Window management and input using Win32 API
- Rendering is using OpenGL
- Sound playback using MCI
- Random generator using CryptoAPI
- Platform specific headers are encapsulated (for fast compilation)
