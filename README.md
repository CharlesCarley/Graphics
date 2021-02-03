# Graphics

Is a small 2D rendering library for use with OpenGL

## Building

This project builds with CMake and has a few external git repositories as dependencies.

+ [FreeImage](https://github.com/CharlesCarley/FreeImage)
+ [FreeType](https://github.com/CharlesCarley/FreeType)
+ [Utils](https://github.com/CharlesCarley/Utils)
+ [Math](https://github.com/CharlesCarley/Math)
+ [Image](https://github.com/CharlesCarley/Image)
+ [Window](https://github.com/CharlesCarley/Window)
+ [SDL](https://github.com/CharlesCarley/SDL)

The files [gitupdate.py](gitupdate.py) or [gitupdate.bat](gitupdate.bat) help automate initial cloning and keeping the modules up to date.

Once this project has been cloned. The following command will initialize the external modules.

```txt
python gitupdate.py 
...
gitupdate.bat 
```

then building with CMake.
```txt
mkdir Build
cd Build
cmake .. 
```

Optional defines.

+ Graphcs_BUILD_WINDOW - Build an optional C-API wrapper for window access. Default: ON
+ Graphcs_USE_SDL - Build the SDL backend for the window API. . Default: OFF
