# Compiling DieKnow

> **Ensure you are on a Windows machine before attempting to compile!**

## 1) Installing gcc

You'll need a C++ compiler in order to compile DieKnow's C++ functions. I used [gcc](https://github.com/gcc-mirror/gcc), but MSVC should work as well. The following compilation instructions apply for gcc.

1. Download the [MSYS2 installer](https://github.com/msys2/msys2-installer/releases/download/2024-07-27/msys2-x86_64-20240727.exe)
2. Open the MSYS2 installer and follow the instructions to install MSYS2. Copy the path that you inputed when asked. You'll need it later.
3. Once it has finished installing, open up the newly installed app "MSYS2 UCRT64", found by typing the name after pressing the Windows key.
4. Type the following command to synchronize the MSYS2 package database. The terminal will close, and you'll have to reopen it again.

   ```bash
   pacman -Syu
   ```
5. Once the terminal reopens, type the following command to install `g++`

   ```bash
   pacman -S mingw-w64-x86_64-gcc
   ```
6. Congratulations! You should have gcc installed on your machine. However, you'll need to add it to your system's PATH environment variable for it to be accessible.

### Adding to PATH

1. Press the Windows key and type "Edit environment variables for your account".
2. On the top half of the screen, go to "Path".
3. Press Alt-E and then Alt-N
4. Paste the installation directory of MSYS2 you copied when installing MSYS2 and then type `mingw64\bin` to complete the path.
5. To check if it works, open up a Command Prompt window and type `g++`. If it says "no input files", you have successfully added g++ to your path.

## 2) Using the compilation command

1. Use `cd` to cd to the directory where you downloaded DieKnow.
2. Open up a Command Prompt or Powershell window and type the following command to compile `api.cpp` into a DLL.

   ```bash
   g++ -Ofast -Wall -shared -std=c++20 -static -o src/dlls/api.dll src/api.cpp -lgdi32
   ```

3. If it works, type the following command to compile the GUI:

   ```bash
   g++ -Ofast -Wall -shared -std=c++20 -static -o src/dlls/gui.dll src/gui.cpp -lgdi32 -lcomctl32
   ```

Several options are used:

* `-Ofast` compiles into the FASTEST DLL as it possibly can.
* `-Wall` enables all compile warnings.
* `-std=c++20` sets the C++ standard to C++20.
* `-static` links the DLL dependencies statically.
* `-lgdi32`, `-lcomctl32` link the needed libraries for Graphics Driver Interface and Windows Common Controls, respectively.
