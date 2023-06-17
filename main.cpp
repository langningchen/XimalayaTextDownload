#include "Curl.hpp"
#include "lodepng.cpp"
#include "jpeg/jpeglib.h"
#include "SHA1.hpp"
#include "MD5.hpp"
#include <locale>
extern string CurrentDir;
const string WEBSign = "WEB-V1-PRODUCT-E7768904917C4154A925FBE1A3848BC3E84E2C7770744E56AFBC9600C267891F";
const string LinuxFilePath = "/mnt/d/12_Mom/";
const string WindowsFilePath = "D:\\12_Mom\\";
const long long INF = 0x7FFF'FFFF'FFFF'FFFF;
struct PICTURE
{
    struct PIXEL
    {
        unsigned char r, g, b, a;
    };
    PIXEL Data[300][600];
    unsigned int Width, Height;
};
PICTURE ReadJPEGFile(string FileName)
{
    PICTURE Picture;
    jpeg_decompress_struct cinfo;
    jpeg_error_mgr JPEGError;
    cinfo.err = jpeg_std_error(&JPEGError);
    jpeg_create_decompress(&cinfo);

    FILE *InputFilePointer = fopen(FileName.c_str(), "rb");
    if (InputFilePointer == NULL)
    {
        cout << "can't open " << FileName << endl;
        exit(0);
    }
    jpeg_stdio_src(&cinfo, InputFilePointer);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);
    Picture.Width = cinfo.output_width;
    Picture.Height = cinfo.output_height;
    unsigned char *Buffer = (unsigned char *)malloc(cinfo.output_width * cinfo.output_components);
    unsigned int i = 0;
    while (cinfo.output_scanline < cinfo.output_height)
    {
        jpeg_read_scanlines(&cinfo, &Buffer, 1);
        for (unsigned int j = 0; j < cinfo.output_width; j++)
        {
            Picture.Data[i][j].r = Buffer[j * 3];
            Picture.Data[i][j].g = Buffer[j * 3 + 1];
            Picture.Data[i][j].b = Buffer[j * 3 + 2];
        }
        i++;
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(InputFilePointer);
    free(Buffer);
    return Picture;
}
PICTURE ReadPNGFile(string FileName)
{
    PICTURE Picture;
    unsigned char *Buffer = NULL;
    unsigned int Error = lodepng_decode32_file(&Buffer, &Picture.Width, &Picture.Height, FileName.c_str());
    if (Error)
    {
        printf("error %u: %s\n", Error, lodepng_error_text(Error));
    }
    for (unsigned int y = 0; y < Picture.Height; y++)
        for (unsigned int x = 0; x < Picture.Width; x++)
        {
            Picture.Data[y][x].r = Buffer[4 * y * Picture.Width + 4 * x + 0];
            Picture.Data[y][x].g = Buffer[4 * y * Picture.Width + 4 * x + 1];
            Picture.Data[y][x].b = Buffer[4 * y * Picture.Width + 4 * x + 2];
            Picture.Data[y][x].a = Buffer[4 * y * Picture.Width + 4 * x + 3];
        }
    delete Buffer;
    return Picture;
}
string GetCurrentTime()
{
    GetDataToFile("https://www.ximalaya.com/revision/time");
    return GetDataFromFileToString();
}
string GetNonce()
{
    GetDataToFile("https://passport.ximalaya.com/web/nonce/" + GetCurrentTime());
    json NonceJSON = json::parse(GetDataFromFileToString());
    if (NonceJSON["ret"] != 0)
    {
        cout << "获得随机数值失败 " << NonceJSON["msg"] << endl;
        exit(0);
    }
    return NonceJSON["nonce"].as_string();
}
string ToString36(int Input)
{
    string Output = "";
    while (Input)
    {
        if (Input % 36 < 10)
            Output += (Input % 36 + '0');
        else
            Output += (Input % 36 - 10 + 'A');
        Input /= 36;
    }
    reverse(Output.begin(), Output.end());
    return Output;
}
string UpperCase(string Input)
{
    for (unsigned int i = 0; i < Input.length(); i++)
        if (Input[i] >= 'a' && Input[i] <= 'z')
            Input[i] -= 32;
    return Input;
}
curl_slist *GetBasicHeaderList(string Host = "www", string Referer = "https://www.ximalaya.com/", bool AddXM_SIGN = false)
{
    curl_slist *HeaderList = NULL;
    HeaderList = curl_slist_append(HeaderList, string("Host: " + Host + ".ximalaya.com").c_str());
    HeaderList = curl_slist_append(HeaderList, "Accept: */*");
    HeaderList = curl_slist_append(HeaderList, "Accept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2");
    HeaderList = curl_slist_append(HeaderList, "Accept-Encoding: deflate, br");
    HeaderList = curl_slist_append(HeaderList, string("Referer: " + Referer).c_str());
    HeaderList = curl_slist_append(HeaderList, "Origin: https://www.ximalaya.com");
    HeaderList = curl_slist_append(HeaderList, "Connection: keep-alive");
    HeaderList = curl_slist_append(HeaderList, "Sec-Fetch-Dest: empty");
    HeaderList = curl_slist_append(HeaderList, "Sec-Fetch-Mode: cors");
    HeaderList = curl_slist_append(HeaderList, "Sec-Fetch-Site: same-site");
    HeaderList = curl_slist_append(HeaderList, "Pragma: no-cache");
    HeaderList = curl_slist_append(HeaderList, "Cache-Control: no-cache");
    if (AddXM_SIGN)
    {
        string CurrentTime = GetCurrentTime();
        MD5 MD5Encoder;
        HeaderList = curl_slist_append(HeaderList, string("xm-sign: {" + MD5Encoder.encode("himalaya-" + CurrentTime) + "}(" + to_string(rand() % 100) + ")" + CurrentTime + "(" + to_string(rand() % 100) + ")" + to_string(time(NULL))).c_str());
    }
    return HeaderList;
}
wstring StringToWString(string Input)
{
    setlocale(LC_CTYPE, "");
    wchar_t *Buffer = new wchar_t[Input.size() * 2];
    mbstowcs(Buffer, Input.c_str(), Input.size() * 2);
    wstring Output(Buffer);
    delete[] Buffer;
    return Output;
}
string EncodeWString(wstring Input)
{
    string Output;
    for (size_t i = 0; i < Input.size(); i++)
        if (Input[i] <= 0x7f)
            Output.push_back((char)Input[i]);
        else
            Output += "\\u" + to_string((int)Input[i]) + "?";
    return Output;
}
int main()
{
    GetDataToFile("https://www.ximalaya.com/revision/my/getCurrentUserInfo", "Header.tmp", "Body.tmp", false, "", GetBasicHeaderList("www", "https://www.ximalaya.com/my/subscribed", true));
    if (json::parse(GetDataFromFileToString())["ret"].as_integer() != 200)
    {
        bool CaptchaSuccess = false;
        unsigned short CaptchaCounter = 0;
        json CaptchaResultJSON;
        while (!CaptchaSuccess)
        {
            if (CaptchaCounter == 5)
            {
                cout << "多次失败，请重新运行程序重试" << endl;

                getchar();
                getchar();
                return 0;
            }
            string CaptchaSessionId = "xm_" + ToString36(time(NULL)) + "000000";
            GetDataToFile("https://mobile.ximalaya.com/captcha-web/check/slide/get?bpId=139&sessionId=" + CaptchaSessionId);
            json CaptchaJSON = json::parse(GetDataFromFileToString());
            if (!CaptchaJSON["msg"].is_null())
            {
                cout << "获得验证码失败 " << CaptchaJSON["msg"].as_string() << endl;
                exit(0);
            }
            GetDataToFile(CaptchaJSON["data"]["bgUrl"].as_string(), "Header.tmp", "Captcha-Background.jpg");
            GetDataToFile(CaptchaJSON["data"]["fgUrl"].as_string(), "Header.tmp", "Captcha-Foreground.png");
            PICTURE JPEGPicture = ReadJPEGFile(CurrentDir + "Captcha-Background.jpg");
            PICTURE PNGPicture = ReadPNGFile(CurrentDir + "Captcha-Foreground.png");
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
            json CaptchaSubmitJSON;
            CaptchaSubmitJSON["bpId"] = 139;
            CaptchaSubmitJSON["sessionId"] = CaptchaSessionId;
            CaptchaSubmitJSON["startTime"] = GetCurrentTime();
            CaptchaSubmitJSON["startX"] = 673;
            CaptchaSubmitJSON["startY"] = 148;
            CaptchaSubmitJSON["type"] = "slider";
            CaptchaSubmitJSON["captchaText"] = to_string(SlideLength + 30) + ",0";
            GetDataToFile("https://mobile.ximalaya.com/captcha-web/valid/slider", "Header.tmp", "Body.tmp", true, CaptchaSubmitJSON.dump());
            CaptchaResultJSON = json::parse(GetDataFromFileToString());
            if (CaptchaResultJSON["msg"].as_string() != "captcha check success")
            {
                cout << "验证码校验失败 " << CaptchaResultJSON["msg"].as_string() << endl
                     << "匹配到的滑块位置为 " << SlideLength << endl
                     << "正在重试" << endl;
                CaptchaCounter++;
            }
            else
                CaptchaSuccess = true;
            remove(string(CurrentDir + "Captcha-Background.jpg").c_str());
            remove(string(CurrentDir + "Captcha-Foreground.png").c_str());
        }
        string PhoneNumber = GetDataFromFileToString("Keys/PhoneNumber2");
        string Nonce = GetNonce();
        json AuthJSON;
        AuthJSON["mobile"] = PhoneNumber;
        AuthJSON["nonce"] = Nonce;
        AuthJSON["sendType"] = 1;
        AuthJSON["signature"] = SHA1(UpperCase("mobile=" + PhoneNumber + "&nonce=" + Nonce + "&sendType=1&" + WEBSign));
        GetDataToFile("https://passport.ximalaya.com/web/sms/send", "Header.tmp", "Body.tmp", true, AuthJSON.dump(), GetBasicHeaderList("passport"), NULL, "application/json", ".ximalaya.com\tTRUE\t/\tFALSE\t0\tfds_otp\t" + CaptchaResultJSON["token"].as_string());
        json AuthJSONResponse = json::parse(GetDataFromFileToString());
        if (AuthJSONResponse["ret"].as_integer() != 0)
        {
            cout << "验证码发送失败 " << AuthJSONResponse["msg"].as_string() << endl;

            getchar();
            getchar();
            return 0;
        }
        bool CheckCodeSuccess = false;
        json CheckCodeJSONResponse;
        while (!CheckCodeSuccess)
        {
            string Code;
            cout << "请输入验证码：";
            cin >> Code;
            Nonce = GetNonce();
            json CheckCodeJSON;
            CheckCodeJSON["code"] = Code;
            CheckCodeJSON["mobile"] = PhoneNumber;
            CheckCodeJSON["nonce"] = Nonce;
            CheckCodeJSON["signature"] = SHA1(UpperCase("code=" + Code + "&mobile=" + PhoneNumber + "&nonce=" + Nonce + "&" + WEBSign));
            GetDataToFile("https://passport.ximalaya.com/web/sms/verify", "Header.tmp", "Body.tmp", true, CheckCodeJSON.dump(), GetBasicHeaderList("passport"));
            CheckCodeJSONResponse = json::parse(GetDataFromFileToString());
            if (CheckCodeJSONResponse["ret"].as_integer() != 0)
                cout << "验证码校验失败 " << CheckCodeJSONResponse["msg"].as_string() << endl;
            else
                CheckCodeSuccess = true;
        }
        json LoginJSON;
        Nonce = GetNonce();
        LoginJSON["mobile"] = PhoneNumber;
        LoginJSON["nonce"] = Nonce;
        LoginJSON["smsKey"] = CheckCodeJSONResponse["bizKey"].as_string();
        LoginJSON["signature"] = SHA1(UpperCase("mobile=" + PhoneNumber + "&nonce=" + Nonce + "&smsKey=" + CheckCodeJSONResponse["bizKey"].as_string() + "&" + WEBSign));
        GetDataToFile("https://passport.ximalaya.com/web/login/quick/v1", "Header.tmp", "Body.tmp", true, LoginJSON.dump(), GetBasicHeaderList("passport"));
        json LoginJSONResponse = json::parse(GetDataFromFileToString());
        if (LoginJSONResponse["ret"].as_integer() != 0)
        {
            cout << "登录失败 " << LoginJSONResponse["msg"].as_string() << endl;

            getchar();
            getchar();
            return 0;
        }
    }
    string AlbumID = "43254755";
    GetDataToFile("https://www.ximalaya.com/revision/album/v1/getTracksList?albumId=" + AlbumID + "&pageNum=1", "Header.tmp", "Body.tmp", false, "", GetBasicHeaderList("www", "https://www.ximalaya.com/album/" + AlbumID, true));
    json AlbumJSONResponse = json::parse(GetDataFromFileToString());
    if (AlbumJSONResponse["ret"].as_integer() != 200)
    {
        cout << "获取合集信息失败 " << AlbumJSONResponse["msg"] << endl;

        getchar();
        getchar();
        return 0;
    }
    int TrackIndex;
    cout << "请输入要下载的音频序号：";
    cin >> TrackIndex;
    if (TrackIndex < 1 || TrackIndex > AlbumJSONResponse["data"]["trackTotalCount"])
    {
        cout << "请输入大于1小于" << AlbumJSONResponse["data"]["trackTotalCount"] << "的数字" << endl;

        getchar();
        getchar();
        return 0;
    }
    GetDataToFile("https://www.ximalaya.com/revision/album/v1/getTracksList?albumId=" + AlbumID + "&pageNum=" + to_string(TrackIndex / AlbumJSONResponse["data"]["pageSize"].as_integer() + 1), "Header.tmp", "Body.tmp", false, "", GetBasicHeaderList("www", "https://www.ximalaya.com/album/" + AlbumID, true));
    AlbumJSONResponse = json::parse(GetDataFromFileToString());
    if (AlbumJSONResponse["ret"].as_integer() != 200)
    {
        cout << "获取合集信息失败 " << AlbumJSONResponse["msg"].as_string() << endl;

        getchar();
        getchar();
        return 0;
    }
    string TrackID = AlbumJSONResponse["data"]["tracks"][TrackIndex % AlbumJSONResponse["data"]["pageSize"].as_integer() - 1]["trackId"].as_string();
    GetDataToFile("https://www.ximalaya.com/revision/track/simple?trackId=" + TrackID, "Header.tmp", "Body.tmp", false, "", GetBasicHeaderList("www", "https://www.ximalaya.com/sound/" + TrackID, true));
    json TrackJSONResponse = json::parse(GetDataFromFileToString());
    if (TrackJSONResponse["ret"].as_integer() != 200)
    {
        cout << "获取音频信息失败 " << TrackJSONResponse["msg"] << endl;

        getchar();
        getchar();
        return 0;
    }
    string Content = TrackJSONResponse["data"]["trackInfo"]["richIntro"].as_string();
    Content = StringReplaceAll(Content, "\r", "");
    Content = StringReplaceAll(Content, "</p>", "\n");
    Content = StringReplaceAll(Content, "<br>", "\n");
    Content = EraseHTMLElement(Content);
    Content = StringReplaceAll(Content, "\n\n", "\n");
    while (Content[0] == '\n' && Content.size() > 0)
        Content.erase(0, 1);
    while (Content[Content.size() - 1] == '\n' && Content.size() > 0)
        Content.erase(Content.size() - 1, 1);
    size_t LastPos = 0;
    string RTFTitle = TrackJSONResponse["data"]["trackInfo"]["title"].as_string();
    RTFTitle = StringReplaceAll(RTFTitle, "<", "");
    RTFTitle = StringReplaceAll(RTFTitle, ">", "");
    RTFTitle = StringReplaceAll(RTFTitle, "|", "");
    RTFTitle = StringReplaceAll(RTFTitle, "\"", "");
    RTFTitle = StringReplaceAll(RTFTitle, "/", "");
    RTFTitle = StringReplaceAll(RTFTitle, "\\", "");
    RTFTitle = StringReplaceAll(RTFTitle, ":", "");
    RTFTitle = StringReplaceAll(RTFTitle, "*", "");
    RTFTitle = StringReplaceAll(RTFTitle, "?", "");
    ofstream OutputFileStream(LinuxFilePath + RTFTitle + ".rtf");
    if (!OutputFileStream.is_open())
    {
        cout << "打开输出文件失败" << endl;

        getchar();
        getchar();
        return 0;
    }
    OutputFileStream << "{\\rtf1\\paperw12240\\paperh15840\\margl720\\margr720\\margt360\\margb360" << endl
                     << EncodeWString(StringToWString(TrackJSONResponse["data"]["trackInfo"]["title"].as_string())) << endl;
    for (size_t Pos = 0; Pos < Content.size(); Pos++)
        if (Content[Pos] == '\n')
        {
            OutputFileStream << "\\par" << EncodeWString(StringToWString(Content.substr(LastPos, Pos - LastPos))) << endl;
            LastPos = Pos + 1;
        }
    OutputFileStream << "\\par}" << endl;
    OutputFileStream.close();
    if (system(string("/mnt/c/Program\\ Files/Microsoft\\ Office/root/Office16/WINWORD.EXE \"" + WindowsFilePath + RTFTitle + ".rtf\"").c_str()))
    {
        cout << "执行失败" << endl;
        return 0;
    }

    return 0;
}
