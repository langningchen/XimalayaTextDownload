#pragma once
#include <iostream>
#include "lodepng.h"
#include "jpeg/jpeglib.h"
using namespace std;
class PICTURE
{
private:
    struct
    {
        unsigned char r, g, b, a;
    } Data[300][600];
    unsigned int Width, Height;

    friend class CAPTCHA;

public:
    bool ReadJPEGFile(string FileName);
    bool ReadPNGFile(string FileName);
};
