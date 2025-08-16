#include "template.h"
#include <sys/stat.h>
#include <stdarg.h>
#include <errno.h>


// petition to add colors
enum LogType {
    INFO,
    WARN,
    ERROR
};

void ouroc_log(enum LogType t, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    switch (t) {
        case INFO:
            printf("[INFO]: ");
            vprintf(fmt, args); // pass variadic args to vprintf
            printf("\n");
            break;
        case WARN:
            printf("[WARN]: ");
            vprintf(fmt, args);
            printf("\n");
            break;
        case ERROR:
            printf("[ERROR]: ");
            vprintf(fmt, args);
            printf("\n");
            break;
    }

    va_end(args);
}

struct stat get_stat(const char* filename){
    struct stat info;
    if(stat(filename,&info) != 0){
        if(errno == ENOENT){
            ouroc_log(ERROR,"File %s does not exist.",filename);
        }else{
            ouroc_log(ERROR,"Could not read %s.",filename);
        }
    }
    return info;
}

struct File {
    char* filename;
    struct stat info;
};

struct File* File(char* filename){
    struct File*result = malloc(sizeof(struct File));
    if(result == NULL){
        ouroc_log(ERROR,"Could not allocate memory for \"struct file\".");
        goto defer;
    }
    if(stat(filename,&result->info) != 0){
        switch(errno){
            case ENOENT:
                ouroc_log(ERROR,"File \"%s\" does not exist.",filename);
                break;
            case EACCES:
                ouroc_log(ERROR,"Dont have permission to get stat for \"%s\".",filename);
                break;
            case ENOTDIR:
                ouroc_log(ERROR,"Component in the path to \"%s\" isn’t a directory.",filename);
                break;
            default:
                ouroc_log(ERROR,"Could not get stat for file \"%s\".",filename);
                break;
        }
        goto defer;
    }
    result->filename = filename;
    return result;
defer:
    if(result != NULL) free(result);
    exit(1);
}

struct Rule {
    char* target;
    char* command;
    struct stat info;
    char* cc;
    DA_TEMPLATE(struct File*) source;
    DA_TEMPLATE(char*) args;
};

struct Rule* Rule(char*target,char* build_cmd){
    struct Rule* result = malloc(sizeof(struct Rule));
    if(result == NULL){
        ouroc_log(INFO,"Failed to allocate memory for Rule to build \"%s\" using \"%s\".",target,build_cmd);
        goto defer;
    }
    result->target = target;
    result->command = build_cmd;

    return result;
defer:
    if(result != NULL) free(result);
    exit(1);
}

void Rule_kill(struct Rule* rule){
    free(rule);
}

void Rule_make(struct Rule*rule){
    int ret = system(rule->command);
    if(ret == -1){
        ouroc_log(ERROR,"Failed to make rule \"%s\" using \"%s\".",rule->target,rule->command);
        exit(1);
    }
}


struct test {
    DA_TEMPLATE(char*) arr;
};

int main(void){
    // wrape this in a macro
    struct Rule rule = {
        .target = "test",
        .command = NULL,
        .info = get_info("test"),
        .cc = get_cc(),
    };
    DA_INIT(&(rule.args));   
    DA_INIT(&(rule.source));   

    
    DA_APPEND(&rule.source,File("test.c"));
    DA_APPEND(&rule.args,"-lm");

    DA_FREE_DATA(&rule.source);
    DA_FREE(&rule.source);
    DA_FREE(&rule.args);

    /*  
    struct Make main = {0};
    main.cmd_rule = "gcc main.c -o main";

    struct Make execute = {0};
    execute.cmd_rule = "./main";
    DA_APPEND(execute.makes,main);
    execute.build_this(&execute);
    */
    return 0;
    DA_TEMPLATE(char*) StringArray;
    DA_INIT(&StringArray);
    for(int i = 0 ; i < 10 ; ++i){
        DA_APPEND(&StringArray,"Hello"); 
    }
    for(int i = 0 ; i < DA_LEN(&StringArray) ; ++i){
        printf("Data[%d] %s\n",i,StringArray.data[i]); 
    }
    
    
    DA_FREE(&StringArray);
    return 0;
}
