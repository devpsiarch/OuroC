----------

# 🌀 OuroC

**Build your C project using nothing but pure C!**

----------

## 🔧 What is OuroC?

**OuroC** is a build system _written in C, for C projects_. No Makefiles, no bash scripts — just pure C code building more C code. If you're tired of hiding behind tools and want full control over your build process (or just want to flex), this one's for you.

----------

## 🚀 How to Use

### 1. Add the Header

Place `ouroc.h` in your home/project directory.

### 2. Create a `build.c`

This file will contain your build instructions. Here's a basic example:

```c
#define OUROC_IMPLI
#include "./ouroc.h"
#include <stdio.h>

int main(void) {
    Builder builder = {
        .bdir = NULL,
        .buffer_used = 0,
        .buffer_max = 256
    };
    initbuilder(&builder);

    builder.appendcc(&builder, "clang");
    builder.appendtarget(&builder, "test");
    builder.appendsrcs(&builder, "test.c");
    builder.appendflags(&builder, "-Wall");
    builder.appendflags(&builder, "-Wextra");
    builder.appendflags(&builder, "-lm");
    builder.appendflags(&builder, "-o");

    builder.construct(&builder);
    SHOWCOMMAND(builder);
    builder.execute(&builder);
    builder.clean_up(&builder);
    
    return 0;
}

```

### 3. Or Construct in Real Time

```c
#define OUROC_IMPLI
#include "./ouroc.h"
#include <stdio.h>

int main(void) {
    Builder b = {
        .bdir = NULL,
        .buffer_used = 0,
        .buffer_max = 256
    };
    initbuilder(&b);

    b.appendcmd(&b, "gcc ");
    b.appendcmd(&b, "-Wall ");
    b.appendcmd(&b, "-Wextra ");
    b.appendcmd(&b, "-lm ");
    b.appendcmd(&b, "-o admin_test ");
    b.appendcmd(&b, "test.c");

    SHOWCOMMAND(b);
    b.execute(&b);
    b.clean_up(&b);

    return 0;
}

```

### 4. Build Once, Run Forever™

Just compile your builder once:

```bash
gcc build.c -o build
```

Then build your project anytime with:

```bash
./build
```

----------

## 🤔 Why OuroC?

> "If you’re building your C projects with anything **but** C, are you even a real programmer?"  

OuroC is here to prove a point — and maybe make your build process more fun and transparent along the way.

----------

## 🛠️ Roadmap

-   Optimized builder for larger projects (dependency tracking, parallel builds, etc.)
    
-   Support passing arguments directly: `./build run` or `./build test`
    

----------

## ❤️ Contribute

Wanna flex your low-level skills? PRs welcome — just keep it pure C.

----------
