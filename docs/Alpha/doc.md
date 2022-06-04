
# Magic Eight


Dominik Völkel    11811035  d.voelkel@student.tugraz.at

Phillip Stranger  11807773  phillip.stranger@student.tugraz.at


Notes from ass2:

Angular momentum visualization: we have no angular momentum.

Seperated Update rate and time delta multiplier.

Implemented FPS updater (do not switch this around to much the internal timer might not like it).

Notes for final release:

Force Field checkbox added.

Object path is shown in debug mode.

Toggle box for euler / RK4

Step size h slider for euler/RK4



## 0. Needed libraries:
Firstly on ubuntu community repository (universe) need to be enabled. (Tutorial: https://www.linuxshelltips.com/enable-universe-repository-ubuntu/)

Allegro needs to be installed:
https://github.com/liballeg/allegro_wiki/wiki/Quickstart

under Linux -> Ubuntu

```
sudo apt update

sudo add-apt-repository ppa:allegro/5.2
sudo apt-get install liballegro*5.2 liballegro*5-dev

#and then we install spdlog
sudo apt install libspdlog-dev

```




## 1. Excecution

Next to this file in this filder is a shell script "start.sh" that links ./res for the game and starts it.
With a terminal inside the extracted folder you can execute the game with:
```
./start.sh
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