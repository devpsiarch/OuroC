#include <stdio.h>
#include "./ouroc.h"
#define OUROC_IMPLI
int main(int argc,char*argv[]){
    if(argc >= 2){
        if(strcmp(argv[1],"-h") == 0){
            printf(QUICK_MANUAL);
            return 0;
        }
    }
    // these two lines should be expressed in a macro
    string_vec test;
    initstring_vec(&test);

    test.append(&test,"hello");
    test.append(&test,"elden");
    test.append(&test,"ring");
    SHOWVEC(test);
    
    test.destroy(&test);
    // char*cc = "clang";
    // char*flags = "-lm";
    // char*file = "test.c";
    // cmd("test",cc,flags,CFLAGS,file,"-o","test");
    
    // well use a macro do to all the ugly code job
    // the API will look something like this ...
    // very original i know
    // these two lines should be expressed in a macro
    Builder tester = {
        .bdir = NULL, 
        .buffer_used = 0,
        .buffer_max = 256
    };
    initbuilder(&tester);
    
    tester.appendcc(&tester,"clang");
    tester.appendtarget(&tester,"test");
    tester.appendsrcs(&tester,"test.c");
    tester.appendflags(&tester,"-Wall");
    tester.appendflags(&tester,"-Wextra");
    tester.appendflags(&tester,"-lm");
    tester.appendflags(&tester,"-o");
    tester.construct(&tester);
    SHOWCOMMAND(tester);
    tester.execute(&tester);
    tester.clean_up(&tester);
    
    Builder admin = {
        .bdir = NULL, 
        .buffer_used = 0,
        .buffer_max = 256
    };
    initbuilder(&admin);
    admin.appendcmd(&admin,"gcc ");
    admin.appendcmd(&admin,"-Wall ");
    admin.appendcmd(&admin,"-Wextra ");
    admin.appendcmd(&admin,"-lm  ");
    admin.appendcmd(&admin,"-o admin_test ");
    admin.appendcmd(&admin,"test.c");
    SHOWCOMMAND(admin);
    admin.execute(&admin);
    return 0;
    // Build builder = {};
    // builder.appendcc("clang");
    // builder.appendtarget("main");
    // builder.appendsrc("...",...);
    // builder.appendflags("...",...);
    // builder.appendbuilddir("...",...);
    // builder.build();
}
