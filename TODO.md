
*** [Plop] ***

- use Asset for Texture inheritance
- Serialise a level into an object before opening the file
	- Prevents erasing the whole file when something goes wrong during serialisation 
	- Centralise all the Assets to load before creating the Entities

*** [Editor] ***
- adapt the camera focus to encapsulate the object size
	- obviously compute the object size first

*** [TD] ***
- use straight line for movement as long as CatmullRom does not support constant speed


