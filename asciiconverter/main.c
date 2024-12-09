#include <stdio.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define max 500
// My suggestion.
//#define CHARS "`.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fneoZ5Yxjy]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@"

void pixelChannels(unsigned char * pixels){
    int n;
    printf("give the pixel number :\n");
    scanf("%d",&n);
    // Grammatical error here. Fixed by Kurowyn.
    printf("These are the pixel channels (%d,%d,%d,%d)\n", pixels[4*n], pixels[4*n+1], pixels[4*n+2],pixels[4*n+3]);
}

void imgInvert(unsigned char *img,int w, int h, int c)
{
    for(int i=0; i<w*h*c;i++)
        img[i] = 255 - img[i];

}

void downscaler(unsigned char *img, unsigned char *nImg, int w, int h, int c, int nw, int nh, int f) {
    for (int i = 0; i < nw; i++) {
        for (int j = 0; j < nh; j++) {
            int sum[4] = {0}; // for the number of channels (up to 4)
            int count = 0;

            // Iterate over the f x f block
            for (int u = 0; u < f && (i * f + u) < w; u++) {
                for (int k = 0; k < (2*f) && (j * 2*f + k) < h; k++) {
                    int id = ((j * 2 * f + k) * w + (i * f + u)) * c; // Calculate the index of the pixel

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
    int w,h,channels;
    int f = 10; //dowscale factor
    char name[max];
    
    printf("give the name of your image (ends with .png or .jpg)\n");
    // Better to use fgets instead.
    // scanf does not check for buffer overflow, which could result in serious undefined behavior.
    scanf("%s",name);
    
    
    printf("starting the program..\n");

    unsigned char *img = stbi_load(name,&w,&h,&channels,0);
    // Better to use a macro here.
    char chars[] = 
"`.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fneoZ5Yxjy]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
    int charLen = strlen(chars);
    if(img == NULL){
        printf("Error loading your image \n");
        exit(1);
    }
    printf("Now loading your image with widht of %dpx and height of %dpx and channels %d\n",w,h,channels);
    
    
    
    //allocating a memory for downscaling
    int nW = w/f,nH = h/(2*f);
    unsigned char  *nImg;
    nImg = (unsigned char*)malloc(nW*nH*channels);
    if (nImg == NULL){
        printf("Failed to allocate memory for downscaling :( ..\n");
        stbi_image_free(img);
        exit(1);
    }
    

    //downscaling the image
    downscaler(img,nImg, w, h, channels, nW, nH,f);

    //initializing the output file
    FILE *fptr ;
    fptr= fopen("output.txt", "w");

    //converting image to ascii
    unsigned char *pixels = nImg;
    for(int rw=0; rw < nH; rw++ ){
        for(int cl=0; cl < nW; cl++){
            unsigned int R = *pixels++;
            unsigned int G = *pixels++;
            unsigned int B = *pixels++;
            if(channels >= 4){
                unsigned int A = *pixels++;
            }
            float Avg = (R+G+B)/ 3.0;
            int charId = (int)(charLen * (Avg / (2*255.)));
            fputc(chars[charId],fptr);
        }
        fputc('\n',fptr);
    }

    fclose(fptr);
    
    stbi_image_free(img);
    free(nImg);
}
