#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#pragma pack(1)
typedef unsigned char BYTE;//1 bytes of memory
typedef unsigned short WORD;//2 bytes of memory
typedef unsigned int DWORD;//4 bytes of memory
typedef int LONG;//4 bytes of memory
typedef struct _BMPFH//takes 14 bytes of memory
{
	BYTE bftype1;//1 byte and must be 'B'
	BYTE bftype2;//1 byte and must be 'M'
	DWORD bfsize;//4 bytes gives us the all size of bmp file (including headers,palette (if it has) data)
	WORD bfreserved1;// 2 btyes of memory could be set as 0
	WORD bfreserved2;// 2 btyes of memory could be set as 0
	DWORD bfOffbits;//4 bytes of memeory gives the position of data starts in the bmp file
} BMPFH;
typedef struct _BMPIH//40 bytes for windows bitmap file
{
	DWORD bisize;//4 bytes and it gives the size of info header
	LONG  biw;//4 bytes and it is the width of image
	LONG bih;//4 bytes and it is the height of iimage
	WORD biplane;//2 bytes which is not important for us
	WORD bibitcount;//2 bytes it is about the type of bitmap file if it is 1 2 color, 4 (16 colors) ..
	DWORD biComp;//4 bytes not important
	DWORD bisizeimage;//4 bytes and it gives the size of data in the image 
	LONG bix;//4 bytes not importatnt
	LONG biy;//4 bytes not important
	DWORD biclused;//4 bytes not important
 	DWORD biclimp;//4 byets not importatnt for us
}  BMPIH;
typedef struct _PALET//in palette it describes colors (what is the color number)
{
	BYTE rgbblue;//blue commponent
	BYTE rgbgreen;//green component
	BYTE rgbred;//red component
	BYTE rgbreserved;//reserved byte the user can use this for therir aims
} PALET;
typedef struct _IMAGE
{
	BMPFH   bmpfh;
	BMPIH   bmpih;
	PALET   *palet;
	BYTE    *data;
}  IMAGE;
IMAGE *ImageRead(IMAGE *image,char *filename)
{
	BMPFH bmpfh;
	BMPIH bmpih;
	FILE *fp;
	DWORD r,rowsize,size;
	fp=fopen(filename,"rb");//tries to open the filename
	if(fp==NULL) {printf("File is not found..");exit(1);}
	fread(&bmpfh,sizeof(BMPFH),1,fp);//reads bitmap file header from the file and set to bmph
	if(bmpfh.bftype1!='B' || bmpfh.bftype2!='M')
	{
		printf("This is not a bitmap file.");
		exit(1);
	}
    fread(&bmpih,sizeof(BMPIH),1,fp);//reads bitmap info header from the file and set to bmpih
	image=(IMAGE *) malloc(bmpfh.bfsize);//allocates memory for image
	if(image==NULL) {printf("There is no enough memory for this operation");exit(1);}
	image->bmpfh=bmpfh;//sets bmpfh to image 
	image->bmpih=bmpih;//sets bmpih to image
	
	r=0;//r is set to 0 in case 24 bits per pixel or more (this kind of images does not have color palette)
    if(bmpih.bibitcount==1) r=2;//if the image 1 bit per pixel (the number of clor is 2)
	if(bmpih.bibitcount==4) r=16;//if the image 4 bits per pixel (the number of clor is 16)
	if(bmpih.bibitcount==8) r=256;////if the image 8 bits per pixel (the number of clor is 256)
	if(r!=0)
	{
		image->palet=(PALET *) malloc(4*r);//allocate memory for color palette
		fread(image->palet,sizeof(BYTE),4*r,fp);//read color palette from image to the memory
	}
	rowsize=(image->bmpih.biw*image->bmpih.bibitcount+31)/32*4;//calculates 1 row of image size 
	size=rowsize*image->bmpih.bih;//all size of image is calculated and set to size 
	image->data=(BYTE *)malloc(size);//allocates enough memory for iamage data
	fread(image->data,size,1,fp);//reads image data from file and sets to image->data
	fclose(fp); //closes the file
	return image;//returns the adress of image on the memory
}       
void ImageWrite(IMAGE *image,char *filename)
{
	FILE *fp;
	int r,rowsize,size;
	
	fp=fopen(filename,"wb");//opens the file );exit(1);}
	if(fp==NULL) {printf("Fie opening error!");exit(1);}
	fwrite(&image->bmpfh,sizeof(BMPFH),1,fp);//writes the bitmap file header to the file
	fwrite(&image->bmpih,sizeof(BMPIH),1,fp);//writes he bitmep info header to the file		
	r=0;
    if(image->bmpih.bibitcount==1) r=2;//if the image has 2 colors
	if(image->bmpih.bibitcount==4) r=16;//if the image has 16 colors
	if(image->bmpih.bibitcount==8) r=256;//if the image has 256 colors
	if(r!=0) fwrite(image->palet,4*r,1,fp);//if the image has color palette the palette is written to the file
	rowsize=(image->bmpih.biw*image->bmpih.bibitcount+31)/32*4;//a row size of image is calculated 
	size=rowsize*image->bmpih.bih;//all size of image is calculated
	fwrite(image->data,size,1,fp);//write image data to the file
	fclose(fp);//closes the file 
}
void writeInfo(IMAGE *image,char *fname)
{
	printf("--------Info about %s  image file\n",fname);
	printf("bfType value		:%c%c\n",image->bmpfh.bftype1,image->bmpfh.bftype2);
	printf("bfsize			:%d\n",image->bmpfh.bfsize);
	printf("bfreserved1		:%d\n",image->bmpfh.bfreserved1);
	printf("bfreserved2		:%d\n",image->bmpfh.bfreserved2);
	printf("bfOffbits		:%d\n",image->bmpfh.bfOffbits);
	printf("bisize			:%d\n",image->bmpih.bisize);
	printf("Width			:%d\n",image->bmpih.biw);
	printf("Height			:%d\n",image->bmpih.bih);
	printf("biplane			:%d\n",image->bmpih.biplane);
	printf("bibitcount		:%d\n",image->bmpih.bibitcount);
	printf("Compression		:%d\n",image->bmpih.biComp);
	printf("bisizeimage		:%d\n",image->bmpih.bisizeimage);
	printf("bix			:%d\n",image->bmpih.bix);
	printf("biy			:%d\n",image->bmpih.biy);
	printf("bi color used		:%d\n",image->bmpih.biclused);
	printf("bi color imp.		:%d\n",image->bmpih.biclimp);
}

void convertToGray(IMAGE *image, IMAGE *grayImage)
{
    int width = image->bmpih.biw;
    int height = image->bmpih.bih;
    int rowsize = (width * image->bmpih.bibitcount + 31) / 32 * 4;
    int newRowsize = (width + 3) / 4 * 4;
    int size = newRowsize * height;

    grayImage->bmpfh = image->bmpfh;
    grayImage->bmpih = image->bmpih;
    grayImage->bmpih.bibitcount = 8;
    grayImage->bmpih.bisizeimage = size;
    grayImage->bmpfh.bfsize = 14 + 40 + 256 * 4 + size;
    grayImage->bmpfh.bfOffbits = 14 + 40 + 256 * 4;

    grayImage->palet = (PALET *)malloc(256 * 4);
    for (int i = 0; i < 256; i++)
    {
        grayImage->palet[i].rgbred = i;
        grayImage->palet[i].rgbgreen = i;
        grayImage->palet[i].rgbblue = i;
        grayImage->palet[i].rgbreserved = 0;
    }

    grayImage->data = (BYTE *)malloc(size);
    for (int j = 0; j < height; j++)
    {
        for (int k = 0; k < width; k++)
        {
            int index = j * rowsize + k * 3;
            BYTE red = image->data[index + 2];
            BYTE green = image->data[index + 1];
            BYTE blue = image->data[index];
            BYTE gray = (BYTE)(0.299 * red + 0.587 * green + 0.114 * blue);
            grayImage->data[j * newRowsize + k] = gray;
        }
    }
}

int main()
{
    IMAGE *image1 = (IMAGE *)malloc(sizeof(IMAGE));
    image1 = ImageRead(image1, "lena_color.bmp");
    
    IMAGE *grayImage1 = (IMAGE *)malloc(sizeof(IMAGE));
    convertToGray(image1, grayImage1);
    ImageWrite(grayImage1, "gray_lena.bmp");
    
    writeInfo(image1, "lena_color.bmp");
    printf("\n");
    writeInfo(grayImage1,"gray_lena.bmp");
    
	printf("\n*****************************************************\n");
    
    IMAGE *image2 = (IMAGE *)malloc(sizeof(IMAGE));
    image2 = ImageRead(image2, "parrots.bmp");
    
    IMAGE *grayImage2 = (IMAGE *)malloc(sizeof(IMAGE));
    convertToGray(image2, grayImage2);
    ImageWrite(grayImage2, "gray_parrots.bmp");
    
    writeInfo(image2, "parrots.bmp");
    printf("\n");
    writeInfo(grayImage2, "gray_parrots.bmp");

    free(image1);
    free(grayImage1);
    free(image2);
    free(grayImage2);
    return 0;
}
