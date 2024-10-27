# DieKnow

This program should significantly improve the performance of your laptop, especially with the C++. The Python version is known to be relatively CPU-heavy (around 5%). However, this is minimal compared to DyKnow, which uses up to 15% of the CPU consistently, and on an Intel Core i3, there isn't much to spare.

You'll have to type "exit" to exit the application or some lingering multitasking threads will continue to run in the background.

## Installation

NOTE: this application only works on Windows, as it uses the Windows API.

### For Python

1. Install Python 3
2. Run the program
3. Enjoy :)

## How to use

1. Type "start" to start

## About

DyKnow creates executables dynamically. Once you kill its process using Task Manager or the `taskkill` command, it restarts right back up, but with a modified executable name. My approach leverages the Windows win32 API to repeatedly close DyKnow.
