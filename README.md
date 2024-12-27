# Introduction

**NOTE!** this utility only works if emulator uses **RawInput API**.

This project is a simple console program that aims to automate the updating of lightgun ids in [Sega Model 3 emulator](https://www.supermodel3.com/).
It does this by updating the `Supermodel.ini` file with new configuration.

You can pass two parameters to the program: `--gun1` is required, while `--gun2` is optional.
The program requires from the user to pass **vendor id** and **product id** of the lightgun, you can retrieve those by using [Arcade Guns Pro Utility](https://www.arcadeguns.com/index.php?main_page=page&id=23).

The best recommended use case for this utiltiy is to run each time when you start a game via [LaunchBox](https://www.launchbox-app.com/) and [Bulk Add/Remove Additional Applications addon](https://forums.launchbox-app.com/files/file/4375-bulk-addremove-additional-applications/).

# Limitations

Currently, the lightgun configuration is hardcoded inside the source code, if you want to personalize it, you need edit the source code and recompile the program.  
I know that this isn't ideal, especially when you want to include your own configuration, but for now it is what it is.

# Installation

Simply copy the `SindenM3Remap.exe` into root folder of the SegaM3 emulator, example path: `Your/Path/To/SegaM3/SindenM3Remap.exe`.  
You can download the precompiled version of this program from [releases page](https://github.com/Patrix9999/SindenM3Remap/releases).

# Usage example

### cmd.exe

```
SindenM3Remap.exe --gun1 "VID:16C0 PID:0F38" --gun2 "VID:16C0 PID:0F39"
```

### powershell

```
./SindenM3Remap --gun1 "VID:16C0 PID:0F38" --gun2 "VID:16C0 PID:0F39"
```

# Building

To build this utility, you only need this software:
- [Visual Studio](https://visualstudio.microsoft.com/pl/) (at least **2019**, make sure to install **C++ Workload** and **CMake Tools for Visual Studio**)

After having IDE installed, simply open up the folder cloned repository folder and build the project.  
The executable file can be located in `out/build/CONFIGURATION/` subdirectory.