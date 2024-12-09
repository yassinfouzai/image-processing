#include <stdio.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

int main(void){
    int w,h,channels;
    unsigned char *img = stbi_load("sky.jpg",&w,&h,&channels,0);
    if(img == NULL){
        printf("Error loading your image \n");
        exit(1);
    }
    printf("Now loading your image with widht of %dpx and height of %dpx",w,h);


    stbi_image_free(img);
}
