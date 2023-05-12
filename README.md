# HideProcessHook
Written by me sometime in 2019, HideProcessHook is a DLL that hooks the NtQuerySystemInformation API and hides a process name. 
explorer.exe is used in this as an example.

Good for learning about basic byte-patch hooking on 32 bit and 64 bit systems.

# Projects
## HideProcessHook
The actual DLL that is used to perform the hook. Upon loading the DLL, NtQuerySytemInformation will be hooked hiding explorer.exe from the linked list
## HideProcessHookTester
Loads HideProcessHook.dll, calls NtQuerySytemInformation and then prints out the results. If all is well, explorer.exe will not be in the output.
## HideProcessHookInjector
Injects HideProcessHook.dll into a pid specified by user input, hiding explorer.exe. Keep in mind that HideProcessHook.dll must be in the same current directory as the injector executable!

# Compatibility
Should work on all Windows versions, both 32 and 64 bit.

# Screenshots

Manually injecting the DLL into Task Manager using Process Hacker:
![](https://camo.githubusercontent.com/4161a66bde47c2f57d3483e6aaefdfab41798af58764e05314fc27873cebd500/68747470733a2f2f692e696d6775722e636f6d2f6f6479784b64652e676966)