# 🌀 OuroC

**OuroC** is a stb-style header only file library that provides a simpler way to create build recipes using only C language.  it also provides small framework containing useful functionality as a desert.

## Getting started

### 1. Add the Header

Place `ouroc.h` in your root project directory.

### 2. Create a `build.c`

This file will contain your build instructions. Here's a basic example:

```c
#include "ouroc.h"
#define OUROC_IMPLI

int main(void){
    OUROC(main,"bin/main","main.c");
        OUROC_BUILD_CMD(&main,"gcc","main.c","-o","bin/main");

    OUROC(run,NULL,"bin/main");
        OUROC_BUILD_CMD(&run,"./bin/main");

    ouroc_run_cmd(&main);
    ouroc_run_cmd(&run);

    OUROC_KILL(&main);
    OUROC_KILL(&run);
    return 0;
}
```

ouroc also supports parallel builds:

```c
    // specify the number of threads you want to use
    OUROC_ASYNC(procs,master,2);

    char* target  = WIN_ELSE("exe/main.exe",  "bin/main");
    char* target1 = WIN_ELSE("exe/main1.exe", "bin/main1");
    char* target2 = WIN_ELSE("exe/main2.exe", "bin/main2");
    char* target3 = WIN_ELSE("exe/main3.exe", "bin/main3");
    
    OUROC_ASYNC_CREATE(async,target,"main.c");
        OUROC_ASYNC_RUN(procs,master,async,"gcc","main.c","-o",target);

    OUROC_ASYNC_CREATE(async1,target1,"main.c");
        OUROC_ASYNC_RUN(procs,master,async1,"gcc","main.c","-o",target1);

    OUROC_ASYNC_CREATE(async2,target2,"main.c");
        OUROC_ASYNC_RUN(procs,master,async2,"gcc","main.c","-o",target2);

    OUROC_ASYNC_CREATE(async3,target3,"main.c");
        OUROC_ASYNC_RUN(procs,master,async3,"gcc","main.c","-o",target3);

    ouroc_pool_wait_all(&procs);

    OUROC_ASYNC_FREE(procs,master);
```

### 3. Bootstrap the build file
Compile the your build file once and use it to build your projects forever ! 
(until you need to change the recipe).
#### On Linux
```bash
gcc build.c -o build
```
then run the build:
```bash
./build
```
#### On windows
BootStrap the build.c file (using mingw assuming you have PATH setup):
```bash
x86_64-w64-mingw32-gcc build.c -o build.exe
```
then run the build (on cmd):
```cmd
build.exe
```
or on powershell:
```powershell
.\build.exe
```
