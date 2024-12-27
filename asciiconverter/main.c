#include <stdio.h>
#include <string.h>
#include <errno.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// TODO: make a header file containing all these definitions and macros.
// I'll be getting to it soon enough.

#define IMAGE_PIXEL_CHANNELS(pixels, pixel_num) printf("Pixel Channels:\t"     \ 
													   "%d\t%d\t%d\t%d\t\n",   \
												 	   pixels[4*pixel_num],	   \        
												       pixels[4*pixel_num+1],  \ 		    
												       pixels[4*pixel_num+2],  \ 	       
												       pixels[4*pixel_num+3]);         

#define IMAGE_SHOW_INFO(image) printf("Image name: %s\nImage width: %d\n"			 \ 
									  "Image height: %d\nImage channel count: %d\n", \ 
									  image.name, image.width, image.height,		 \ 
									  image.channels);

#define F 10
#define RGB_MAX 255

#define CHAR_SIZE sizeof(char)
#define MAX_STR_LENGTH 640000

#define CHARS "`.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fneoZ5Yxjy]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@"
#define CHARS_LEN strlen(CHARS)

typedef unsigned int Colorcode;

typedef unsigned char Pixel;

typedef struct {
	Pixel *data;
	const char *name;	
	int width;
	int height;
	int channels;	
} Image;



Image
image_init(const char *image_name){
	Image image = {
				  	.name = image_name
				  }; 

	image.data = stbi_load(image.name,
		  	     		   &image.width, 
		  	     		   &image.height, 
		  	     		   &image.channels, 
		  	     		   STBI_default);

	if(image.data == NULL){
		fprintf(stderr, "STB ERROR: %s\n", stbi_failure_reason());
		exit(1);
	}
	// This particular coding style is inspired by Tsoding.
	// Pretty good channel, even for beginners. Go watch him.
	// He makes videos related to all kinds of things related to programming,
	// in several languages (including C, and lots of it at that). 

	return image;
}


void 
image_invert(Image *image)
{
    for(int i = 0; i < image->width * image->height * image->channels; i++){
        image->data[i] = RGB_MAX - image->data[i];
	}
}

void
image_downscale(Image *dst_img, const Image src_img) {
	// Create the new scaled image.
	dst_img->width = src_img.width/F;
	dst_img->height = src_img.height/(2 * F);
	dst_img->data = malloc(dst_img->width * dst_img->height * src_img.channels);

	// It's i, j, k, and then l.
	// 1 and l do indeed look confusing, though, I must add.
	// Even then, if you have syntax highlighting on,
	// then it shouldn't be much of a trouble differentiating
	// the two.
	// TODO: actually understand how this works.
	int count, k, l;
    for (int i = 0; i < dst_img->width; i++) {
        for (int j = 0; j < dst_img->height; j++) {
            int sum[4] = {0}; 
            count = 0;

			k = 0;
			while(k < F && (i * F + k) < src_img.width){
				l = 0;
				while(l < (2 * F) && (j * 2 * F + l) < src_img.height){
					int pixel_id = ((j * 2 * F + l) * src_img.width + (i * F + k)) * \ 
								   src_img.channels;

                    for (int cc = 0; cc < src_img.channels; cc++) {
                        sum[cc] += src_img.data[pixel_id + cc];
                    }

					count++;
					l++;
				}
				k++;
			}

            for (int cc = 0; cc < src_img.channels; cc++) {
				// Too long for the screen to just put it directly between the
				// brackets.
				// ECPL (eighty characters per line rule).
				int index = (j * dst_img->width + i) * src_img.channels + cc;
                dst_img->data[index] = sum[cc] / count;
            }
        }
    }
}

void
image_generate_ascii_file(Image image, const char *ascii_file_name){
	// TODO: add checks for ascii_file_name.
	FILE *file_ptr;
	file_ptr = fopen(ascii_file_name, "w");
	if(file_ptr == NULL){
		fprintf(stderr, "ERROR: file could not be opened (error %d)", errno);
		exit(1);
	}

	Colorcode A, R, G, B;
	float colors_mean;
	int char_index;

	int i = 0;
	// Not sure what rw and cl stand for.
    for(int rw = 0; rw < image.height; rw++){
        for(int cl = 0; cl < image.width; cl++){
            if(image.channels == STBI_rgb_alpha){
                A = *image.data++;
            }
			R = *image.data++;
			G = *image.data++;
			B = *image.data++;

			// I thought of doing:
			// colors_mean = (A + R + G + B)/(float)image.channels;
			// but for some reason, that leads colors_mean to become 0,
			// causing chaos.
			// TODO: fix it.
			colors_mean = (A + R + G + B)/3.0;
            char_index = (int)(CHARS_LEN * (colors_mean/(2 * RGB_MAX)));
            fputc(CHARS[char_index], file_ptr);

			i++;
        }
        fputc('\n',file_ptr);
    }
	fclose(file_ptr);
}

// TODO: make this into a full-fledged CLI utility, with
// all the possible flags properly configured. We can also add 
// a flag that determines if you want to use the program in CLI
// mode or in GUI mode.
int 
main(int argc, char *argv[]){
	if(argc != 3){
		fprintf(stderr, "usage: ./ascii (IMAGE_NAME) (ASCII_FILE_NAME)");	
		return EXIT_FAILURE;
	}

	Image input_image = image_init(argv[1]);
	fprintf(stdout, "Image loaded successfully. Here's information about your image.\n");
	IMAGE_SHOW_INFO(input_image);
	IMAGE_PIXEL_CHANNELS(input_image.data, input_image.channels);

	Image downscaled_image;
	image_downscale(&downscaled_image, input_image);
	image_generate_ascii_file(downscaled_image, argv[2]);
	fprintf(stdout, "An ascii file named %s has been generated out of %s. Goodbye.\n",
			argv[2], input_image.name);

	return EXIT_SUCCESS;
}
