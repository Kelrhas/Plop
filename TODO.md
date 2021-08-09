
*** [Plop] ***

- fix memory tracking (the std::map are released before every delete is called so we try to access freed memory)
- use Asset for Texture inheritance
- Serialise a level into an object before opening the file
	- Prevents erasing the whole file when something goes wrong during serialisation 
	- Centralise all the Assets to load before creating the Entities
- improve world/local matrices computation, use caching

*** [Editor] ***
- adapt the camera focus to encapsulate the object size
	- obviously compute the object size first
- select with mouse -> picking
- snapped transformation with CTRL or SHIFT (see blender)

*** [TD] ***
- implements pathfind in grid


