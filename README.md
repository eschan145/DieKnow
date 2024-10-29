# DieKnow

This program should significantly improve the performance of your laptop, especially with the C++. The Python version is known to be relatively CPU-heavy (around 5%). However, this is minimal compared to DyKnow, which uses up to 15% of the CPU consistently, and on an Intel Core i3, there isn't much to spare.

You'll have to type "exit" to exit the application or some lingering multitasking threads will continue to run in the background.

## Installation

NOTE: this application only works on Windows, as it uses the Windows API.

### For Python


1. Install Python 3
2. Click on Code
   ![image](https://github.com/user-attachments/assets/31ca7d0e-eaad-4a46-a11b-b38216639b05)
3. Click on Download Zip
   ![image](https://github.com/user-attachments/assets/1b77af9c-c6ce-4197-94d8-29ae63c499c5)
4. Extract the compressed zip
5. Double-click on the [`main.py`](main.py) file in the extracted folder.
6. Enjoy :)

## Commands

### `start`

Start the DieKnow process. DyKnow executables will be terminated forcefully every five seconds, which is sufficient to keep DyKnow consistently closed down. If the delay was too low (or none at all), CPU usage would be very high, possibly as high or higher than DyKnow.

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

DieKnow provides an API that is accessible at [`dieknow.py`](dieknow.py), which just calls the C++ functions.

## About

DyKnow creates executables dynamically. Once you kill its process using Task Manager or the `taskkill` command, it restarts right back up, but with a modified executable name. How it does this is unknown, but it likely uses Task Scheduler. My approach leverages the Windows win32 API, specifically the [`TerminateProcess`](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-terminateprocess) function, to repeatedly close DyKnow.

A `ctypes` precompiled C++ binary is located in [`api.dll`](api.dll), which is accessed by [`main.py`](main.py) to call the C++ functions. C++ is used as it lowers the CPU usage of DieKnow compared to Python. The DLL file is over 3 MBs because it is statically built (with use of the `-static` g++ option), allowing easy distribution of it.

Using a command such as [`taskkill`](https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/taskkill) will result in an error: `Access is Denied`.
