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

| Option                       | Description                                                     | Default |
|:-----------------------------|:----------------------------------------------------------------|:--------|
| Graphcs_BUILD_WINDOW         | Build an optional C-API wrapper for window access.              | ON      |
| Graphcs_USE_SDL              | Build the SDL backend for the window API.                       | OFF     |
| Graphics_NO_PALETTE          | Disable the built in color palette.                             | OFF     |
| Graphics_OP_CHECKS           | Add extra checks for function parameters.                       | ON      |
| Graphics_EXTRA_BUILTIN_FONTS | Include extra [fonts](https://fonts.google.com/) in the binary. | OFF     |
