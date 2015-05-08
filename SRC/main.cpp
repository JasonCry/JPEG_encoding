
#include <stdio.h>
#include <stdlib.h>
#include "../INC/lena_gray.h"
#include "../INC/JPEG400.h"

/*=====================================================================================
name: OpenFile
func: 打开一个用于保存编码后的JPEG图像的文件
input:无
return: 文件的句柄
description:
======================================================================================*/
FILE* OpenFile()
{
    FILE *fpWrite;
    if (NULL != (fpWrite = fopen("F:\\VS\\JPEG\\jpeg_debug\\Doc\\jpeg_out_data.jpg", "wb+")))
    {
        printf("Open File Successfully!\n");
    }
    else
    {
        printf("Cannot Open File!\n");
    }
    return fpWrite;
}


//#define width_img	512
//#define height_img	512
#define width_img	752
#define height_img	480

int main()
{
    int size = width_img * height_img * 3, sizeout = 0, x, y;
    //int size = 1280 * 720, sizeout = 0, x, y;
    //===========test varable------jxiao===================
    FILE *out_imag;
    out_imag = OpenFile();

    FILE *in_imag;
    if (NULL != (in_imag = fopen("jpeg_in_data.dat", "wb+")))
    {
        printf("Open File Successfully!\n");
    }
    else
    {
        printf("Cannot Open File!\n");
    }

    //===========test varable------jxiao===================
    unsigned char *outpict;
    outpict = (unsigned char *)malloc(size);
    unsigned char *inpict;
    inpict = (unsigned char *)malloc(size);

    inpict = lena;
    /*       for (x = 0; x < height_img; x++)//inpict初始化成RGB模式
    {
    for (y = 0; y < width_img; y++)
    {

    *(inpict + x*width_img+ y) = y % 255;
    //			*(inpict + x * 3 + y * width_img * 3) = y % 255;
    //			*(inpict + x * 3 + y * width_img * 3 + 1) = y % 255;// x % 200;
    //			*(inpict + x * 3 + y * width_img * 3 + 2) = y % 255;
    }

    }
    */
    fwrite(inpict, sizeof(unsigned char), width_img * height_img, in_imag);

    sizeout = encode_image(inpict, outpict, 1, 0, width_img, height_img);

    fwrite(outpict, sizeof(unsigned char), sizeout, out_imag);
    fclose(out_imag);

    free(outpict);
    //        free(inpict);




    return 0;
}