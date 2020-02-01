## Note: This repository is now read-only
As of January 2020 we will no longer be updating this repo or accepting new Pull Requests or issues. The code has migrated to the Autodesk [maya-usd repo](https://github.com/Autodesk/maya-usd) where it will continue to be maintained and developed by Luma Pictures, Autodesk, Pixar and others. 

# Maya to Hydra

Render a Maya scene using a Hydra render delegate.

## What's it good for?

The primary motivation behind this project is to use HdStorm, the
OpenGL renderer bundled with USD, as an alternative to
Viewport 2.0.

There are several advantages to HdStorm over Viewport 2.0:

1. Ability to cast shadows between [AL_USDMaya](www.github.com/AnimalLogc/AL_usdMaya) proxy shapes and real Maya shapes, using the [maya-to-hydra-alproxy](https://github.com/LumaPictures/maya-to-hydra-alproxy) adapter.
1. Consistent lighting and shading between Hydra-enabled applications: Maya, Katana, usdview, etc
1. HdStorm is under active development by Pixar
1. HdStorm is open source: you can add core features as you need them
1. HdStorm is extensible: you can create plugins for custom objects

Using Hydra also has beneftis for offline renderers. Any renderer that implements a Hydra render delegate can now have a fully interactive render viewport in Maya, along with support for render proxies via AL_USDMaya. This could be particularly useful for newer projects, like Radeon ProRender (which already has a [render delegate](https://github.com/GPUOpen-LibrariesAndSDKs/RadeonProRenderUSD)) or in-house renderers.

## Status

This project is still alpha.

## Building

Configure the build using cmake and run the build via your favourite build system.

The plugin targets Maya 2018, Linux and the latest USD dev branch, which is built including UsdMaya and UsdImaging. USD_ROOT can be passed to cmake to set the location to the compiled USD libraries.

Requirements:

| Package | Version |
| --- | --- |
| GCC | 4.8.5/6.3.1 |
| USD | 0.18.11/0.19.01/0.19.03/0.19.05/0.19.07/0.19.10 |
| Maya | 2018/2019 |
| Boost | 1.61 |
| TBB | 4.4+ |

Note: Support of smooth mesh preview is limited to Maya 2019.

## Environment configuration

Add `<prefix>/plug-ins` to MAYA_PLUG_IN_PATH and `<prefix>/lib` to PXR_PLUGINPATH_NAME when configuring the environment for Maya.

## List of supported Maya nodes

Shapes:
- mesh

Lights:
- areaLight
- pointLight
- spotLight
- directionalLight
- aiSkydomeLight

Shaders:
- UsdPreviewSurface
- lambert
- blinn
- file

## Contributing

The codebase (including the coding style) is highly volatile at this moment, breaking changes can arrive at any time. Check CONTRIBUTING.md for details about writing code for Maya to Hydra.
