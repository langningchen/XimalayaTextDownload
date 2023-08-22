#include "XimalayaAlbum.hpp"

void XIMALAYA::ALBUM::SetID(int AlbumID)
{
    this->AlbumID = AlbumID;
}
void XIMALAYA::ALBUM::GetInfo()
{
    cout << "正在获取合集信息……" << flush;
    GetDataToFile("https://www.ximalaya.com/revision/album/v1/getTracksList?albumId=" + to_string(AlbumID) + "&pageNum=1",
                  "", "", false, "",
                  XIMALAYA::GetBasicHeaderList("www", "https://www.ximalaya.com/album/" + to_string(AlbumID), true));
    json AlbumJSONResponse = json::parse(GetDataFromFileToString());
    if (AlbumJSONResponse["ret"].as_integer() != 200)
        TRIGGER_ERROR("获取合集信息失败 " + AlbumJSONResponse["msg"].as_string());
    TrackTotalCount = AlbumJSONResponse["data"]["trackTotalCount"].as_integer();
    PageSize = AlbumJSONResponse["data"]["pageSize"].as_integer();
    cout << "成功" << endl;
}
int XIMALAYA::ALBUM::GetTrackTotalCount()
{
    return TrackTotalCount;
}
int XIMALAYA::ALBUM::GetTrackIDByIndex(int Index)
{
    cout << "正在获取音频ID……" << flush;
    GetDataToFile("https://www.ximalaya.com/revision/album/v1/getTracksList?albumId=" + to_string(AlbumID) + "&pageNum=" + to_string((Index - 1) / PageSize + 1),
                  "", "", false, "",
                  GetBasicHeaderList("www", "https://www.ximalaya.com/album/" + to_string(AlbumID), true));
    json AlbumJSONResponse = json::parse(GetDataFromFileToString());
    if (AlbumJSONResponse["ret"].as_integer() != 200)
        TRIGGER_ERROR("获取音频ID失败 " + AlbumJSONResponse["msg"].as_string());
    cout << "成功" << endl;
    return AlbumJSONResponse["data"]["tracks"][(Index - 1) % PageSize]["trackId"].as_integer();
}
