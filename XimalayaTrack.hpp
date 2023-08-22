#pragma once
#include "Ximalaya.hpp"
class XIMALAYA::TRACK
{
private:
    int TrackID;
    string RichIntroduction;
    string Title;
    string VideoURL;

public:
    void SetID(int TrackID);
    void GetInfo();
    string GetRichIntroduction();
    string GetTitle();
    void GetVideoInfo();
    string GetVideoURL();
};
