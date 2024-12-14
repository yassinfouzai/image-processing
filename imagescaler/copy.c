#include <stdio.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

void pixelChannels(unsigned char * pixels){
    int n;
    printf("give the pixel number :\n");
    scanf("%d",&n);
    printf("this is the pixel channels (%d,%d,%d,%d)\n", pixels[4*n], pixels[4*n+1], pixels[4*n+2],pixels[4*n+3]);
}

void imgInvert(unsigned char *img,int w, int h, int c)
{
    for(int i=0; i<w*h*c;i++)
        img[i] = 255 - img[i];

}

int main(void){
    int w,h,channels,f;
    
    printf("starting the program... \n");
    unsigned char * img = stbi_load("test.png",&w,&h,&channels,0);
    
    if(img == NULL){
        printf("Error loading your image \n");
        exit(1);
    }
    printf("Now loading your image with widht of %dpx and height of %dpx and channels %d\n",w,h,channels);

    
    
    unsigned char  *nImg = (unsigned char *)malloc(w*h*channels);
    if (nImg == NULL) {
        printf("Failed to allocate memory for the new image.\n");
        stbi_image_free(img);
        return -1;
    }
    memcpy(nImg, img, w * h * channels);
    imgInvert(nImg,w,h,channels);

    // Save the new image to a outputpng
    if (!stbi_write_png("output.png", w, h, channels, nImg, w * channels)) {
        printf("Failed to save the new image.\n");
    } else {
        printf("New image saved as output.png\n");
    }


    

    stbi_image_free(img);
    free(nImg);
    return 0;
}
