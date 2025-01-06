# DieKnow

A bypass of DyKnow, powered primarily by the Windows win32 API.

DieKnow will significantly improve the performance of your laptop by a large amount on your CPU. It will use up to 3% CPU, higher than optimal but still much lower than DyKnow's 15%.

For ease of use, a beta GUI application is located in [`gui.pyw`](src/gui.pyw), built entirely by the Windows API without an external library such as Qt or GTK.

# Desktop Applications

<table border="0">
 <tr>
    <td><b style="font-size:30px">DieKnow Shell</b></td>
    <td><b style="font-size:30px">DieKnow GUI</b></td>
 </tr>
 <tr>
    <td><img src="https://github.com/user-attachments/assets/5b02591e-28fd-450e-89fd-8b3fcf540f42" alt="Screenshot of DieKnow Shell"></td>
    <td><img src="screenshots/screenshot4.png" alt="Screenshot of DieKnow GUI"></td>
 </tr>
</table>

Compilation instructions can be found [here](COMPILING.md).

About this:
* This **DOES NOT** need administrator privileges, just a Python installation, which is often found in the Software Center.
* It's very compact (just around 600 lines).
* **DOES NOT** need a C++ compiler.
* **DOES NOT** need to run an executable or a batch file, which is usually blocked by the school via Windows AppLocker.
* **DOES NOT** have any external dependencies.
* Written in robust C++ code for maximum performance.

What can DyKnow do:

* Monitor your device screen (including when you press "Show password" when typing a password)
* Monitor your search history
* Monitor what apps you've been on
* Lock your device
* Track your device's location
* Make your computer really, really slow
* Track your keystrokes
* Make your computer useless

When it is running, you'll see the DyKnow icon disappear from your taskbar. 💀

## Installation

> [!IMPORTANT]
> This application only works on a 64-bit Windows, as it uses the Windows API and has been compiled with 64-bit gcc.

### For Python

1. Install Python 3. On most school computers it should be installed by default.
2. Click on **Releases**, found on the right side panel. If you're feeling brave, you can download the latest development ZIP. Note that this may not be as subject to as much testing as release branches, so downloading it is not recommended.

   ![releases](screenshots/releases.png)

3. Download the `Complete.zip` file.
4. Extract the compressed zip.
5. Double-click on the [`DieKnow.lnk`](DieKnow.lnk) shortcut file in the extracted folder.
6. Enjoy :)

## Commands

### Quickstart

Double-click the [`main.py`](src/main.py) and type `start` to start the monitoring and closing of DyKnow. If a shell is too complicated for you, you can use the GUI application in [`gui.pyw`](src/gui.pyw) by double-clicking it. Ensure Python is installed.

If you want to allow DyKnow to start up once the computer is logged on, you can perform the following steps:

1. Create a shortcut to the [`main.py`](src/main.py) or [`gui.pyw`](src/gui.pyw) file, depending if you want to start up with a GUI or a console.
2. Cut the shortcut by pressing `Ctrl-X`.
3. Press `Windows-R` to open up the Run window.
4. Type `shell:startup` in the input box that appears.
5. In the Windows Explorer dialog that appears, paste the shortcut you copied in step 2.

That's it! It may take around two minutes for it to start up.

### `start`

Start the DieKnow process. DyKnow executables will be terminated forcefully every five seconds, or whatever is set in [`interval.txt`](interval.txt), which is sufficient to keep DyKnow consistently closed down. If the delay was too low (or none at all), CPU usage would be very high, possibly as high or higher than DyKnow.

### `stop`

Kill the DieKnow threads but keep the app running. Threads associated with DieKnow will be terminated.

### `count`

Retrieve the number of executables killed by DieKnow.

### `directory`

Retrieve the files in the DyKnow installation directory.

It should return something similar to this.

```
Files in C:/Program Files/DyKnow/Cloud/7.10.22.9:
amjbk.exe
Demo32_64.exe
Demo64_32.exe
dkInteractive.exe
DyKnowLogSender.exe
DyKnowTest.exe
kyplu.exe
MonitorStateReader.exe
winProcess.exe
```

Here, `kyplu.exe` and `amjbk.exe` are the main DyKnow monitoring executable, but as the name is changed randomly each time it’s restarted, it will vary.

### `exit`

Exit the DieKnow application and destroy all threads associated with it.

## DieKnow API

DieKnow provides an API that is accessible at [`dieknow.py`](src/dieknow.py), which just calls the C++ functions.

## About

DyKnow creates executables dynamically. Once you kill its process using Task Manager or the `taskkill` command, it restarts right back up every few seconds, but with a modified executable name. How it does this is unknown, but it likely uses Task Scheduler. My approach leverages the Windows win32 API, specifically the [`TerminateProcess`](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-terminateprocess) function, to repeatedly close DyKnow.

A `ctypes` precompiled C++ binary is located in [`api.dll`](src/api.dll), which is accessed by [`main.py`](src/main.py) to call the C++ functions. C++ is used as it lowers the CPU usage of DieKnow compared to Python. The DLL file is over 3 MBs because it is statically built (with use of the `-static` g++ option), allowing easy distribution of it and without having to manage all the dependencies, such as `user32.dll` or `kernel32.dll`.

Using a command such as [`taskkill`](https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/taskkill) will result in an error: `Access is Denied`.

I was able to program this bypass not because I'm smart, but because the people who programmed DyKnow weren't or thought the students were stupid or both.

**Directory structure**:

* [`src`](src/) - Python and C++ source files
   * [`dlls`](src/dlls/) - precompiled C++ source files as shared objects. It's advised not to mess around with these files.
      * [`api.dll`](src/dlls/api.dll) - compiled DieKnow C++ API
      * [`gui.dll`](src/dlls/gui.dll) - compiled DieKnow GUI
   * [`api.cpp`](src/api.cpp) - DieKnow functions and C++ API
   * [`gui.cpp`](src/gui.cpp) - GUI application
   * [`system.cpp`](src/system.cpp) - system interaction and processing
   * [`settings.cpp`](src/settings.cpp) - settings loader for DieKnow
   * [`dieknow.py`](src/dieknow.py) - DieKnow Python API
   * [`main.py`](src/main.py) - Shell-like interface to DieKnow API
   * [`gui.pyw`](src/gui.pyw) - Python link to C++ GUI
* [`tests`](tests/) - nonstatic build testing
   * [`testdll.py`](tests/testdll.py) - Dependency checker for DLLs

Associated `*.h` files in the `src` directory are the includes for the implementation found in `*.cpp`.

### FAQs

#### Why is C++ and Python used and not only Python?

The Intel Core i3 CPU is quite slow and at the lower end of Intel's CPU lineup. Therefore it is important for highest efficiency to use C++, as Python is built on C and very inefficient. DieKnow does not only use C++ because most executables are blocked by organizations' AppLocker policy. This can be mitigated by using ctypes and building a shared object or dynamic link library from C++ files.

#### How can I compile this myself?

First, ensure you have everything set up to run DieKnow. Take a look at the GitHub Actions [`build.yml`](.github/workflows/build.yml) workflow and follow along with it. You'll need a C++ compiler, preferably `g++` or MSVC, compile it as a shared object (with the `-shared` flag), and link the required libraries (`-lgdi32` and `-lcomctl32`). The commands DieKnow uses to build itself are:

```bash
g++ -Ofast -Wall -shared -std=c++20 -static -o src/dlls/api.dll src/api.cpp -lgdi32
g++ -Ofast -Wall -shared -std=c++20 -static -o src/dlls/gui.dll src/gui.cpp -lgdi32 -lcomctl32
```

#### I'm getting high CPU usage for DieKnow. What can I do?

I'm working to optimize the DieKnow process, and it uses higher CPU than it should. However, it still significantly less than what DyKnow uses.

You can build DieKnow yourself and specify the flag `march=native` in the `g++` command. This asks the compiler to enable optimizations targeted specifically towards your CPU.
