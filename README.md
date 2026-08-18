![](https://github.com/philj56/tofi/blob/master/screenshot_fullscreen.png)

A simple opengl launcher based on dmenu. one fine day i tried to
open dmenu. but it wont open at all. so i got angry, and wrote a
launcher myself.



# Build instructions

### Required dependencies
- `git` for cloning this repository
- `gl.h` which is available in your GPU drivers, and also `glfw` needs to be installed (dependencies)
- `cmake` for building this project.
- `nvim` or `vim`, your favourite text editor.


### Compilation guide

Get hold of the binary using these commands
```bash
git clone git@github.com:blameaniket/stunning-spoon.git
cd <directory>
mkdir build && cd build/ && cmake .. && cd ..
cmake --build build
```

After compilation run the app using this command:
```bash
./build/app
```

# License
MIT
