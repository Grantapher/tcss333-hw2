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

    unsigned int* filesize = (unsigned int*) (header + 2);
    unsigned int* width = (unsigned int*) (info + 4);
    unsigned int* height = (unsigned int*) (info + 8);
    unsigned int* imageSize = (unsigned int*) (info + 20);

    unsigned int matrixSize = *filesize - 54;
    unsigned char* previousMatrix = malloc(matrixSize);
    fread(previousMatrix, 1, matrixSize, in);
  
    fclose(in);

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
    int writeCount = *imageSize;
    free(previousMatrix);

    out = fopen("altered.bmp", "w");
    if(14 != fwrite(header, 1, 14, out)) {
        fail("First header failed to write!", 4);
    }
    free(header);
    
    if(40 != fwrite(info, 1, 40, out)) {
        fail("Second header failed to write!", 6);
    }
    free(info);
    
    if(writeCount != fwrite(nextMatrix, 1, writeCount, out)) {
        printf("Pixels failed to write!", 5);
    }
    free(nextMatrix);

    fclose(out);
 
    printf("Success!\n");
    return 0;
}


