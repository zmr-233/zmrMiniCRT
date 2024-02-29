//malloc_.c

#include "minicrt.h"

typedef struct _heap_header
{
	enum{
		HEAP_BLOCK_FREE	= 0XABABABAB,//空闲块的魔数
		HEAP_BLOCK_USED	= 0XCDCDCDCD //占用块的魔数
	}type;
	unsigned long size; //size包括块meta头
	struct  _heap_header* next;
	struct  _heap_header* prev; 
}heap_header;

#define ADDR_ADD(a,o) (((char*)(a)) + o)
#define HEADER_SIZE	(sizeof(heap_header))

static heap_header* list_head	= NULL;		//static？

void free_(void* ptr){
	heap_header* header	= (heap_header*)ADDR_ADD(ptr,-HEADER_SIZE);
	
	if (header->type != HEAP_BLOCK_USED)
		return;

	header->type	= HEAP_BLOCK_FREE;
	if (header->prev != NULL && header->prev->type == HEAP_BLOCK_FREE){
		//MERGE--释放块的前一个块也正好为空
		header->prev->next = header->next;
		if(header->next != NULL)
			header->next->prev = header->prev;
		header->prev->size += header->size;

		header	= header->prev;
	}

	if (header->next != NULL && header->next->type == HEAP_BLOCK_FREE){
		//MERGE--释放块的后一个块也是空块
		header->size += header->next->size;
		header->next = header->next->next;
	}
}

void* malloc_(unsigned long size){
	heap_header *header;
	if (size == 0)
		return NULL;
	header	= list_head;

	while(header != 0){
		if (header->type == HEAP_BLOCK_USED){
			header = header->next;
			continue;
		}
		
		if ((header->size > size +HEADER_SIZE)
			&&(header->size <= size + HEADER_SIZE *2)){
			header->type = HEAP_BLOCK_USED;
		}

		//空闲块空间足够，且剩余的内部碎片分离出来还可以再使用
		if (header->size > size + HEADER_SIZE*2){
			//split
			heap_header* next = (heap_header*)ADDR_ADD(header,size + HEADER_SIZE);
			next->prev	= header;
			next->next	= header->next;
			next->type	= HEAP_BLOCK_FREE;
			next->size	= header->size -(size - HEADER_SIZE);
			header->next	= next;
			header->size	= size +HEADER_SIZE;
			header->type	= HEAP_BLOCK_USED;
			return	ADDR_ADD(header,HEADER_SIZE);
		}
		header = header->next;
	}

	return NULL;
}



#ifndef WIN32

//brk设置.data边界：注意--
static long brk(void* end_data_segment)
{
	long ret	= 0;
	//brk system call number:45
	//long //usr/include/asm-i386/unistd.h
	//#define __NR_brk 45
#ifdef USEOLDCALL
	asm( "movq $45, %%rax   \n\t"
		 "movq %1, %%rbx  \n\t"
		 "int $0x80		\n\t"
		 "movq %%rax,%0  \n\t"
		 : "=r"(ret):"m"(end_data_segment));
#else
	asm("syscall"
        : "=a"(ret)
        : "a"(__NR_brk), "D"(end_data_segment) 
        : "rcx", "r11", "memory"); //rcx，r11会被brk使用
    return ret;
#endif
}
#endif

#ifdef WIN32
#include <Windows.h>
#endif

long mini_crt_init_heap()
{
	void *base	= NULL;
	heap_header *header	= NULL;
	//32MB heap size
	unsigned long heap_size	= 1024*1024*32; //以base为起点分配32MB的内存空间
	//glibc-2.39/sysdeps/mach/hurd/brk.c的brk只申请128MB

#ifdef WIN32
	base = VirtualAlloc(0,heap_size,MEM_COMMIT | MEM_RESERVE,PAGE_READWRITE);
	if (base == NULL)
		return 0;

#else
	base	= (void*)brk(0); //获取当前程序断点位置而不改变它的方法
	void* end = ADDR_ADD(base,heap_size);
	end		= (void*)brk(end); 
	if (!end)
		return 0;
#endif

	header	= (heap_header*)base;

	header->size	= heap_size;
	header->type	= HEAP_BLOCK_FREE;
	header->next	= NULL;
	header->prev	= NULL;

	list_head		= header;
	return 1;
}
