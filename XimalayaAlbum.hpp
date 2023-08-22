#pragma once
#include "Ximalaya.hpp"
class XIMALAYA::ALBUM
{
private:
    int AlbumID;
    int TrackTotalCount;
    int PageSize;

public:
    void SetID(int AlbumID);
    void GetInfo();
    int GetTrackTotalCount();
    int GetTrackIDByIndex(int Index);
};
