

![](./assets/launcher_screenshot.png)

**stunning-spoon (beta v1.0.0)**

A simple and customizable opengl launcher based on dmenu.



# Build instructions

### Required dependencies
- `git` for cloning this repository
- `cmake` for building this project, and for windows **MSVC build tools** is
  required along with `nmake`. the build script of this project uses nmake for
  windows
- `vcpkg` for installing required dependencies
- all opengl related files: `gl.h` or `glad` is required which already included
  in the external folder, and also `glfw` needs to be installed, you can
  install them using `vcpkg`.
- `nvim` or `vim`, your favourite text editor.



### Compilation guide

#### a) For linux:

Get hold of the binary using these commands
```bash
git clone git@github.com:blameaniket/stunning-spoon.git
cd <directory>
vcpkg install
mkdir build && cd build/ && cmake .. && cd ..
cmake --build build
```

After compilation run the app using this command:
```bash
./build/app
```

#### b) For windows:

First setup windows command line with the batchfile `setup_cmdline_windows.bat`:
```bat
.\setup_cmdline_windows.bat
vcpkg install
```

then build the project with `build_windows.bat`:
```bat
.\build_windows.bat
```

finally run the binary:
```bat
.\build\app.exe
```


# Configuration

>[!NOTE]
>There are two config files, which contains the commands which will
>appear in the menu. for **linux** it is `config_entries` and for **windows** it is
`config_entries.windows`.

You can directly configure the look of the launcher from the `src/main.c` file.
The thing you can also configure is the entries part. You can put whatever
entries you want like a bookmark. to configure the entries, edit the
`config_entries` file.

An example `config_entries` file would be:
```text
firefox: firefox
discord: /usr/sbin/vesktop
poweroff: poweroff
reboot: reboot
```

In the entry `discord: /usr/sbin/vesktop`, `discord` appears on the launcher
and the command that gets executed is `/usr/sbin/vesktop`.



# License
MIT
