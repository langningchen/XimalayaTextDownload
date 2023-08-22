#pragma once
#include <set>
#include "Picture.hpp"
class CAPTCHA
{
private:
    PICTURE JPEGPicture;
    PICTURE PNGPicture;

public:
    void SetBackgroundPicture(PICTURE &Picture);
    void SetForegroundPicture(PICTURE &Picture);
    int Solve();
};
