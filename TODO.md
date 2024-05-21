
**[General]**
- use an engine executable that calls the game dll (faster to compile ? easier to iterate on gameplay code by hot reloading the dll)
- ~~use C++20~~
- update imgui (tables)
- ~~use a code style file~~


**[Core]**

- ~~change window title depending on level & configuration~~
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
- use commands for rendering (probably vulkan switch)
	- add debug render command (cube, line, sphere ...)
- spritesheet creator/modification
- prefab system
	- ~~simple instanciation~~
	- ~~add PrefabInstance (store GUID) component to track from what prefab it comes from~~
	- ~~only serialise difference between prefab and instance~~
	- update prefab from instance
	- prefab edition ?
	- prevent adding children to a prefab instance
	- prevent referencing prefab children in any component
	- focus the entity that we just instantiated
	- add PrefabInstance to the source entity when creating a prefab
- VFX
	- GPU side for long term
	- add more shape like cone and mesh (maybe also hexagon to match TD grid)
	- auto augment pool size up to a fixed point
	- remove spawners/updaters from possible list if already created
	- auto kill for burst effect
	- take entity rotation into account for spawning particles
- add a virtual file system to load from different mount points (to load from the Editor/Engine/Game folder)
- fix picking (enttID that change version have a big int value, that may be rounded when converting to float)
- fix closing app
- change CanAddComponent/CanRemoveComponent from template function to type value inside component struct
- clean up RegisterComponents (calling sometimes Application::, sometimes ComponentManager -the right one-)
- use more of static constexpr and using, especially inside Components
- clean usage of entt::handle, Entity and entt::entity
- use more StringView instead of String when we just want to output/see the content


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
- ~~fix render while playing is paused~~
	- fix picking while paused
- fix EditorCamera rotation when a Camera entity is selected and possibly when level is playing
- fix zoom