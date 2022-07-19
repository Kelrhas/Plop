
**[General]**
- use an engine executable that calls the game dll (faster to compile ? easier to iterate on gameplay code by hot reloading the dll)
- use a code style file
- passer sur CMake


**[Plop]**

- change window title depending on level & configuration
- fix memory tracking (the std::map are released before every delete is called so we try to access freed memory, problem of static order initialization)
- Assets
	- use Asset class for Texture inheritance
	- change the path stored in definition files to be relative to said definition file
- Serialise a level into an object before opening the file
	- Prevents erasing the whole file when something goes wrong during serialisation 
	- Centralise all the Assets to load before creating the Entities
- improve world/local matrices computation, use caching ? or a WorldMatrixComponent
- fix resolution of 0 when minimizing
- save the window being maximized
- ~~use entt::handle~~
- update EnTT
	- use registry storage to store multiple component of the same type
- use commands for rendering (probably vulkan switch)
	- add debug render command (cube, line, sphere ...)
- spritesheet creator/modification
- prefab system
	- ~~simple instanciation~~
	- add PrefabInstance (store GUID) component to track from what prefab it comes from
	- only serialise difference between prefab and instance
	- update prefab from instance
- add a virtual file system to load from different mount points (to load from the Editor/Engine/Game folder)
- fix picking (enttID that change version have a big int value, that may be rounded when converting to float)
- fix closing app


**[Editor]**

- add Panel class to ease the use of mouse pos computation (mostly for viewport panel)
- adapt the camera focus to encapsulate the object size
	- obviously compute the object size first
- ~~select with mouse -> picking~~
- snapped transformation with CTRL or SHIFT (see blender)
- multi selection
- simple undo/redo
	- ~~use commands for editor modification~~
	- ~~entity transform~~
	- entity creation
	- entity deletion
	- component added
	- component removed
- make sure serialisation produces the same output everytime
	- might mean loading the registry in a different manner, see snapshot & archive
- fix render while playing is paused
- fix EditorCamera rotation when a Camera entity is selected and possibly when level is playing
- fix zoom


**[TD]**

- using a hexgrid
	- ~~pathfind~~
	- ~~make a component to be serialised -> rebuild all the grid from data, and not entities~~
	- make sure Private::bPickingCell is reset when cliking outside the viewport
- quadtree but for hex ?
- clean LevelGrid

