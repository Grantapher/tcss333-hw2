#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** args) {

    if(argc != 2) {
        printf("Format: ./granttoe_hw2_runner <input .bmp>\n");       
        exit(10);
    }

    char str[256];
    int returnCode;

    sprintf(str, "./granttoe_hw2 shrink \"%s\" small.bmp", args[1]);
    if(0 != (returnCode = system(str))) exit(returnCode);

    sprintf(str, "./granttoe_hw2 expand \"%s\" big.bmp", args[1]);
    if(0 != (returnCode = system(str))) exit(returnCode);

    printf("Both files created successfully!\n");
    return 0;
}
