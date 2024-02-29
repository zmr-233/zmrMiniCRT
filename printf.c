#include "minicrt.h"

static char __fputc_tmp_val__ = 0;
long fputc(char c,FILE* stream){
	__fputc_tmp_val__ = c;
	if (fwrite(&__fputc_tmp_val__,1,1,stream) != 1)
		return EOF;
	else
		return c;
}

static char __fputs_tmp_array__[256] = {0};
static int __fputs_tmp_size__ = 256;
long fputs(const char* str,FILE *stream){
	long len = strlen_(str);
	if( len >= __fputs_tmp_size__ )
		return EOF;
	strcpy( __fputs_tmp_array__,str );
	if (fwrite(__fputs_tmp_array__,1,len,stream) != len)
		return EOF;
	else
		return len;
}

#ifndef WIN32
#define va_list char*
#define va_start(ap,arg) (ap=(va_list)&arg+sizeof(arg))
#define va_arg(ap,t)	(*(t*)((ap+=sizeof(t)) - sizeof(t)))
#define va_end(ap) (ap=(va_list)0)
#else
#include <Windows.h>
#endif

/**
 * 1.不支持格式控制
*/
static long vfprintf(FILE* stream, const char *format, va_list arglist)
{
	long translating	= 0;
	long ret	= 0; //记录最终输出的字符个数
	const	char* p	=0;

//  fputs("***********entry the Vprintf*********\n",stream);
	for (p = format;*p != '\0';++p){
		switch (*p){
		case '%':
			if (!translating)
				translating	= 1; //translating置为1，代表后面的字符需要解析
			else{
				if (fputc('%',stream) < 0)
					return EOF;
				++ret;
				translating = 0;
			}
			break;

		case 'd':
			if (translating){

				char buf[16];
				translating	= 0;
				itoa_(va_arg(arglist,long),buf,10);
				if (fputs(buf,stream) < 0)
					return EOF;
				ret+= strlen_(buf);
			} 
			else if (fputc('d',stream) < 0)
				return EOF;
			else
				++ret;
			break;

		case 's':
			if (translating){
				const char *str	= va_arg(arglist,const char*);

				translating	= 0;
				if (fputs(str,stream) < 0)
					return EOF;
				ret +=strlen_(str);
			}
			else if (fputc('s',stream) < 0)
				return EOF;
			else
				++ret;

			break;
		
		case 'l':
			if (translating) {
				p++; //移动到u
				if (*p == 'u') {
					char buf[32]; //足够存储无符号长整数
					translating = 0;
					utoa_(va_arg(arglist, unsigned long), buf, 10);
					if (fputs(buf, stream) < 0)
						return EOF;
					ret += strlen_(buf);
				}
				else {
					//如果不是 %lu，则输出l并将格式指针回退一个字符
					if (fputc('l', stream) < 0)
						return EOF;
					--p; //回退，因为我们需要重新处理当前字符
					++ret;
				}
			}
			else if (fputc('l', stream) < 0)
				return EOF;
			else
				++ret;
			break;
		
		default:
			if (translating)
				translating	= 0;
			if (fputc(*p,stream) <0)
				return EOF;
			else
				++ret;

			break;
		}
	}
	return ret;
}
#ifdef WIN32
long printf(const char* format, ...){
	//fputs("entry the printf*************\n", stdout);
	va_list(arglist);
	va_start(arglist, format);
	return vfprintf(stdout, format, arglist);
}

long fprintf(FILE* stream, const char* format, ...){
	va_list(arglist);
	va_start(arglist, format);
	return vfprintf(stream, format, arglist);
}
#else
long printf(const char *format,...){
	char* arglist;
	asm( "movq %%rbp,%0":"=r"(arglist) );
	arglist -= 0xa8;
	return vfprintf(stdout,format,arglist);
}

long fprintf(FILE* stream,const char *format,...){
	char* arglist;
	asm( "movq %%rbp,%0":"=r"(arglist) );
	arglist -= 0xa8;
	return vfprintf(stream,format,arglist);
}
#endif
