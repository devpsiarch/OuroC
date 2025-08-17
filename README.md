# 🌀 OuroC
> Project available for now only for Linux , Windows support soon. 

**Build recipes in pure C.**


## What is OuroC?

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
	    
	// run the command
    ouroc_run_cmd(&main);
    ouroc_run_cmd(&run);
	
	// clean up
    OUROC_KILL(&main);
    OUROC_KILL(&run);
    return 0;
}
```
### 4. Bootstrap the build file
#### On Linux
```bash
gcc build.c -o build
```
then run the build:
```bash
./build
```
