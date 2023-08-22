#include "Captcha.hpp"
void CAPTCHA::SetBackgroundPicture(PICTURE &Picture)
{
    JPEGPicture = Picture;
}
void CAPTCHA::SetForegroundPicture(PICTURE &Picture)
{
    PNGPicture = Picture;
}
int CAPTCHA::Solve()
{
    unsigned int PNGPictureBottom = 0;
    for (unsigned int x = PNGPicture.Height - 1; x >= 0; x--)
    {
        for (unsigned int y = 0; y < PNGPicture.Width; y++)
            if (PNGPicture.Data[x][y].a != 0)
            {
                PNGPictureBottom = x;
                break;
            }
        if (PNGPictureBottom != 0)
            break;
    }
    set<pair<long long, unsigned int>, greater<pair<long long, unsigned int>>> Deltas;
    for (unsigned int y = 0; y < JPEGPicture.Width - 62; y++)
    {
        long long Delta = 0;
        for (unsigned int i = 0; i < 62; i++)
        {
            Delta += abs((int)PNGPicture.Data[PNGPictureBottom][14 + i].r - (int)JPEGPicture.Data[PNGPictureBottom][y + i].r);
            Delta += abs((int)PNGPicture.Data[PNGPictureBottom][14 + i].g - (int)JPEGPicture.Data[PNGPictureBottom][y + i].g);
            Delta += abs((int)PNGPicture.Data[PNGPictureBottom][14 + i].b - (int)JPEGPicture.Data[PNGPictureBottom][y + i].b);
        }
        Deltas.insert(make_pair(Delta, y));
    }
    set<pair<long long, unsigned int>>::iterator sit = Deltas.begin();
    unsigned int SlideLength = 0xFFFFFFFF;
    while (SlideLength == 0xFFFFFFFF)
    {
        bool CanBe = true;
        for (unsigned int i = 5; i < 57; i++)
            if ((JPEGPicture.Data[PNGPictureBottom - 1][sit->second + i].r +
                 JPEGPicture.Data[PNGPictureBottom - 1][sit->second + i].g +
                 JPEGPicture.Data[PNGPictureBottom - 1][sit->second + i].b) < 600)
            {
                CanBe = false;
                break;
            }
        if (CanBe)
            SlideLength = sit->second;
        sit++;
        if (sit == Deltas.end())
            SlideLength = (*Deltas.begin()).second;
    }
    return SlideLength;
}
