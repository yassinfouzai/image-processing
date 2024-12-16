#include <stdio.h>
#include <math.h>
#include <stdlib.h>

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

void testing(unsigned char *img, int w, int h, int c)
{
    unsigned char *nimg = (unsigned char*)calloc(w * h * c, sizeof(unsigned char));
    if (nimg == NULL) {
        printf("Memory allocation failed for smaller image.\n");
    } else {
        printf("Memory allocation succeeded for smaller image.\n");
        free(nimg);
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

void genGaussian(float *kernel, int size, int sigma)
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
void gaussianBlur(unsigned char *img, int w, int h, int c, int tension, int strength)
{
    int ks = 2*tension + 1;
    float *kernel = (float *)malloc(ks * ks * sizeof(float));
    if (!kernel) {
        printf("Failed to allocate memory for the kernel\n");
        return;
    }
    genGaussian(kernel,ks,strength);
    printf("working..\n");
    applyKernel(img,w,h,c,ks,kernel);

    free(kernel);
}


void bilateralBlur(unsigned char *img, int w , int h, int c, int color, int space)
{
}

int main(void){
    int w,h,channels,filter;
    char name[max],nname[max];

    int tension,space,color,strength;
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
    printf("1)Box Blur\n2)Gaussian Blur\n3)Bilateral Blur\n");
    scanf("%d",&filter);
    switch(filter){
        case 1:
            do{
            printf("give the tension value  (1-5) :\n");
            scanf("%d",&tension);
            }while(tension < 1 || tension > 5);
            boxBlur(img,w,h,channels,tension); 
            printf("Box blur completed.\n");
            break;
        case 2:
            do{
            printf("give the tension value  (1-5) :\n");
            scanf("%d",&tension);
            }while(tension < 1 || tension > 5);
            
            do{
            printf("give the strength value  (1-5) :\n");
            scanf("%d",&strength);
            }while(strength < 1 || strength > 5);

            gaussianBlur(img,w,h,channels,tension,strength);
            printf("Gaussian blur completed.\n");
            break;
        case 3:
            do{
            printf("Choose a kernel space : (space >= 2)\n");
            scanf("%d",&space);
            }while(space < 2);

            bilateralBlur(img,w,h,channels,color,space);
            printf("Gaussian blur completed.\n");
            break;
        case 4:
            testing(img,w,h,channels);
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
