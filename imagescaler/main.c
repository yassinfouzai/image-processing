#include <stdio.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define max 200

void pixelChannels(unsigned char * pixels){
    int n;
    printf("give the pixel number :\n");
    scanf("%d",&n);
    printf("this is the pixel channels (%d,%d,%d,%d)\n", pixels[4*n], pixels[4*n+1], pixels[4*n+2],pixels[4*n+3]);
}


void downscaler(unsigned char *img, unsigned char *nImg, int w, int h, int c, int nw, int nh, int f) {
    for (int i = 0; i < nw; i++) {
        for (int j = 0; j < nh; j++) {
            int sum[4] = {0}; // for the number of channels (up to 4)
            int count = 0;

            // Iterate over the f x f block
            for (int u = 0; u < f && (i * f + u) < w; u++) {
                for (int k = 0; k < f && (j * f + k) < h; k++) {
                    int id = ((j * f + k) * w + (i * f + u)) * c; // Calculate the index of the pixel

                    // Sum the channels
                    for (int cc = 0; cc < c; cc++) {
                        sum[cc] += img[id + cc];
                    }
                    count++;
                }
            }

            // Average the values for each channel and assign to the downscaled image
            for (int cc = 0; cc < c; cc++) {
                nImg[(j * nw + i) * c + cc] = sum[cc] / count;
            }
        }
    }
}

int main(void){
    int w,h,channels,f;
    char name[max];

    printf("Give the name of your image ( ends with .png) : \n");
    scanf("%s",name);
    
    printf("starting the program... \n");
    unsigned char * img = stbi_load(name,&w,&h,&channels,0);
    
    if(img == NULL){
        printf("Error loading your image \n");
        exit(1);
    }
    printf("Now loading your image with widht of %dpx and height of %dpx and channels %d\n",w,h,channels);

    do{
    printf("give me the downscale factor (f>= 2)\n");
    scanf("%d",&f);
    }while(f < 2);


    //allocating a memory for downscaling
    int nW = w/f,nH = h/f;
    unsigned char  *nImg;
    nImg = (unsigned char*)malloc(nW*nH*channels);
    if (nImg == NULL){
        printf("Failed to allocate memory for downscaling :( ..\n");
        stbi_image_free(img);
        exit(2);
    }
    

    //downscaling the image
    downscaler(img,nImg, w, h, channels, nW, nH,f);
    

    // overwriting the original image with the new downscaled image
    if (!stbi_write_png(name, nW, nH, channels, nImg, nW * channels)) {
        printf("Failed to save the downscaled image.\n");
    } else {
        printf("Original image overwritten with downscaled image.\n");
    }

    stbi_image_free(img);
    return 0;
}
