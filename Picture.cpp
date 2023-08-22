#include "Picture.hpp"
bool PICTURE::ReadJPEGFile(string FileName)
{
    jpeg_decompress_struct cinfo;
    jpeg_error_mgr JPEGError;
    cinfo.err = jpeg_std_error(&JPEGError);
    jpeg_create_decompress(&cinfo);

    FILE *InputFilePointer = fopen(FileName.c_str(), "rb");
    if (InputFilePointer == NULL)
    {
        cout << "can't open " << FileName << endl;
        return false;
    }
    jpeg_stdio_src(&cinfo, InputFilePointer);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);
    Width = cinfo.output_width;
    Height = cinfo.output_height;
    unsigned char *Buffer = (unsigned char *)malloc(cinfo.output_width * cinfo.output_components);
    unsigned int i = 0;
    while (cinfo.output_scanline < cinfo.output_height)
    {
        jpeg_read_scanlines(&cinfo, &Buffer, 1);
        for (unsigned int j = 0; j < cinfo.output_width; j++)
        {
            Data[i][j].r = Buffer[j * 3];
            Data[i][j].g = Buffer[j * 3 + 1];
            Data[i][j].b = Buffer[j * 3 + 2];
        }
        i++;
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(InputFilePointer);
    free(Buffer);
    return true;
}
bool PICTURE::ReadPNGFile(string FileName)
{
    PICTURE Picture;
    unsigned char *Buffer = NULL;
    unsigned int Error = lodepng_decode32_file(&Buffer, &Width, &Height, FileName.c_str());
    if (Error)
    {
        printf("error %u: %s\n", Error, lodepng_error_text(Error));
        return false;
    }
    for (unsigned int y = 0; y < Height; y++)
        for (unsigned int x = 0; x < Width; x++)
        {
            Data[y][x].r = Buffer[4 * y * Width + 4 * x + 0];
            Data[y][x].g = Buffer[4 * y * Width + 4 * x + 1];
            Data[y][x].b = Buffer[4 * y * Width + 4 * x + 2];
            Data[y][x].a = Buffer[4 * y * Width + 4 * x + 3];
        }
    delete Buffer;
    return true;
}
