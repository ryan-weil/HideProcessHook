# HideProcessHook
Written by me sometime in 2019, HideProcessHook is a DLL that hooks the NtQuerySystemInformation API and hides a process name. 
explorer.exe is used in this as an example.

Good for learning about basic byte-patch hooking on 32 bit and 64 bit systems.

# Usage

Inject the DLL into a process of the respective architecture, and its NtQuerySytemInformation will be hooked, hiding whatever process was specified to be hidden from the linked list

# Compatibility
Works on 32bit and 64bit systems, from Windows XP - Windows 10.

# Screenshots

Demo hiding the explorer.exe process in Task Manager:
![](https://i.imgur.com/vEET5Gw.gif)
