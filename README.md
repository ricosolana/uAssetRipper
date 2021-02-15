# UnityAssetObjRipper
Convert asset files created by uTinyRipper (or other) into .obj models

## Usage
You will need a Python environment.

To begin ripping .obj models: `python.exe UnityAssetObjRipper.py input output`. This is the base command to convert one Unity .asset file, where `input` is the **file** path (with no slash at the end), and `output` is the output **directory** (not an output file).

To convert more than one asset, `python.exe UnityAssetObjRipper.py input output -all`. Same as before, except `input` and `output` are **directories** (no ending slashes).

## Examples

`python.exe UnityAssetObjRipper.py operating_chair.asset asset_out`

This *will* create a new directory called `asset_out`, with the new file `operating_chair.asset.obj`

`python.exe UnityAssetObjRipper.py pre_assets assets_out -all`

This will read from `pre_assets` and write all found `.asset` files as `.obj` to `assets_out`

## Notice

The source code is provided as is, you can look at it, I am not responsible for anything that happens to your system while using it. 

Some `.asset` files might not be read correctly since different versions of Unity and different `.asset` formats might not be compatible with this tool. Use it at your own will.

## Samples

From Lurking Game

![alt text](samples.PNG?raw=true "Title")
