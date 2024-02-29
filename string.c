//string.c
/*
纯粹的用户态的计算,实现较为简单
*/
#include "minicrt.h"
char* itoa_(long n,char* str,long radix){
	char digit[]	="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char* p	= str;
	char* head = str;
	if (!p || radix < 2 || radix > 36) //radix代表是几进制
		return p;

	if (radix != 10 && n < 0)
		return p;

	if (n == 0){ //如果要转换的数字n为0，则直接在输出字符串中直接输出
		*p++	= '0';
		*p		= 0;
		return p;
	}

	if (radix == 10 && n < 0){ //如果是10进制，且为负数，则先添加负号，然后转正留待后续处理
		*p++	= '-';
		n		= -n;
	}

	while (n){
		*p++	= digit[n%radix];
		n	/= radix;
	}

	*p = 0; //数字转换完了，末尾添加0
	
	for (--p; head < p; ++head,--p){ //上面的数字字符串是倒序的，这里将数字字符串倒过来
		char temp	= *head;
		*head		= *p;
		*p			= temp;
	}

	return str;
}

char* utoa_(unsigned long n, char* str, long radix) {
	char digit[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char* p = str;
	char* head = str;

	if (!p || radix < 2 || radix > 36)
		return p;

	if (n == 0) {
		*p++ = '0';
		*p = 0;
		return p;
	}

	while (n) {
		*p++ = digit[n % radix];
		n /= radix;
	}

	*p = 0;

	for (--p; head < p; ++head, --p) {
		char temp = *head;
		*head = *p;
		*p = temp;
	}

	return str;
}

long strcmp(const char* src, const char* dst){
	long ret	= 0;
	unsigned char* p1	= (unsigned char*)src;
	unsigned char* p2	= (unsigned char*)dst;

	while(!(ret = *p1 - *p2)&&*p2)
		++p1,++p2;
	if (ret < 0)
		ret	= -1;
	else if (ret > 0)
		ret	= 1;

	return (ret);
}

char * strcpy(char* dest, const char* src){
	char* ret	= dest;
	while(*src)
		*dest++ = *src++;
	*dest	= '\0'; //字符串拷贝完后，手动在末尾添加\0
	return ret; //返回copy后的字符串的首
}

unsigned long strlen_(const char* str){
	long cnt	= 0;
	if(!str)
		return 0;
	for (;*str != '\0';++str)
		++cnt;  //printf("inside the for-loop %d\n", cnt);

	return cnt;
}