# OS
The simplest example of an operating system.

### How to build on Linux
1. Clone repository
2. Enter to OS folder
3. Create **o** folder for .o files
4. Download cross-compiler([i386-elf 4.9.1](https://wiki.osdev.org/GCC_Cross-Compiler#Prebuilt_Toolchains "i386-elf 4.9.1"))
5. Install *nasm*
6. Clone [scalable-font2](https://gitlab.com/bztsrc/scalable-font2 "scalable-font")
7. Run build_all_crosscompile.sh (add *-d* to use Visual Studio Code GDB)

### What do I need to do to use gdb?
1. Add this line to the file *~/.gdbinit*
```sh
add-auto-load-safe-path ~/OS/.gdbinit
```
The OS folder is assumed to be located in the user`s folder

2. Create a file *.gdbinit* in the OS folder

3. Put it in the created folder *.gdbinit* file the following commands
```sh
target remote localhost:1234
symbol-file kernel.sym
break kmain
```

### How do I debug the kernel using Visual Studio Code and gdb?
0. (You may need to delete it *.gdbinit* files to avoid conflicts.)
You must install Native Debugger from the extensions
1. Run *build_all_crosscompile.sh -d*
You can also add -s "qemu parameters string" to change the qemu startup settings. (run. *build_all_crosscompile.sh info* to find out more)
2. Open *OS* folder in Visual Studio Code 
3. Run *Attach to QEMU* in Visual Studio Code
4. Run *system_reset* in the qemu monitor
