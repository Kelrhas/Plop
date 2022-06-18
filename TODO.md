
**[General]**
- use an engine executable that calls the game dll (faster to compile ? easier to iterate on gameplay code by hot reloading the dll)
- use a code style file
- passer sur CMake


**[Plop]**

- fix memory tracking (the std::map are released before every delete is called so we try to access freed memory, problem of static order initialization)
- use Asset for Texture inheritance
- Serialise a level into an object before opening the file
	- Prevents erasing the whole file when something goes wrong during serialisation 
	- Centralise all the Assets to load before creating the Entities
- improve world/local matrices computation, use caching
- fix resolution of 0 when minimizing
- save the window being maximized
- ~~use entt::handle~~
- update EnTT
	- use registry storage to store multiple component of the same type
- use commands for rendering (probably vulkan switch)
	- add debug render command (cube, line, sphere ...)


**[Editor]**

- adapt the camera focus to encapsulate the object size
	- obviously compute the object size first
- ~~select with mouse -> picking~~
- snapped transformation with CTRL or SHIFT (see blender)
- multi selection
- ~~simple undo/redo~~
	- ~~use commands for editor modification~~
- make sure serialisation produces the same output everytime
	- might mean loading the registry in a different manner, see snapshot & archive


**[TD]**

- implements pathfind in grid
- add a quadtree to keep track of ennemies


