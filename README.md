# DieKnow

This program should significantly improve the performance of your laptop, especially with the C++. The Python version is known to be relatively CPU-heavy (around 5%). However, this is minimal compared to DyKnow, which uses up to 15% of the CPU consistently, and on an Intel Core i3, there isn't much to spare.

You'll have to type "exit" to exit the application or some lingering multitasking threads will continue to run in the background.

## Installation

### For Python

1. Install Python 3
2. Install the following packages using `pip` in the Command Prompt
   ```
   py -m pip install psutil
   py -m pip install win32api
   ```
3. Run the program
4. Enjoy :)

### For C++

1. Build it via `g++`.
2. Run the generated executable

## How to use

1. Type "start" to start

## About

DyKnow creates executables dynamically. Once you kill its process using Task Manager or the `taskkill` command, it restarts right back up, but with a modified executable name. My approach leverages the Windows win32 API to repeatedly close DyKnow.
