# maxwdwyer.github.io

GAME: https://maxwdwyer.github.io/SnowGame/
IT MIGHT TAKE A WHILE TO LOAD

This game was not meant to be played in browser, most things work but some things might be slower and the game might crash.

If clicking does not working for digging try holding down lmb instead of just clicking.

This was made in c++ with raylib and then compiled for html5 with emscripten.

This game is also unfinished, I intend to add different ice block shapes and sledding.

"New folder (2)" contains the source files, if you want to build it you need the raylib libray. 
If you want to just have the executable you should go into headers.h and make sure that IS_WEB_BUILD = false



Credit:

Sebastian Lague on youtube for explanation of marching cubes algorithm

dwilliamson on GitHub for the premade tables for marching cubes

Luís Almeida who made the depth rendering example on raylibs site, I used parts of that to help with the reflection in the ice

Brad‑Chamberlain (I think) for making the blender sapling tree gen addon I used for the trees

TheManTheMythTheGameDev who made an example of shadows on raylibs site, I started to add shadows but never finished
