#include "minicrt.h"
#define USE1E

#ifdef USE1E
int main(int argc, char* argv[]) {
    printf("Hello world\n");

    char* str = (char*)malloc_(100);
    strcpy(str, "Hello, miniCRT!");
    printf("%s\n", str);
    free_(str);

    char number[32];
    itoa_(123456, number, 10);
    printf("The number is: %s, length: %lu\n", number, strlen_(number));

    if (strcmp(number, "123456") == 0)
        printf("strcmp works!\n");

    FILE* fp;
#define FWRITE
#ifdef FWRITE
    fp = fopen("./test.txt", "w");
    if (fp != NULL) {
        fwrite("Hello, File!\n", 1, 13, fp);
        fclose(fp);
    }
    else {
        printf("FILE* ERROR!\n");
    }
#endif

#define CONTINUE_DO
#ifdef CONTINUE_DO
    fp = fopen("./test.txt", "r");
    if (fp != NULL) {
        char buffer[100] = { 0 }; // Initialize buffer to zero
        fread(buffer, 1, 13, fp);
        buffer[12] = '\0'; // Ensure null-termination
        printf("File content: %s\n", buffer);
        fclose(fp);
    }
    else {
        printf("FILE* ERROR!\n");
    }

    //
    fp = fopen("./test.txt", "r+");
    if (fp) {
        fseek(fp, 0, 0); // Move to the beginning of the file
        fwrite("MiniCRT ", 1, 8, fp);
        fclose(fp);
    }
    else {
        printf("FILE* ERROR!\n");
    }

    //
    fp = fopen("./test.txt", "a");
    if (fp) {
        fputc('\n', fp);
        fputs("This is a test of fputs", fp);
        fclose(fp);
    }
    else {
        printf("FILE* ERROR!\n");
    }

    //
    printf("This is a test of printf\n");
    fp = fopen("./test.txt", "a");
    if (fp) {
        fprintf(fp, "This is a test of fprintf.\n");
        fclose(fp);
    }
    else {
        printf("FILE* ERROR!\n");
    }
#endif
    return 0;
}
#else
void fun(){
    printf("After main\n");
}
int main(int argc, char* argv[]) {
   atexit(fun);
   printf("Main\n"); 
   return 0;
}
#endif
