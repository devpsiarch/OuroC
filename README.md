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
    // declare the target and its file dependencies
    OUROC(main,"bin/main","main.c");
    // build the command to build
    OUROC_BUILD_CMD(&main,"gcc","main.c","-o","bin/main");

    OUROC(run,NULL,"bin/main");
    OUROC_BUILD_CMD(&run,"./bin/main");

    ouroc_run_cmd(&main);
    ouroc_run_cmd(&run);

    // clean up
    OUROC_KILL(&main);
    OUROC_KILL(&run);
    return 0;
}
```
ouroc also supports parallel builds (still in development) :
```c
    // declare the collection of INDEPENDENT build processes
    OUROC_POOL(procs);
    // i used the same file but you get the point
    OUROC(async,"bin/main","main.c");
    OUROC(async1,"bin/main1","main.c");
    OUROC(async2,"bin/main2","main.c");
    OUROC(async3,"bin/main3","main.c");

    OUROC_BUILD_CMD(&async,"gcc","main.c","-o","bin/main");
    OUROC_BUILD_CMD(&async1,"gcc","main.c","-o","bin/main1");
    OUROC_BUILD_CMD(&async2,"gcc","main.c","-o","bin/main2");
    OUROC_BUILD_CMD(&async3,"gcc","main.c","-o","bin/main3");

    ouroc_pool_run_async_single(&procs,&async);
    ouroc_pool_run_async_single(&procs,&async1);
    ouroc_pool_run_async_single(&procs,&async2);
    ouroc_pool_run_async_single(&procs,&async3);

    ouroc_pool_wait_all(&procs);

	// clean up
    OUROC_KILL(&async);
    OUROC_KILL(&async1);
    OUROC_KILL(&async2);
    OUROC_KILL(&async3);
    DA_FREE(&procs.procs);
```

### 3. Bootstrap the build file
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
> Windows support is shaky, please point out any problems.
