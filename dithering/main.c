#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define max 500


const int bayer_matrix_2x2[2][2] = {
    {0,2},
    {3,1},
};

const int bayer_matrix_4x4[4][4] = {
    {0, 8, 2, 10},
    {12, 4, 14, 6},
    {3, 11, 1, 9},
    {15, 7, 13, 5}
};

const int bayer_matrix_8x8[8][8] = {
        {0, 32, 8, 40, 2, 34, 10, 42},
        {48, 16, 56, 24, 50, 18, 58, 26},
        {12, 44, 4, 36, 14, 46, 6, 38},
        {60, 28, 52, 20, 62, 30, 54, 22},
        {3, 35, 11, 43, 1, 33, 9, 41},
        {51, 19, 59, 27, 49, 17, 57, 25},
        {15, 47, 7, 39, 13, 45, 5, 37},
        {63, 31, 55, 23, 61, 29, 53, 21}
};

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

void threshold(unsigned char *img, int w, int h, int c, int t)
{
    greyscaler(img,w,h,c);
    int i, j;
    for(i = 0; i < h; i++) {
        for(j = 0; j < w; j++) {
            int pID = (i * w + j) * c;
            int v;
            if (img[pID] >= t) {
                v = 255;
            } else {
                v = 0;
            }

            img[pID] = v;
            img[pID+1] = v;
            img[pID+2] = v;
            if (c > 3) {
                img[pID + 3] = img[pID + 3];
            }
        }

    }
    
    printf("threshold completed :) \n");
}

void average_dithering(unsigned char *img, int w, int h, int c)
{
    int i,j,k,tp = w*h*c;
    long sum=0;


    for(i=0;i<h;i++)
        for(j=0;j<w;j++)
            for(k=0;k<c;k++)
            {
                int pID  = (i * w + j) * c + k;
                sum += img[pID];
            }

    unsigned char avg = (unsigned char)(sum / tp);


    for(i=0;i<h;i++)
        for(j=0;j<w;j++)
            for(k=0;k<c;k++)
            {
                int pID = (i * w + j) * c + k;

                if (c == 4 && k == 3){
                    ;
                } else if (img[pID] >= avg) {
                    img[pID] = 255;
                } else {
                    img[pID] = 0;
                }
            } 
}


void bayer_dither(unsigned char *img, int w, int h, int c, const int *bm, int bm_size) {
<<<<<<< HEAD
    //greyscaler(img, w, h, c);
=======
    greyscaler(img, w, h, c);
>>>>>>> 4f5a539edabb7f292cfdf1353947c35420ff6faa

    int i,j,k,maxV = 0;
    for (i = 0; i < bm_size * bm_size; i++) {
        if (bm[i] > maxV) {
            maxV = bm[i];
        }
    }

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            int pID = (i * w + j) * c;

            float threshold = (bm[(i % bm_size) * bm_size + (j % bm_size)] / (float)maxV) * 255.0;

            for (k = 0; k < c; k++) {
                if (k < 3) {
                    if (img[pID + k] > threshold) {
                          img[pID + k] = 255;
                    } else {
                          img[pID + k] = 0;
                    }                
                }
            }
        }
    }
}


<<<<<<< HEAD
void quantize_pixel(unsigned char *px, int levels, int c) {
    int i;
    for (i = 0; i < c; i++) {
        if (i < c - 1){
            int level = (1 << levels) - 1;
            int step_size = 255 / level;
            px[i] = (px[i] / step_size) * step_size;
        }
    }
}

void quantize(unsigned char *img, int w, int h, int c, int l) {
    int i, j;
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            int pID = (i * w + j) * c;
            quantize_pixel(&img[pID], l, c);
        }
    }
}

int main(void){
    int w,h,channels,t,filter,bm_choice,level;
=======

int main(void){
    int w,h,channels,t,filter,bm_choice;
>>>>>>> 4f5a539edabb7f292cfdf1353947c35420ff6faa
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
   
    printf("choose the dithering effect :\n");
<<<<<<< HEAD
    printf("1)threshold\n2)average dithering\n3) Bayer Dithering\n4) quantize\n");
=======
    printf("1)threshold\n2)average dithering3) Bayer Dithering\n\n");
>>>>>>> 4f5a539edabb7f292cfdf1353947c35420ff6faa
    scanf("%d",&filter);
    switch(filter){
        case 1:
            do{
            printf("give the threshold value (0-255) :\n");
            scanf("%d",&t);
            }while(t < 0 || t > 255);
            threshold(img,w,h,channels,t);
        case 2:
            average_dithering(img,w,h,channels);
            printf("Average dithering completed.\n");
            break;
        case 3:
            printf("Choose Bayer matrix size:\n");
            printf("1) 2x2\n2) 4x4\n3) 8x8\n");
            scanf("%d", &bm_choice);

            switch (bm_choice) {
                case 1:
                    bayer_dither(img, w, h, channels, &bayer_matrix_2x2[0][0], 2);
                    break;
                case 2:
                    bayer_dither(img, w, h, channels, &bayer_matrix_4x4[0][0], 4);
                    break;
                case 3:
                    bayer_dither(img, w, h, channels, &bayer_matrix_8x8[0][0], 8);
                    break;
                default:
                    printf("Invalid choice. Defaulting to 4x4 Bayer matrix.\n");
                    bayer_dither(img, w, h, channels, &bayer_matrix_4x4[0][0], 4);
                    break;
            }
            printf("Bayer dithering completed.\n");
            break;
<<<<<<< HEAD
        case 4:
            do{
            printf("Choose a level to limit colors to : (level >= 2)\n");
            scanf("%d",&level);
            }while(level < 2);
            quantize(img,w,h,channels,level);
            printf("Quantizing completed.\n");
            break;
=======
>>>>>>> 4f5a539edabb7f292cfdf1353947c35420ff6faa
        default:
            printf("Invalid choice.\n");
            stbi_image_free(img);
            return 1;
    }
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
