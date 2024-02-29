//stdio.c
/*
现有问题:
1.没有buffer机制
2.\r\n与\n不进行转换
*/
#include "minicrt.h"

long mini_crt_init_io(){
	return	1;
}

#ifdef WIN32
#include <Windows.h>

#ifdef USE_OLD
FILE* fopen(const char *filename,const char* mode){
	HANDLE hFile	= 0;
	long access		= 0;
	long creation	= 0;
	if (strcmp(mode,"w") == 0){
		access		|= GENERIC_WRITE;
		creation	|= CREATE_ALWAYS;
	}

	if (strcmp(mode,"w+") == 0){
		access		|= GENERIC_WRITE |GENERIC_READ;
		creation	|= CREATE_ALWAYS;
	}

	if (strcmp(mode,"r") == 0){
		access	|= GENERIC_READ;
		creation	= CREATE_ALWAYS;
	}

	if (strcmp(mode,"r+") == 0){
		access	|= GENERIC_WRITE | GENERIC_READ;
		creation	|= TRUNCATE_EXISTING;
	}

	hFile	= CreateFileA(filename,access,0,0,creation,0,0);
	if (hFile == INVALID_HANDLE_VALUE){
		return 0;
	}

	return (FILE*)hFile;
}
#else
FILE* fopen(const char* filename, const char* mode) {
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD access = 0;
	DWORD creation = 0;

	if (strcmp(mode, "r") == 0) {
		access = GENERIC_READ;
		creation = OPEN_EXISTING;
	}
	else if (strcmp(mode, "w") == 0) {
		access = GENERIC_WRITE;
		creation = CREATE_ALWAYS;
	}
	else if (strcmp(mode, "a") == 0) {
		access = FILE_APPEND_DATA;
		creation = OPEN_ALWAYS;
	}
	else if (strcmp(mode, "r+") == 0) {
		access = GENERIC_READ | GENERIC_WRITE;
		creation = OPEN_EXISTING;
	}
	else if (strcmp(mode, "w+") == 0) {
		access = GENERIC_READ | GENERIC_WRITE;
		creation = CREATE_ALWAYS;
	}
	else if (strcmp(mode, "a+") == 0) {
		access = GENERIC_READ | FILE_APPEND_DATA;
		creation = OPEN_ALWAYS;
	}
	else if (strcmp(mode, "x") == 0) {
		access = GENERIC_WRITE;
		creation = CREATE_NEW;
	}
	else if (strcmp(mode, "x+") == 0) {
		access = GENERIC_READ | GENERIC_WRITE;
		creation = CREATE_NEW;
	}
	else if(strcmp(mode, "a") != NULL) {
		// Move to the end of the file for append mode
		SetFilePointer(hFile, 0, NULL, FILE_END);
	}
	else {
		// Unsupported mode
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	hFile = CreateFileA(filename, access, 0, NULL, creation, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		// Error handling
		return NULL;
	}

	return (FILE*)hFile;
}
#endif

long fread(void* buffer,long size,long count,FILE* stream){
	long read	= 0;
	if (!ReadFile((HANDLE)stream,buffer,size*count,&read,0)){
		return 0;
	}

	return read;
}

long fwrite(const void* buffer,long size,long count,FILE* stream){
	long written	= 0;
	if (!WriteFile((HANDLE)stream,buffer,size*count,&written,0)){
		return 0;
	}
	return written;
}

long fclose(FILE *fp){
	return CloseHandle((HANDLE)fp);
}

long fseek(FILE* fp,long offset,long set){
	return SetFilePointer((HANDLE)fp,offset,0,set);
}

#else //define linux

#define AT_FDCWD -100

//#define SEEK_SET 0  // 从文件开头开始
//#define SEEK_CUR 1  // 从当前文件位置开始
//#define SEEK_END 2  // 从文件末尾开始

static long open(const char* filename,long flags,long mode){
	long fd	= 0;
	#ifdef USEOLDCALL
	asm("movq $5,%%rax		\n\t"
		"movq %1,%%rbx	\n\t"
		"movq %2,%%rcx \n\t"
		"movq %3,%%rdx \n\t"
		"int $0x80  \n\t"
		"movq %%rax,%0 \n\t":
		"=m"(fd):"m"(filename),"m"(flags),"m"(mode));
	#else
        register long r10 asm("r10") = mode; // 将mode移动到r10寄存器
        asm("syscall"
            : "=a" (fd)
            : "0" (__NR_openat), "D" (AT_FDCWD), "S" (filename), "d" (flags), "r" (r10)
            : "rcx", "r11", "memory");
	#endif

		return fd;
}

static long read(long fd,void* buffer,unsigned long size){
	long ret	= 0;
	#ifdef USEOLDCALL
	asm("movq $3,%%rax \n\t"
		"movq %1,%%rbx \n\t"
		"movq %2,%%rcx \n\t"
		"movq %3, %%rdx \n\t"
		"int $0x80  \n\t"
		"movq %%rax,%0  \n\t":
	"=m"(ret):"m"(fd),"m"(buffer),"m"(size));
	#else
		register unsigned long r10 asm("r10") = size;
		asm("syscall"
			: "=a" (ret)
			: "0" (__NR_read), "D" (fd), "S" (buffer), "d" (r10)
			: "rcx", "r11", "memory");
	#endif

	return ret;
}

long write(long fd,const void *buffer,unsigned long size){
	long ret	= 0;
	#ifdef USEOLDCALL
	asm("movq $4,%%rax  \n\t"
		"movq %1,%%rbx \n\t"
		"movq %2,%%rcx  \n\t"
		"movq %3,%%rdx \n\t"
		"int $0x80  \n\t"
		"movq %%rax,%0 \n\t":
	"=m"(ret):"m"(fd),"m"(buffer),"m"(size));
	#else
		register unsigned long r10 asm("r10") = size;
		asm("syscall"
			: "=a" (ret)
			: "0" (__NR_write), "D" (fd), "S" (buffer), "d" (r10)
			: "rcx", "r11", "memory");
	#endif


	return ret;
}

static long close(long fd){
	long ret	= 0;
	#ifdef USEOLDCALL
	asm("movq $6,%%rax  \n\t"
		"movq %1,%%rbx  \n\t"
		"int $0x80  \n\t"
		"movq %%rax,%0 \n\t":
	"=m"(ret):"m"(fd));
	#else
		asm("syscall"
			: "=a" (ret)
			: "0" (__NR_close), "D" (fd)
			: "rcx", "r11", "memory");
	#endif


	return	ret;
}

static long seek(long fd,long offset,long mode){
	long ret	= 0;
	#ifdef USEOLDCALL
	asm("movq $19,%%rax  \n\t"
		"movq %1,%%rbx \n\t"
		"movq %2,%%rcx  \n\t"
		"movq %3,%%rdx \n\t"
		"int $0x80  \n\t"
		"movq %%rax,%0 \n\t":
	"=m"(ret):"m"(fd),"m"(offset),"m"(mode));
	#else
		register long r10 asm("r10") = mode;
		asm("syscall"
			: "=a" (ret)
			: "0" (__NR_lseek), "D" (fd), "S" (offset), "d" (r10)
			: "rcx", "r11", "memory");
	#endif


	return ret;
}

//<bits/fcntl.h>头文件中
#define O_RDONLY	00
#define O_WRONLY	01
#define O_RDWR		02
#define O_CREAT		0100
#define O_TRUNC		01000
#define O_APPEND	02000
#define O_EXCL   	0200

#ifdef USEOLD
FILE* fopen(const char *filename,const char* mode){
	long fd	= -1;
	long flags	= 0;
	long access	= 00700; //设置权限700

	if (strcmp(mode,"w") == 0)
		flags	|= O_WRONLY | O_CREAT |O_TRUNC;

	if (strcmp(mode,"w+") == 0)
		flags	|= O_RDWR |O_CREAT|O_TRUNC;

	if (strcmp(mode,"r") == 0)
		flags	|= O_RDONLY;

	if (strcmp(mode,"r+") == 0)
		flags	|= O_RDWR | O_CREAT;

	fd =open(filename,flags,access);
	return (FILE*)fd;
}
#else
//#include<sys/types.h>
//#include<fcntl.h>

FILE* fopen(const char *filename, const char* mode) {
    int fd = -1;
    int flags = 0;

	//typedef unsigned int mode_t;
    int access = 0600; // 设置文件的权限

    if (strcmp(mode, "r") == 0)
        flags |= O_RDONLY;
    else if (strcmp(mode, "w") == 0) 
        flags |= O_WRONLY | O_CREAT | O_TRUNC;
    else if (strcmp(mode, "a") == 0)
        flags |= O_WRONLY | O_CREAT | O_APPEND;
    else if (strcmp(mode, "r+") == 0)
        flags |= O_RDWR;
    else if (strcmp(mode, "w+") == 0)
        flags |= O_RDWR | O_CREAT | O_TRUNC;
    else if (strcmp(mode, "a+") == 0)
        flags |= O_RDWR | O_CREAT | O_APPEND;
    else if (strcmp(mode, "x") == 0)
        flags |= O_WRONLY | O_CREAT | O_EXCL;
    else if (strcmp(mode, "x+") == 0)
        flags |= O_RDWR | O_CREAT | O_EXCL;
    else 
        return NULL;

    fd = open(filename, flags, access);
    if (fd == -1)
        return NULL;

    return (FILE*)fd;
}
#endif

long fread(void* buffer,long size,long count,FILE* stream){
	return read((long)stream,buffer,size*count);
}
long fwrite(const void* buffer,long size,long count,FILE* stream){
	return write((long)stream,buffer,size*count);
}

long fclose(FILE *fp){
	return close((long)fp);
}

long fseek(FILE* fp,long offset,long set){
	return seek((long)fp,offset,set);
}

#endif

