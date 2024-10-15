# DieKnow

## Installation

1. Install Python 3
2. Install the following packages using `pip` in the Command Prompt
   ```
   py -m pip install psutil
   py -m pip install win32api
   ```
3. Run the program
4. Enjoy :)

## How to use

1. Type "start" to start

## About

DyKnow creates executables dynamically. Once you kill its process using Task Manager or the `taskkill` command, it restarts right back up, but with a modified executable name. My approach leverages the Windows win32 API to repeatedly close DyKnow.
