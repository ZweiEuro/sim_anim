## 1. Excecution
Cd a terminal to /bin.
execute via command:,

```bash
ln -fs ../res ./res #link the resource folder
./magic_eight
```

## 2. Manual
Clicking with the mouse onto the table will fire the white ball to the mouse with increasing speed the further the click is away from the white ball.
Esc -> shutdown and exit
P -> Pause physics and collision
R -> reset game objects
S -> show/hide settings

The settings can change a time delta premultiplier to slow down the physics simulation.
A debug view may be enabled to show collision bodies, past collision points and velocity vectors.
Additionaly the view shows uninteractable Catmull-rom splines. These do not have a function yet.


## 3. Tech
Rigid Body collision:
GameObject/RigidBody.cpp implements rigid body collisions resolution.
GameObject/GameObject.cpp implements collision detection.

The physics are simulated by the Gamecore/PhysicsManager.cpp

Path interpolation:
WIP arc length parametrization not done yet.
math/PathInterpol.cpp

## Note
If you want to compile this yourself you will need 
allegro5 and spdlog as libraries in your system.