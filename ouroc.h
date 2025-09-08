#ifndef OUROC_H
#define OUORC_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <errno.h>

#ifdef _WIN32
  #include <windows.h>
  #include <io.h>
  #include <process.h>
#else
  #include <unistd.h>
  #include <pthread.h>
#endif

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
    -> [...] More features

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

#ifdef _WIN32
    #define WIN_ELSE(_win,_else) (_win)
#else
    #define WIN_ELSE(_win,_else) (_else)
#endif

struct da_impl {
    size_t len;
    size_t elem_size;
    size_t capacity;
};


#define DA_TEMPLATE(t) struct {struct da_impl impl;t *data;}

#define DA_INIT(a)                                  \
    da_init_impl(&(a)->impl,sizeof(*(a)->data));    \
    (a)->data = NULL


#define DA_TEMPLATE_INIT(t,name)  \
    DA_TEMPLATE(t) name;          \
    DA_INIT(&name)

#define DA_RESET(a) (a)->impl.len = 0
#define DA_LEN(a) (a)->impl.len

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


struct rb_impl {
    size_t count;
    size_t head;
    size_t tail;
    size_t elem_size;
    size_t capacity;
    bool fixed;
};

// tells the ring buffer if it should grow (like a dynamic array) or stay fixed in capacity
// depending on the use of the user
#define FIXED true
#define NON_FIXED false

#define RB_TEMPLATE(t) struct {struct rb_impl impl;t *data;}

#define RB_INIT(a,capacity,type)                            \
    rb_init_impl(&(a)->impl,sizeof(*(a)->data),(capacity)); \
    (a)->data = malloc((a)->impl.elem_size*(capacity));     \
    (a)->impl.fixed = (type)

#define RB_FREE(a) \
    free((a)->data);


#define RB_TEMPLATE_INIT(type,t,cap,name)   \
    RB_TEMPLATE(t) name;                    \
    RB_INIT(&name,(cap),(type))


#define RB_FULL(a) (a)->impl.count == (a)->impl.capacity
#define RB_EMPTY(a) (a)->impl.count == 0
#define RB_COUNT(a) (a)->impl.count
#define RB_FIXED(a) (a)->impl.fixed
#define RB_CAP(a) (a)->impl.capacity
#define RB_HEAD(a) (a)->impl.head
#define RB_TAIL(a) (a)->impl.tail

/* These bellow are not safe, user should always check if rb is empty or not */
#define RB_BACK(a) (a)->data[RB_HEAD(a)]
#define RB_FRONT(a) (a)->data[RB_TAIL(a)-1]

#define RB_ENQUEUE(a,value)                                             \
do{                                                                     \
    if(!(RB_FULL(a))){                                                  \
        (a)->data[(a)->impl.tail] = (value);                            \
        (a)->impl.tail = ((a)->impl.tail + 1) % ((a)->impl.capacity);   \
        ++RB_COUNT(a);                                                  \
    }else{                                                              \
        if(!RB_FIXED(a)){                                               \
            /* reallocate and reorder the ring buffer*/                 \
            printf("reallocating not yet implimented\n");               \
        }else{                                                          \
            /* do nothing */                                            \
            printf("Can do nothing since fixed\n");                     \
        }                                                               \
    }                                                                   \
}while(0)

#define RB_DEQUEUE(a,ref)                                               \
do{                                                                     \
    if(!(RB_EMPTY(a))){                                                 \
        (*ref) = (a)->data[RB_HEAD(a)];                                 \
        (a)->impl.head = ((a)->impl.head + 1) % ((a)->impl.capacity);   \
        --RB_COUNT(a);                                                  \
    }else{                                                              \
        printf("Cant do anything, rb is empty\n");                      \
    }                                                                   \
}while(0)

#define RB_DEQUEUE_NULL(a)                                              \
do{                                                                     \
    if(!(RB_EMPTY(a))){                                                 \
        (a)->impl.head = ((a)->impl.head + 1) % ((a)->impl.capacity);   \
        --RB_COUNT(a);                                                  \
    }else{                                                              \
        printf("Cant do anything, rb is empty\n");                      \
    }                                                                   \
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
// initilizes the "struct rb_impl" part of the ring buffer that defines:len,const cap,data...
void rb_init_impl(struct rb_impl* impl,size_t elem_size,size_t capacity);
// filesystem functionality
char* ouroc_read_all_file(const char* filepath);
bool ouroc_file_exists(const char* filepath);
bool ouroc_dir_exists(const char* filepath);
bool ouroc_touch_file(const char* filepath);
bool ouroc_make_dir(const char* filepath);
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
    -> [...] More features
    -> [...] More options for functionality/usage
    -> Nested dependencies management
    -> [DONE] Better Async/sync builds
    -> [ALRIGHT] Better logging messages
    -> Color in logging
    -> [DONE] Thread safe printing to stdout (annoying)
    -> controlled building

------------------------------------------------------------------------------
*/

enum LogType {
    OUROC_INFO,
    OUROC_WARN,
    OUROC_ERROR
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

#ifdef _WIN32
    static CRITICAL_SECTION stdout_lock;
    static int stdout_lock_initialized = 0; // not locked
    static void init_stdout_lock() {
        if (!stdout_lock_initialized) {
            InitializeCriticalSection(&stdout_lock);
            stdout_lock_initialized = 1;
        }
    }
#else
    static pthread_mutex_t stdout_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

#ifdef _WIN32
    typedef HANDLE ouroc_proc;
#else
    typedef pthread_t ouroc_proc;
#endif

struct ouroc_pool {
    RB_TEMPLATE(ouroc_proc) procs;
};

#define OUROC_POOL(name,max_threads) struct ouroc_pool name; RB_INIT(&name.procs,(max_threads),FIXED)
#define OUROC_POOL_APPEND(master,proc) RB_ENQUEUE(&(master)->procs,(proc))


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
struct stat ouroc_get_file_state(const char* filename);
// addes pointers to strings to build "stream" to be executed
void ouroc_append_stream_many(struct ouroc*master,const unsigned int count,...);
// runs the command after it has been assembled from "stream"
void ouroc_run_cmd(struct ouroc*master);
// inititilizes the "target" and its file "dependencies" 
void ouroc_init_many(struct ouroc*master,char* target,const size_t count,...);

/* async function delcaration */

#ifdef _WIN32
    typedef DWORD ouroc_proc_ret;
    #define OUROC_CALLCONV WINAPI
#else
    typedef void* ouroc_proc_ret;
    #define OUROC_CALLCONV 
#endif




#ifdef _WIN32
    #define OUROC_CREATE_PROC(proc,porter,arg) \
            (proc) = CreateThread(NULL,0,(porter),(arg),0,NULL)

    #define OUROC_WAIT_PROC(proc)               \
        do{                                     \
            WaitForSingleObject(proc,INFINITE); \
            CloseHandle(proc);                  \
        }while(0)
#else
    #define OUROC_CREATE_PROC(proc,porter,arg) \
        pthread_create(&(proc),NULL,(porter),(arg));
    
    #define OUROC_WAIT_PROC(proc) \
        pthread_join(proc,NULL)
#endif

// man-in_middle function that pthread/win api uses , it calls "ouroc_run_cmd"
ouroc_proc_ret OUROC_CALLCONV ouroc_build_thread_porter(void*arg);
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

void rb_init_impl(struct rb_impl* impl,size_t elem_size,size_t capacity){
    impl->count = 0;
    impl->head = impl->tail = 0;
    impl->elem_size = elem_size;
    impl->capacity = capacity;
}

char* ouroc_read_all_file(const char* filepath){
    char* file_content = NULL;
    FILE* fp = fopen(filepath,"rb");
    if(fp == NULL) goto defer;
    long file_size;
    fseek(fp,0,SEEK_END);
    file_size = ftell(fp);
    fseek(fp,0,0);
    if(file_size < 0) goto defer;
    file_content = malloc((size_t)file_size+1);
    if(file_content == NULL) goto defer;
    if(fread(file_content,1,file_size,fp) != (size_t)file_size) goto defer;
    file_content[file_size] = '\0';
    fclose(fp);
    return file_content;
defer:
    if(fp != NULL) fclose(fp);
    if(file_content != NULL) free(file_content);
    return NULL;
}
bool ouroc_file_exists(const char* filepath){
    #ifdef _WIN32
        return _access(filepath,F_OK) == 0;
    #else
        return access(filepath,F_OK) == 0;
    
    #endif
}
bool ouroc_dir_exists(const char* filepath){
    return ouroc_file_exists(filepath);
}
bool ouroc_touch_file(const char* filepath){
    FILE*fp = fopen(filepath,"w");
    if(fp == NULL) goto defer;
    return true;
defer:
    if(fp != NULL) fclose(fp);
    return false;
}
bool ouroc_make_dir(const char* filepath){
    #ifdef _WIN32
        return CreateDirectoryA(filepath, NULL) || GetLastError() == ERROR_ALREADY_EXISTS;    
    #else
        return mkdir(filepath, 0755) == 0 || errno == EEXIST;
    #endif
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
        case OUROC_INFO:
            printf("[OUROC_INFO]: ");
            vprintf(fmt, args);
            printf("\n");
            break;
        case OUROC_WARN:
            printf("[OUROC_WARN]: ");
            vprintf(fmt, args);
            printf("\n");
            break;
        case OUROC_ERROR:
            printf("[OUROC_ERROR]: ");
            vprintf(fmt, args);
            printf("\n");
            break;
    }

    va_end(args);
}

struct stat ouroc_get_file_state(const char* filename){
    struct stat info;
    if(stat(filename,&info) != 0){
        switch(errno){
            case ENOENT:
                ouroc_log(OUROC_ERROR,"File \"%s\" does not exist.",filename);
                break;
            case EACCES:
                ouroc_log(OUROC_ERROR,"Dont have permission to get stat for \"%s\".",filename);
                break;
            case ENOTDIR:
                ouroc_log(OUROC_ERROR,"Component in the path to \"%s\" isn’t a directory.",filename);
                break;
            default:
                ouroc_log(OUROC_ERROR,"Could not get stat for file \"%s\".",filename);
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
    if(!ouroc_file_exists(master->target)) goto rebuild_target;
    struct stat target_info = ouroc_get_file_state(master->target);
    /* Check for changed dependency files */
    for(size_t i = 0 ; i < DA_LEN(&master->depend) ; ++i){
        char* file = master->depend.data[i];
        struct stat dep_info = ouroc_get_file_state(file);
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
    if(master->target == NULL) ouroc_log(OUROC_INFO,"executing.");
    else ouroc_log(OUROC_INFO,"Building \"%s\".",master->target);
    int ret = system(command.data);
    if(ret != 0){
        ouroc_log(OUROC_ERROR,"Failed building \"%s\".",master->target);
        exit(1);
    }
    if(master->target == NULL) ouroc_log(OUROC_INFO,"execution done.");
    else ouroc_log(OUROC_INFO,"\"%s\" done.",master->target);
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

ouroc_proc_ret OUROC_CALLCONV ouroc_build_thread_porter(void*arg){
    struct ouroc* obj = arg;
    // Here were locking stdout before running the command 
    // since ouroc logs the build proccess and its quite annoying
    // to see many threads printing at the same time
#ifdef _WIN32
    init_stdout_lock();
    EnterCriticalSection(&stdout_lock);
    ouroc_run_cmd(obj);
    fflush(stdout);
    LeaveCriticalSection(&stdout_lock);
    return 0;
#else
    pthread_mutex_lock(&stdout_lock);
    ouroc_run_cmd(obj);
    fflush(stdout);
    pthread_mutex_unlock(&stdout_lock);
    return NULL;

#endif
}

void ouroc_pool_run_async_single(struct ouroc_pool* master,struct ouroc* value){
    ouroc_proc proc;
    OUROC_CREATE_PROC(proc,ouroc_build_thread_porter,value);
    // if the queue is full then we wait for the first queued proccess
    if(RB_FULL(&master->procs)){
        OUROC_WAIT_PROC(RB_BACK(&master->procs));
        RB_DEQUEUE_NULL(&master->procs);
    }
    OUROC_POOL_APPEND(master,proc);
}

void ouroc_pool_wait_all(struct ouroc_pool* master){
    while(!RB_EMPTY(&master->procs)){
        OUROC_WAIT_PROC(RB_BACK(&master->procs));
        RB_DEQUEUE_NULL(&master->procs);
    }
}

#endif
