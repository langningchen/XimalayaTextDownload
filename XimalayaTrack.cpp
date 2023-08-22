#include "XimalayaTrack.hpp"

void XIMALAYA::TRACK::SetID(int TrackID)
{
    this->TrackID = TrackID;
}
void XIMALAYA::TRACK::GetInfo()
{
    cout << "正在获取音频信息……" << flush;
    GetDataToFile("https://www.ximalaya.com/revision/track/simple?trackId=" + to_string(TrackID), "", "", false, "",
                  GetBasicHeaderList("www", "https://www.ximalaya.com/sound/" + to_string(TrackID), true));
    json TrackJSONResponse = json::parse(GetDataFromFileToString());
    if (TrackJSONResponse["ret"].as_integer() != 200)
        TRIGGER_ERROR("获取音频信息失败 " + TrackJSONResponse["msg"].as_string());
    RichIntroduction = TrackJSONResponse["data"]["trackInfo"]["richIntro"].as_string();
    Title = TrackJSONResponse["data"]["trackInfo"]["title"].as_string();
    cout << "成功" << endl;
}
string XIMALAYA::TRACK::GetRichIntroduction()
{
    return RichIntroduction;
}
string XIMALAYA::TRACK::GetTitle()
{
    return Title;
}
void XIMALAYA::TRACK::GetVideoInfo()
{
    cout << "正在获取音频视频信息……" << flush;
    GetDataToFile("https://mpay.ximalaya.com/mobile/track/pay/video/" + to_string(TrackID) + "/" + to_string(time(NULL)) + "000?device=pc&isBackend=false&isDownload=true&videoQualityLevel=1&_=" + to_string(time(NULL)) + "000");
    json VideoJSONResponse = json::parse(GetDataFromFileToString());
    cout << "成功" << endl;
    cout << "正在解密视频地址……" << flush;
    int RandomSeed = VideoJSONResponse["seed"].as_integer();
    string EncryptTable = "";
    string SourceTable = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ/\\:._-1234567890";
    string EncryptedFileName = VideoJSONResponse["fileId"].as_string();
    string FileName = "";
    while (SourceTable.length() > 0)
    {
        RandomSeed = (RandomSeed * 211 + 30031) % 65536;
        int Index = RandomSeed / 65536.0 * SourceTable.length();
        EncryptTable += SourceTable[Index];
        SourceTable = SourceTable.erase(Index, 1);
    }
    auto SpiltEncryptedFileName = SpiltString(EncryptedFileName, "*");
    for (int i = 0; i < SpiltEncryptedFileName.size() - 1; i++)
        FileName += EncryptTable[stoi(SpiltEncryptedFileName[i])];

    auto a = [&](string e, string t)
    {
        int r[256];
        for (int i = 0; i < 256; i++)
            r[i] = i;
        int o = 0;
        for (int i = 0; i < 256; i++)
        {
            o = (o + r[i] + e[i % e.length()]) % 256;
            swap(r[i], r[o]);
        }
        o = 0;
        string a = "";
        int i = 0;
        for (int c = 0; c < t.length(); c++)
        {
            o = (o + r[i = (i + 1) % 256]) % 256;
            swap(r[i], r[o]);
            a.push_back(t[c] ^ r[(r[i] + r[o]) % 256]);
        }
        return a;
    };
    auto f = [&]()
    {
        string e = VideoJSONResponse["ep"].as_string();
        int o = e.length();
        string a = "";
        int r = 0;
        int i[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                   -1, -1, -1, 62, -1, -1, -1, 63, 52, 53,
                   54, 55, 56, 57, 58, 59, 60, 61, -1, -1,
                   -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6,
                   7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
                   18, 19, 20, 21, 22, 23, 24, 25, -1, -1,
                   -1, -1, -1, -1, 26, 27, 28, 29, 30, 31,
                   32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
                   42, 43, 44, 45, 46, 47, 48, 49, 50, 51,
                   -1, -1, -1, -1, -1};
        while (r < o)
        {
            int t;
            do
            {
                t = i[255 & e[r++]];
            } while (r < o && t == -1);
            if (t == -1)
                break;
            int n;
            do
            {
                n = i[255 & e[r++]];
            } while (r < o && n == -1);
            if (n == -1)
                break;
            a.push_back(t << 2 | (48 & n) >> 4);
            do
            {
                if (61 == (t = 255 & e[r++]))
                    return a;
                t = i[t];
            } while (r < o && -1 == t);
            if (t == -1)
                break;
            a.push_back((15 & n) << 4 | (60 & t) >> 2);
            do
            {
                if (61 == (n = 255 & e[r++]))
                    return a;
                n = i[n];
            } while (r < o && -1 == n);
            if (n == -1)
                break;
            a.push_back((3 & t) << 6 | n);
        }
        return a;
    };
    vector<string> t = SpiltString(a("xkt3a41psizxrh9l", f()), "-");
    if (t.size() != 4)
        TRIGGER_ERROR("解密视频地址失败");
    cout << "成功" << endl;
    VideoURL = VideoJSONResponse["domain"].as_string() + "/download/" + VideoJSONResponse["apiVersion"].as_string() + "/" + FileName + "?sign=" + t[1] + "&buy_key=" + t[0] + "&token=" + t[2] + "&timestamp=" + t[3] + "&duration=" + VideoJSONResponse["duration"].as_string();
}
string XIMALAYA::TRACK::GetVideoURL()
{
    return VideoURL;
}
