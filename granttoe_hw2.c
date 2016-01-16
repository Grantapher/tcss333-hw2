#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void fail(char* message, int code) { 
    printf("%s\n", message);
    exit(code);
}

void formatFail() {
    fail("Format: ./granttoe_hw2 (shrink|expand) <bmp file>\n", 2);
}

void printHex(unsigned char* str, unsigned char size) {
    int i;
    for(i = 0; i < size; i++) {
        printf("%02x ", *(str + i));
    }
    printf("\n");
}

unsigned int swapEndianInt(unsigned int little) {
    return (little >> 24)
         | ((little >> 8) && 0x0000ff00)
         | ((little << 8) && 0x00ff0000)
         | (little << 24);
}

unsigned int swapEndianShort(unsigned short little) {
    return (little >> 8) 
         | (little << 8);
}

void printImageHex(unsigned char* image, unsigned int width, unsigned int height) {
    int i, j, k;
    for(i = height - 1; i >= 0; i--) {
        for(j = 0; j < width * 3; j += 3) {
            for(k = 2; k >= 0; k--) {
                printf("%02x", image [(i * width * 3) + j + k]);
            }
            printf(" ");
        }
        printf("\n");
    }
    printf("\n\n");
}

int main(int argc, char** args) {
    if(argc != 3) formatFail();

    int shrink; 
    if(!strcmp("shrink", args[1])) {
        shrink = 1;
    } else if (!strcmp("expand", args[1])) {
        shrink = 0;
    } else {
        formatFail();
    }

    FILE *in, *out;
    in = fopen(args[2], "r");
    if (NULL == in) fail("Bad .bmp file!", 3);
    
    unsigned char* header = (unsigned char*) malloc(14);
    fread(header, 1, 14, in);

    unsigned char* info = (unsigned char*) malloc(40);
    fread(info, 1, 40, in);

    unsigned int* filesize = header + 2;
    unsigned int* width = info + 4;
    unsigned int* height = info + 8;
    unsigned int* imageSize = info + 20;

    unsigned int matrixSize = *filesize - 54;
    unsigned char* previousMatrix = malloc(matrixSize);
    fread(previousMatrix, 1, matrixSize, in);
   
    printImageHex(previousMatrix, *width, *height);
    
    unsigned char* nextMatrix;
    if(shrink){
        nextMatrix = malloc(matrixSize / 4);
        int i, j, k, rowWidth = *width * 3, colorWidth = 3;
        for(i = 0; i < *height; i+= 2) {
            for(j = 0; j < *width; j += 2) {
                for(k = 0; k < 3; k++) {
                    unsigned char current = previousMatrix[i * rowWidth + j * colorWidth + k];
                    nextMatrix[i / 2 * rowWidth / 2 + j / 2 * colorWidth + k] = current;            // bottom-left  
                }
            }
        }

        *filesize = (*filesize - 54) / 4 + 54;
        *width /= 2;
        *height /= 2;
    } else {
        nextMatrix = malloc(4 * matrixSize);
        int i, j, k, rowWidth = *width * 3, colorWidth = 3;
        for(i = 0; i < *height; i++) {
            for(j = 0; j < *width; j++) {
                for(k = 0; k < 3; k++) {
                    unsigned char current = previousMatrix[i * rowWidth + j * colorWidth + k];
                    nextMatrix[i * 2 * rowWidth * 2 + j * 2 * colorWidth + k] = current;            // bottom-left  
                    nextMatrix[(i * 2 + 1) * rowWidth * 2 + j * 2 * colorWidth + k] = current;      // top-left
                    nextMatrix[i * 2 * rowWidth * 2 + (j * 2 + 1) * colorWidth + k] = current;      // bottom-right
                    nextMatrix[(i * 2 + 1) * rowWidth * 2 + (j * 2 + 1) * colorWidth + k] = current;// top-right
                }
            }
        }

        *filesize = (*filesize - 54) * 4 + 54;
        *width *= 2;
        *height *= 2;
    }
    *imageSize = *width * *height * 3;

    printImageHex(nextMatrix, *width, *height);

    out = fopen("expanded.bmp", "w");
    fwrite(header, 1, 14, out);
    fwrite(info, 1, 40, out);
    fwrite(nextMatrix, 1, *imageSize, out);

    
    free(header);
    free(info);
    free(previousMatrix);
    fclose(out);
    fclose(in);
    printf("Success!\n");
    return 0;
}


