#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** args) {

    if(argc != 2) exit(1);
    char str[256];
    
    sprintf(str, "./granttoe_hw2 shrink %s small.bmp", args[1]);
    system(str);

    sprintf(str, "./granttoe_hw2 expand %s big.bmp", args[1]);
    system(str);

    return 0;
}
