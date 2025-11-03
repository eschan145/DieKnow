# [1]

**Problem**: the DieKnow window is intended to start up and display the screen as shown in the video. Sometimes, an organization may have a specific policy blocking PowerShell from running, therefore the window shows up and then closes immediately.

**Solution**:

This solution involves running the Python script directly without the use of PowerShell which may be blocked.

1. Open the DieKnow directory
2. Open the `src/` folder within the DieKnow directory
3. Find the `main.py` file and double-click it to run. A window may open asking which app to start the file. In that case, press "Python" and then "Ok"
4. Run the DieKnow shell by typing `start` and then `<Enter>` as usual.
   
# [2]

**Problem**: the DieKnow home screen message does not show up and an error is shown instead.

**Solution**:

1. If the program crashes with `Windows fatal exception: page fault`, try closing other applications that may be using a lot of RAM. It may also be because DieKnow was installed in a folder locked by OneDrive, and OneDrive is not running. In that case, simply start OneDrive.
2. If the program crashes with `Windows fatal exception: stack overflow`, restart your device and try again.
3. If some error shows up related to DLL initialization:
      * You may be on a 32-bit machine or have a really, really ancient CPU
      * You may be on another platform besides Windows 11 or on ARM

# [3]

**Problem**: the DyKnow icon does not disappear from the system tray

**Solution**: there is no solution (at least without more information)

---

If none of the above resolves your problem or your problem is #3, post an issue in the Issues pane and I'll take a look at it.
