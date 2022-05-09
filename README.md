# uAssetRipper
Convert asset files created by uTinyRipper (or other) into .obj models

Use the executable or build the cpp version, as it supports a lot more formats than the Python version atm.

## Usage
If dump directory is not provided, I think `./ripped` is default

Parse a single file:
- `./UnityAssetRipper.exe path/to/my/file.asset`

Parse a single file and dump to a specified directory:
- `./UnityAssetRipper.exe path/to/my/file.asset my_ripped_file`

Parse all files in a directory:
- `./UnityAssetRipper.exe path/to/my/files`

Parse all files in a directory and dump to a specified directory:
- `./UnityAssetRipper.exe path/to/my/files my_ripped_files

## Disclaimer
Do whatever you want with the source code, but credit me or something idk

## Samples
From Lurking Game
![alt text](lurking-splash.PNG?raw=true "Title")

Valheim
![alt text](valheim-splash.PNG?raw=true "Title")

# Blah
The executable binary fixes most of the issues regarding. The reason for this is because I hate Python and decided to rewrite this project in c++. I looked through an .asset file in Unity and saw some patterns between vertex formats and stuff, which makes the cpp version better.

Idk why the executable is slower than the python version (~30%), maybe because this one does more checking...

> with 3524 assets:
> 	509.12s to run c++ ripper (debug)
> 	 84.00s to run c++ ripper (release)
> 	 54.90s to run python ripper

Subnautica Ghost Leviathan has a color component in the vertex buffer when viewed in the Unity editor, which is interesting. It's also interesting how the 'Ghost' leviathan is actually the emperor model.

I used this tool with Valheim after discovering that the vertex buffer formats were not going to let my Python version work correctly. It's also annoying how about half of the Valheim assets are default named. 

Submeshes are not *yet* supported, and texture uvs are still not written to the obj file for simplicity. 

I suppose it is easy enough to do but I built this for viewing and 3d printing, not porting to another game or system.