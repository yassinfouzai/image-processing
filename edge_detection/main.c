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

void doubleThreshold(float *m, unsigned char *img, int w, int h, float tl, float th) {
    for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++) {
            int pID = i * w + j;

            if (m[pID] >= th)
                img[pID] = 255;
            else if (m[pID] >= tl)
                img[pID] = 128;
            else 
                img[pID] = 0;
        }
}


void applyKernel(unsigned char *img, int w, int h, int c, int ks, float *kernel)
{
    int i,j,k,off=ks/2;
    float sum;

    printf("working1..\n");
    unsigned char *nimg = (unsigned char*)malloc(w * h * c * sizeof(unsigned char));

    printf("working2..\n");
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

    printf("working 3...\n");
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

    printf("working4..\n");
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

    printf("working 4..\n");
    free(kernel);
    printf("working 5.....\n");
}

void computeGradients(unsigned char *img, int w, int h, int c, float *m, float *d)
{
    int i,j,k,l,gi,gj;
    printf("working 7.1\n");
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
    printf("working 7.2\n");
}

void hysteresis(unsigned char *img, int w, int h) {

    int i,j,n,ni,nj;
    bool isconnected;
    // Neighbor offsets for 8 neighbors
    int neibors[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}
    };

    for (i = 1; i < h - 1; i++) {
        for (j = 1; j < w - 1; j++) {
            int pID = i * w + j;

            if (img[pID] == 128) {
                isconnected = false;
                for (n = 0; n < 8; n++) {
                    ni = i + neibors[n][0];
                    nj = j + neibors[n][1];
                    int nID = ni * w + nj;
                    
                    if (img[nID] == 255) {
                        isconnected = true;
                        break;
                    }
                }

                if (isconnected) {
                    img[pID] = 255;                 
                } else {
                    img[pID] = 0;                 
                }
            }
        }
    }
}

void canny(unsigned char *img, int w, int h, int c, int tension, float strength, unsigned char threshold) 
{
    greyscaler(img,w,h,c);
    gaussianBlur(img,w,h,c,tension,strength);
    printf("working6\n");

    float *magnitude = (float *)malloc(w * h * sizeof(float));
    float *direction = (float *)malloc(w * h * sizeof(float));
    if (!magnitude || !direction) {
        printf("Failed to allocate memory for gradients.\n");
        free(magnitude);
        free(direction);
        return;
    }
    printf("working 7 ?\n");
    computeGradients(img, w, h, c, magnitude, direction);
    printf("working 8\n");
    doubleThreshold(magnitude,img,w,h,threshold*0.5,threshold);
    printf("working 9\n");
    hysteresis(img,w,h);
    printf("working 10\n");
    free(magnitude);
    free(direction);
}

int main(void){
    int w,h,channels,filter;
    char name[max],nname[max];
    
    unsigned char threshold;
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
                printf("give the threshold value (0-255):\n");
                scanf("%hhu",&threshold);
            }while(threshold < 0 || threshold > 255);

            canny(img,w,h,channels,tension,strength,threshold); 
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
