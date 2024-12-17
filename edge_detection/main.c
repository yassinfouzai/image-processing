#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define max 500

const int SOBEL_X[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
};

const int SOBEL_Y[3][3] = {
    {-1, -2, -1},
    { 0,  0,  0},
    { 1,  2,  1}
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

void threshold(unsigned char *img, int w, int h, int c, unsigned char t)
{
    greyscaler(img,w,h,c);
    int i, j;
    for(i = 0; i < h; i++) {
        for(j = 0; j < w; j++) {
            int pID = (i * w + j) * c;
            int v;
            if (img[pID] >= t)
                v = 255;
            else
                v = 0;

            img[pID] = v;
            img[pID+1] = v;
            img[pID+2] = v;
            if (c > 3)
                img[pID + 3] = img[pID + 3];
        }

    }
    
    printf("threshold completed :) \n");
}

void doubleThreshold(float *m, unsigned char *img, int w, int h, int c, float tl, float th) {
    int i,j;

    for (i=0;i<h;i++)
        for (j=0;j<w;j++) {
            int pID = (i * w + j) * c;
            int mID = i * w + j;
            int v;
            if (img[pID] >= th)
                v = 255;
            else if (img[pID] >= tl)
                v = 128;
            else 
                v = 0;

            img[pID] = v;
            img[pID+1] = v;
            img[pID+2] = v;
        }
}


void applyKernel(unsigned char *img, int w, int h, int c, int ks, float *kernel)
{
    int i,j,k,off=ks/2;
    float sum;

    unsigned char *nimg = (unsigned char*)malloc(w * h * c * sizeof(unsigned char));

    if(nimg == NULL) {
        printf("failed to allocate memory\n");
    }

    for(i=0;i<h;i++)
        for(j=0;j<w;j++)
            for(k=0;k<c;k++){
                sum = 0.0;
                for(int y=-off;y<=off;y++)
                    for(int x=-off;x<=off;x++)
                    {
                        int yy = y + i;
                        int xx = x + j;

                        if(yy >= 0 && yy < h && xx >= 0 && xx < w){
                            int pID = (yy * w + xx) * c + k;
                            int kID = (y + off) * ks + (x + off);
                            sum += img[pID] * kernel[kID];
                        }
                    }

                int pID = (i * w + j) * c + k;
                nimg[pID] = (unsigned char)sum;
            }

    memcpy(img, nimg, w*h*c);
    free(nimg);
}

void genBox(float *kernel,int size)
{
    int i;
    float v = 1.0/(size*size);
    for(i=0;i<size*size;i++)
        kernel[i] = v;

}

void boxBlur(unsigned char *img, int w, int h, int c, int tension)
{
    int ks = 2*tension + 1;
    float *kernel = (float *)malloc(ks * ks * sizeof(float));
    if (!kernel) {
        printf("Failed to allocate memory for the kernel\n");
        return;
    }
    genBox(kernel,ks);
    applyKernel(img,w,h,c,ks,kernel);
    free(kernel);

}

void genGaussian(float *kernel, int size, float sigma)
{
    int i,j;
    int off = size/2;
    float sum = 0.0;
    

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int x = i - off;
            int y = j - off;
            kernel[i * size + j] = exp(-(float)(x * x + y * y) / (2.0f * sigma * sigma));
            sum += kernel[i * size + j];
        }
    }

    for(i=0;i<size*size;i++)
        kernel[i] /= sum;

}
void gaussianBlur(unsigned char *img, int w, int h, int c, int tension, float strength)
{
    int ks = 2*tension + 1;
    float *kernel = (float *)malloc(ks * ks * sizeof(float));
    if (!kernel) {
        printf("Failed to allocate memory for the kernel\n");
        return;
    }
    genGaussian(kernel,ks,strength);
    applyKernel(img,w,h,c,ks,kernel);

    free(kernel);
}

void computeGradients(unsigned char *img, int w, int h, int c, float *m, float *d)
{
    int i,j,k,l,gi,gj;
    for(i = 1; i < h - 1; i++)
        for(j = 1; j < w - 1; j++) {
            gi = 0;
            gj = 0;

            for (k = -1; k <= 1; k++)
                for (l = -1; l <= 1; l++) {
                    int ni = i + k;
                    int nj = j + l;
                    
                    if(ni >= 0 && ni < h && nj >= 0 && nj < w) {
                        int pixel = img[(ni * w + nj) * c];
                        gj += pixel * SOBEL_X[k + 1][l + 1];
                        gi += pixel * SOBEL_Y[k + 1][l + 1];
                    }
                }
            int id = i * w + j; 
            m[id] = sqrt(gi * gi + gj * gj);
            d[id] = atan2(gi, gj);
        }
}

void hysteresis(unsigned char *img, int w, int h, int c) {
    int i, j, n, ni, nj, v;
    bool isconnected;
    // Neighbor offsets for 8 neighbors
    int neighbors[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        { 0, -1},          { 0, 1},
        { 1, -1}, { 1, 0}, { 1, 1}
    };


    for (i = 1; i < h - 1; i++) 
        for (j = 1; j < w - 1; j++) {
            int pID = (i * w + j) * c;


            if (img[pID] == 128) {
                isconnected = false;
                for (n = 0; n < 8; n++) {
                    ni = i + neighbors[n][0];
                    nj = j + neighbors[n][1];
                    if (ni >= 0 && ni < h && nj >= 0 && nj < w) {
                        int nID = (ni * w + nj) * c;
                        if (img[nID] == 255){
                            isconnected = true;
                            break;
                        }
                    }
                }
                v = isconnected ? 255 : 0;
                img[pID] = v;   
                img[pID + 1] = v;
                img[pID + 2] = v;
                if (c > 3)
                    img[pID + 3] = img[pID + 3];
            }
        }
}

void nonMaxSuppression(float *magnitude, float *direction, unsigned char *nms, int w, int h, int c) {
    int i, j;
    float angle, mag1, mag2;
    int x, y;

    for (i = 0; i < h - 1; i++)
        for (j = 0; j < w - 1; j++) {
            int pID = (i * w + j) * c;
            angle = direction[i * w + j];

            //Normalize angle to 0-180
            if (angle < 0) angle += M_PI;
            if (angle >= M_PI) angle -= M_PI;

            if ((angle >= 0 && angle < M_PI / 8) || (angle >= 7 * M_PI / 8 && angle < M_PI)) {
                x = 1;
                y = 0;
            } else if (angle >= M_PI / 8 && angle < 3 * M_PI / 8) {
                x = 1;
                y = -1;
            } else if (angle >= 3 * M_PI / 8 && angle < 5 * M_PI / 8) {
                x = 0;
                y = 1;
            } else {
                x = -1;
                y = -1;
            }

            mag1 = magnitude[(i + y) * w + (j + x)];
            mag2 = magnitude[(i - y) * w + (j - x)];

            if (magnitude[i * w + j] >= mag1 && magnitude[i * w + j] >= mag2)
                for (int k = 0; k < c; k++)
                    nms[pID + k] = (unsigned char)magnitude[i * w + j];
            else 
                for (int k = 0; k < c; k++)
                    nms[pID + k] = 0;
        }
}


void canny(unsigned char *img, int w, int h, int c, int tension, float strength, unsigned char th, unsigned char tl) 
{
    greyscaler(img,w,h,c);
    gaussianBlur(img,w,h,c,tension,strength);

    float *magnitude = (float *)malloc(w * h * sizeof(float));
    float *direction = (float *)malloc(w * h * sizeof(float));
    unsigned char *nms = (unsigned char *)malloc(w * h * c * sizeof(unsigned char));
    if (!magnitude || !direction || !nms) {
        printf("Failed to allocate memory.\n");
        free(magnitude);
        free(direction);
        free(nms);
        return;
    }

    printf("computing gradients..\n");
    computeGradients(img, w, h, c, magnitude, direction);
    
    printf("Applying the non maximum suppresion..\n");
    nonMaxSuppression(magnitude,direction,nms,w,h,c);

    printf("Applying the double doubleThreshold..\n");
    doubleThreshold(magnitude,nms,w,h,c,tl,th);
    
    printf("Applying the hysteresis..\n");
    hysteresis(nms,w,h,c);

    memcpy(img, nms, w * h * c * sizeof(unsigned char));

    free(magnitude);
    free(direction);
    free(nms);
}

int main(void){
    int w,h,channels,filter;
    char name[max],nname[max];
    
    unsigned char threshold,threshold2;
    int tension,space,color;
    float strength;
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
    printf("1)Canny Edge Detection\n");
    scanf("%d",&filter);
    switch(filter){
        case 1:

            do{
                printf("give the tension value  (1-5) :\n");
                scanf("%d",&tension);
            }while(tension < 1 || tension > 5);

            do{
                printf("give the strength value (1.0-5.0):\n");
                scanf("%f",&strength);
            }while(strength < 1.0 || strength > 5.0);
            
            do{
                printf("give the high threshold value (0-255):\n");
                scanf("%hhu",&threshold);
            }while(threshold < 0 || threshold > 255);

            do{
                printf("give the low threshold value (0-255):\n");
                scanf("%hhu",&threshold2);
            }while(threshold2 < 0 || threshold2 > threshold);
            canny(img,w,h,channels,tension,strength,threshold,threshold2); 
            printf("Canny Edge Detection completed.\n");
            break; 
        default:
            printf("Invalid choice.\n");
            stbi_image_free(img);
            return 1;
    }
    printf("give a name to save to ..(ends with .png)\n");
    scanf("%s",nname);

    // save the image
    if (!stbi_write_png(nname, w, h, channels, img, w * channels)) {
        printf("Error saving the image\n");
        stbi_image_free(img);
        return 1;
    } else {
        printf("Image saved successfully as %s\n", nname);
    }   

    
    stbi_image_free(img);
    printf("ended the program \n");
    return 0;
}
