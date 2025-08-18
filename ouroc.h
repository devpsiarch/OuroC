#ifndef OUROC_H
#define OUORC_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

/*
------------------------------------------------------------------------------
    template.h -- small C framework
------------------------------------------------------------------------------
    Definition of the template.h header file that serves as a small
    framework that "facilitates" working with the C programming language.
    (The name might be a bit misleading but idk, that’s how I named the
    first file and I’m used to it now).

    More features will be added soon, like useful data structures and
    some algorithms and "STL"-style containers.

TODO
    -> More features

------------------------------------------------------------------------------
*/

#define ASSERT(cond,...)                                                \
do{                                                                     \
    if(!(cond)){                                                        \
        fprintf(stderr,"%s,%d: ASSERTION FAILED: ",__FILE__,__LINE__);  \
        fprintf(stderr,__VA_ARGS__);                                    \
        fprintf(stderr,"\n");                                           \
        exit(1);                                                        \
    }                                                                   \
}while(0)

struct da_impl {
    size_t len;
    size_t elem_size;
    size_t capacity;
};


#define DA_TEMPLATE(t) struct {struct da_impl impl;t *data;}

#define DA_RESET(a) (a)->impl.len = 0
#define DA_LEN(a) (a)->impl.len

#define DA_INIT(a)                                  \
    da_init_impl(&(a)->impl,sizeof(*(a)->data));    \
    (a)->data = NULL

#define DA_ALLOC(a,size)                            \
do{                                                 \
    (a)->data = malloc((size)*(a)->impl.elem_size); \
    (a)->impl.capacity = (size);                    \
}while(0)                                       

#define DA_FREE(a) free((a)->data)
#define DA_FREE_DATA(a)                         \
do{                                             \
    for(size_t i = 0 ; i < DA_LEN((a)) ; ++i){  \
        free((a)->data[i]);                     \
    }                                           \
}while(0)

#define INIT_DA_CAPACITY 512

#define DA_APPEND(a,value)                                                                  \
do{                                                                                         \
    if((a)->impl.len >= ((a)->impl.capacity)){                                              \
        (a)->impl.capacity = ((a)->impl.len == 0) ? INIT_DA_CAPACITY : (a)->impl.capacity*2;\
        (a)->data = realloc((a)->data,(a)->impl.capacity*(a)->impl.elem_size);              \
        ASSERT((a)->data != NULL,"ERROR: failed to reallocate memory for DA.\n");           \
    }                                                                                       \
    (a)->data[(a)->impl.len++] = (value);                                                   \
}while(0)

#define DA_POP(a)   \
do{                 \
    --(a)->impl.len;\
}while(0)           

#define DA_GET(a,index) (a)->data[(index)]

#define DA_PRINT(a)                             \
do{                                             \
    for(size_t i = 0 ; i < DA_LEN((a)) ; ++i){  \
        printf("%s\n",DA_GET((a),i));           \
    }                                           \
}while(0)

#define DA_RESERVE(a,size) (a)->impl.capacity = (size);


typedef struct {
    char* data;
    struct da_impl impl;
}StringOwn;

#define STRING_OWN(name)    \
    StringOwn name;         \
    DA_INIT(&name);         \
    DA_APPEND(&name,'\0')   

#define STRING_OWN_APPEND(str,c)        \
do{                                     \
    (str)->data[DA_LEN(str)-1]=(c);     \
    DA_APPEND((str),'\0');              \
}while(0)                           
#define STRING_OWN_POP(str)                 \
do{                                         \
    if(DA_LEN((str)) > 1){                  \
        DA_POP((str));                      \
        (str)->data[DA_LEN(str)-1]='\0';    \
    }                                       \
}while(0)

#define STRING_OWN_RESERVER(str) DA_RESERVE((str))
#define STRING_OWN_FREE(str) DA_FREE((str))
#define STRING_OWN_LEN(str) DA_LEN((str))-1
#define STRING_OWN_CAT(str,elem)            \
do{                                         \
    size_t len = strlen((elem));            \
    for(size_t i = 0 ; i < len ; ++i){      \
        STRING_OWN_APPEND(str,(elem)[i]);   \
    }                                       \
}while(0)

/*
 *
 * Function delcaration for template.h
 *
*/
// creates a string on the heap and returns its pointer
char* heap_string(const char* stack_string);
// initilizes the "struct da_impl" part of the dynamic array that defines:size,cap.data...
void da_init_impl(struct da_impl* impl,size_t elem_size);

/*
------------------------------------------------------------------------------
    ouroc.h -- build recipes header
------------------------------------------------------------------------------
    The main definition for the ouroc.h header file that describes
    the creating of build recipes.

    Current support: Linux
    hopes and prayers for Windows support. (fr maybe soon, when i feel like it).

USAGE
    The library is supposed to utilize the macros that facilitate
    recipes building, although I recommend giving the library a read.

    Here is a sample file:

        int main(void){
            // macro that initializes everything u need for ouroc object
            OUROC(main,"test","test.c");
            OUROC_BUILD_CMD(&main,"gcc","test.c","-o","test");

            OUROC(run,NULL,"test");
            OUROC_BUILD_CMD(&run,"./test");

            ouroc_run_cmd(&main);
            ouroc_run_cmd(&run);

            // macro for cleaning up everything
            OUROC_KILL(&main);
            OUROC_KILL(&run);
            return 0;
        }

TODO
    -> More features
    -> More options for functionality/usage
    -> Nested dependencies management
    -> Better Async/sync builds
    -> Better logging messages
    -> Color in logging

------------------------------------------------------------------------------
*/

enum LogType {
    INFO,
    WARN,
    ERROR
};

struct ouroc {
    char* target;
    DA_TEMPLATE(char*) depend;
    DA_TEMPLATE(char*) stream;
};

#define OUROC_BUILD_CMD(cmd, ...)                               \
    ouroc_append_stream_many(cmd,                               \
    (sizeof((const char*[]){__VA_ARGS__})/sizeof(const char*)), \
    __VA_ARGS__)


#define OUROC_INIT(cmd,target, ...)                             \
    ouroc_init_many(cmd,                                        \
    target,                                                     \
    (sizeof((const char*[]){__VA_ARGS__})/sizeof(const char*)), \
    __VA_ARGS__)

#define OUROC_KILL(cmd)      \
    DA_FREE(&(cmd)->stream);  \
    DA_FREE(&(cmd)->depend);


#define OUROC(name,...)             \
    struct ouroc name = {0};        \
    OUROC_INIT(&name,__VA_ARGS__)
    
/* async build commands here */

typedef pthread_t ouroc_proc;

struct ouroc_pool {
    DA_TEMPLATE(ouroc_proc) procs;
};

#define OUROC_POOL(name) struct ouroc_pool name; DA_INIT(&name.procs)
#define OUROC_POOL_APPEND(master,proc) DA_APPEND(&(master)->procs,(proc))


/*
 *
 * Function declarations for the core ouroc.h header file.
 * 
 * small explination provided for each function.
 *
 * */


// duh logs to stdout a message (add '\n' on its own)
void ouroc_log(enum LogType t, const char *fmt, ...);
// returns status of a file and exit(1) if fail
struct stat get_file_state(const char* filename);
// addes pointers to strings to build "stream" to be executed
void ouroc_append_stream_many(struct ouroc*master,const unsigned int count,...);
// runs the command after it has been assembled from "stream"
void ouroc_run_cmd(struct ouroc*master);
// inititilizes the "target" and its file "dependencies" 
void ouroc_init_many(struct ouroc*master,char* target,const size_t count,...);

/* async function delcaration */

// man-in_middle function that pthread api uses , it calls "ouroc_run_cmd"
void* ouroc_build_thread_porter(void*arg);
// adds then runs async the build process for "value"
void ouroc_pool_run_async_single(struct ouroc_pool* master,struct ouroc* value);
// waits for all the ouroc processes, then cleans up
void ouroc_pool_wait_all(struct ouroc_pool* master);


#endif
#ifndef OUROC_IMPLI
#define OUORC_IMPLI


/*
 *
 * Function implimentation for template.h
 *
*/
char* heap_string(const char* stack_string){
    size_t len = strlen(stack_string);
    char* str = malloc(sizeof(char)*(len+1));
    memcpy(str,stack_string,len+1);
    return str;
}
void da_init_impl(struct da_impl* impl,size_t elem_size){
    impl->len = 0;
    impl->elem_size = elem_size;
    impl->capacity = 0;
}

/*
 *
 * Function implimentation for core ouroc.h
 *
*/

void ouroc_log(enum LogType t, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    switch (t) {
        case INFO:
            printf("[INFO]: ");
            vprintf(fmt, args);
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

struct stat get_file_state(const char* filename){
    struct stat info;
    if(stat(filename,&info) != 0){
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
        exit(1);
    }
    return info;
}
void ouroc_append_stream_many(struct ouroc*master,const unsigned int count,...){
    char* next;
    va_list args;
    va_start(args,count);
    for(unsigned int i = 0 ; i < count ; ++i){
        next = va_arg(args,char*);
        DA_APPEND(&master->stream,next);
    }
    va_end(args);
}
void ouroc_run_cmd(struct ouroc*master){
    /* Check if target exists */
    if(access(master->target,F_OK) != 0) goto rebuild_target;
    struct stat target_info = get_file_state(master->target);
    /* Check for changed dependency files */
    for(size_t i = 0 ; i < DA_LEN(&master->depend) ; ++i){
        char* file = master->depend.data[i];
        struct stat dep_info = get_file_state(file);
        if(dep_info.st_mtime > target_info.st_mtime) goto rebuild_target;
    }
    return;
rebuild_target:
    /* Building the command to be executed */
    STRING_OWN(command);
    for(size_t i = 0 ; i < DA_LEN(&master->stream) ;++i){
        char* arg = master->stream.data[i];
        STRING_OWN_CAT(&command,arg);
        STRING_OWN_APPEND(&command,' ');
    }    
    if(master->target == NULL) ouroc_log(INFO,"executing.");
    else ouroc_log(INFO,"Building \"%s\".",master->target);
    int ret = system(command.data);
    if(ret != 0){
        ouroc_log(ERROR,"Failed building \"%s\".",master->target);
        exit(1);
    }
    if(master->target == NULL) ouroc_log(INFO,"execution done.");
    else ouroc_log(INFO,"\"%s\" done.",master->target);
    STRING_OWN_FREE(&command); 
}
void ouroc_init_many(struct ouroc*master,char* target,const size_t count,...){
    master->target = target;
    DA_INIT(&master->depend);
    DA_INIT(&master->stream);
    char* next;
    va_list args;
    va_start(args,count);
    for(size_t i = 0 ; i < count; i++){
        next = va_arg(args,char *);
        DA_APPEND(&master->depend,next);
    }
    va_end(args);
}

/* asyn implimentation */

void* ouroc_build_thread_porter(void*arg){
    struct ouroc* obj = arg;
    ouroc_run_cmd(obj);
    return NULL;
}

void ouroc_pool_run_async_single(struct ouroc_pool* master,struct ouroc* value){
    ouroc_proc proc;
    pthread_create(&proc,NULL,ouroc_build_thread_porter,value);
    OUROC_POOL_APPEND(master,proc);
}

void ouroc_pool_wait_all(struct ouroc_pool* master){
   for(size_t i = 0 ; i < DA_LEN(&master->procs) ; i++){
        pthread_join(master->procs.data[i],NULL);
   }
   DA_RESET(&master->procs);
}

#endif
