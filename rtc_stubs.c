// rtc_stubs.c
#include"minicrt.h"
#ifdef WIN32

void _RTC_CheckStackVars(void* esp, void* ebp) {
    //用于检查栈变量
}

void _RTC_InitBase(void) {
    //用于初始化运行时检查
}

void _RTC_Shutdown(void) {
    //用于关闭运行时检查
}
#endif