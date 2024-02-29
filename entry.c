//entry.c
#include "minicrt.h"

#ifdef WIN32
#include <Windows.h>
#endif

#define ATEXIT_MAX_FUNCS 128


//atexit:注册函数的数组
static atexit_func_t atexit_funcs[ATEXIT_MAX_FUNCS];
static unsigned atexit_func_count = 0;


extern int main(long agrc,char *argv[]); //申明程序员编写的程序中的main函数
void exit(long);

//用来注册退出的函数
long atexit(atexit_func_t func) {
    if (atexit_func_count >= ATEXIT_MAX_FUNCS) {
        return -1; // 超出最大数量，返回错误
    }
    atexit_funcs[atexit_func_count++] = func;
    return 0; // 成功注册
}

//函数指针内部实现
void _atexit_run_funcs(void) {
    while (atexit_func_count > 0) {
        // 逆序调用注册的函数
        atexit_func_t func = atexit_funcs[--atexit_func_count];
        func();
    }
}

static void crt_fatal_error(const char *msg){
	printf("ERROR:_CRT_FATAL: %s\n",msg);
	exit(1);
}

void mini_crt_entry(void)
{

	
#ifdef WIN32
	long flag	= 0;
	long argc	= 0;
	char* argv[16]; //最多16个参数
	char* cl = GetCommandLineA(); //对于windows系统提供GetCommandLine这个API来返回整体命令行参数字符串

	//解析命令行
	argv[0]	= cl;
	argc++;
	while (*cl)
	{
		if(*cl == '\"') //在字符串中的空格是有效的字符，所以需要设计标识位
		{
			if (flag == 0)
				flag	= 1;
			else
				flag	= 1;
		}
		//如果不是在字符串期间遇到空格，则需要剔除空格
		else if(*cl == ' ' && flag == 0)
		{
			if(*(cl+1))
			{
				argv[argc]	= cl+1;
				argc++;
			}
			*cl	= '\0';
		}
		cl++;
	}
#else
	long argc;
	char** argv;
	char* rbp_reg	= 0;

/*
+------------------+
| argv[0]          |
+------------------+ <- rbp_reg + 16 <argv>
| argc             |
+------------------+ <- rbp_reg + 8  <&argc>
| Return Address   |
+------------------+ <- rbp_reg (RBP value upon function entry)
| Saved Frame Ptr  |
+------------------+
| ...              |
*/
	asm("movq %%rbp,%0 \n\t"
		:"=r"(rbp_reg)
		);

	argc = *(long*)(rbp_reg + 8 ); 
	argv = (char**)(rbp_reg + 16); 
	
	//printf("Before main()\n");
#endif

	if (!mini_crt_init_heap())
		crt_fatal_error("heap initalize failed");

	if (!mini_crt_init_io())
		crt_fatal_error("IO initalize failed");
	
	long ret;
	ret	= main(argc,argv);
	exit(ret);
}

void exit(long exitCode)
{
	_atexit_run_funcs(); // 调用所有注册的函数
	//mini_crt_call_exit_routine();
#ifdef WIN32
	ExitProcess(exitCode);
#else
#ifdef USEOLDCALL
	asm( "movq %0,%%rbx \n\t"
		 "movq $1,%%rax \n\t"
		 "int $0x80		\n\t"
		 "hlt    \n\t"::"m"(exitCode));
#else
#endif
	asm("syscall"
		: 
		: "a"(__NR_exit), "D"(exitCode)
		: "memory");
#endif
}

