#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define max 500

void pixelChannels(unsigned char * pixels){
    int n;
    printf("give the pixel number :\n");
    scanf("%d",&n);
    printf("this is the pixel channels (%d,%d,%d,%d)\n", pixels[4*n], pixels[4*n+1], pixels[4*n+2],pixels[4*n+3]);
}

void greyscaler(unsigned char *img, int w, int h, int c)
{ 
    int i,j;
    for(i=0;i<h;i++)
        for(j=0;j<w;j++){

            int pID = (i * w + j) * c;
            int R = img[pID];
            int G = img[pID+1];
            int B = img[pID+2];


            //weighted greyscaling
            unsigned char grey = (unsigned char)(0.299 * R + 0.587 * G + 0.114 * B);
            
            img[pID] = grey;
            img[pID+1] = grey;
            img[pID+2] = grey;
            
            if (c > 3) {
                img[pID+3] = img[pID+3];
            }

        }
}


int main(void){
    int w,h,channels;
    char name[max];
    
    printf("give the name of the image (.png) : \n");
    scanf("%s",name);
    
    printf("starting the program ..\n");
    unsigned char *img = stbi_load(name,&w,&h,&channels,0);

    if(img == NULL){
        printf("Error loading your image \n");
        exit(1);
    }
    printf("Now loading your image with widht of %dpx and height of %dpx and channels %d\n",w,h,channels);

    greyscaler(img,w,h,channels);

    // Overwrite the image
    if (!stbi_write_png(name, w, h, channels, img, w * channels)) {
        printf("Error saving the image\n");
        stbi_image_free(img);
        return 1;
    } else {
        printf("Image saved successfully as %s\n", name);
    }   

    stbi_image_free(img);
    printf("ended the program \n");
    return 0;
}
