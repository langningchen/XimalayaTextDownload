#include "Ximalaya.hpp"
#include "XimalayaAlbum.hpp"
#include "XimalayaTrack.hpp"
#include "RTFWriter.hpp"
int main()
{
    CLN_TRY
    XIMALAYA Ximalaya;
    if (!Ximalaya.IsLoggedIn())
    {
        Ximalaya.SendSMSVerifyCode("18001921393");
        string Code;
        do
        {
            cout << "请输入验证码：";
            cin >> Code;
        } while (!Ximalaya.LoginBySMS(Code));
    }

    XIMALAYA::ALBUM Album;
    Album.SetID(43254755);
    Album.GetInfo();

    // int TrackIndex;
    // cout << "请输入要下载的音频序号：";
    // cin >> TrackIndex;
    // if (TrackIndex < 1 || TrackIndex > Album.GetTrackTotalCount())
    //     TRIGGER_ERROR("请输入大于1小于" + to_string(Album.GetTrackTotalCount()) + "的数字");
    int StartIndex = 1;
    cout << "请输入要下载的音频序号起始值：";
    cin >> StartIndex;
    if (StartIndex < 1 || StartIndex > Album.GetTrackTotalCount())
        TRIGGER_ERROR("请输入大于1小于" + to_string(Album.GetTrackTotalCount()) + "的数字");
    for (int TrackIndex = StartIndex; TrackIndex <= Album.GetTrackTotalCount(); TrackIndex++)
    {
        XIMALAYA::TRACK Track;
        Track.SetID(Album.GetTrackIDByIndex(TrackIndex));
        Track.GetInfo();
        Track.GetVideoInfo();

        string FixedTitle = Track.GetTitle();
        FixedTitle = StringReplaceAll(FixedTitle, "<", "");
        FixedTitle = StringReplaceAll(FixedTitle, ">", "");
        FixedTitle = StringReplaceAll(FixedTitle, "|", "");
        FixedTitle = StringReplaceAll(FixedTitle, "\"", "");
        FixedTitle = StringReplaceAll(FixedTitle, "/", "");
        FixedTitle = StringReplaceAll(FixedTitle, "\\", "");
        FixedTitle = StringReplaceAll(FixedTitle, ":", "");
        FixedTitle = StringReplaceAll(FixedTitle, "*", "");
        FixedTitle = StringReplaceAll(FixedTitle, "?", "");

        string VideoURL = Track.GetVideoURL();
        string FileName = "/mnt/c/Data/" + FixedTitle + ".mp4";
        cout << "正在下载：" << FileName << ": " << VideoURL << endl;
        GetDataToFile(VideoURL, "", FileName, false, "", NULL, NULL, "application/json", "", true);
        // system("mocp -l Download\\ succeeds.mp3");
    }

    // RTF_WRITER RTFWriter;
    // RTFWriter.SetTitle(Track.GetTitle());
    // RTFWriter.SetContent(Track.GetRichIntroduction());
    // RTFWriter.WriteToFile("/mnt/c/Data/" + FixedTitle + ".rtf");
    system("mocp -l Download\\ failed.mp3");
    CLN_CATCH
    return 0;
}
