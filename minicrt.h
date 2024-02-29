#ifndef __MINI_CRT_H__
#define __MINI_CRT_H__

#ifdef __cplusplus
extern "C" {
#endif


#ifndef WIN32
//系统调用号
#include<asm/unistd.h>
//#include<bits/fcntl.h> 会报错
#endif

//malloc_ 
#ifndef NULL
#define NULL (0)
#endif

void free_(void* ptr);
void* malloc_(unsigned long size);
static long brk(void *end_data_segment);
long mini_crt_init_heap();

//字符串
char* itoa_(long n,char* str,long radix);
char* utoa_(unsigned long n, char* str, long radix);

long strcmp(const char* src, const char* dst);
char* strcpy(char* dest, const char* src);
unsigned long strlen_(const char* str);

//文件与IO----注意：这是文件描述符，并不是FILE结构
typedef long FILE;

#define EOF	(-1)

#ifdef WIN32
//Win的标准输入输出必须用GetStdHandle的API获得
#define stdin	((FILE*)(GetStdHandle(STD_INPUT_HANDLE))) 
#define stdout	((FILE*)(GetStdHandle(STD_OUTPUT_HANDLE)))
#define stderr	((FILE*)(GetStdHandle(STD_ERROR_HANDLE)))
#else

#define stdin	((FILE*)0)
#define stdout	((FILE*)1)
#define stderr	((FILE*)2)


#endif

long write(long fd,const void *buffer,unsigned long size);

long mini_crt_init_io();
FILE* fopen(const char *filename,const char* mode);
long fread(void* buffer,long size,long count,FILE* stream);
long fwrite(const void* buffer,long size,long count,FILE* stream);
long fclose(FILE *fp);
long fseek(FILE* fp,long offset,long set);

//printf
long fputc(char c,FILE* stream);
long fputs(const char* str,FILE *stream);
long printf(const char *format,...);
long fprintf(FILE* stream,const char *format,...);

//internal 
void do_global_ctors();
void mini_crt_call_exit_routine();

//atexit
typedef void(*atexit_func_t )( void );
long atexit(atexit_func_t func);


#ifdef WIN32
//空实现 _RTC_CheckStackVars
void _RTC_CheckStackVars(void* esp, void* ebp);
//空实现 _RTC_InitBase
void _RTC_InitBase(void);
//空实现 _RTC_Shutdown
void _RTC_Shutdown(void);
#endif

#ifdef __cplusplus
}
#endif


#endif // end __MINI_CRT_H__

