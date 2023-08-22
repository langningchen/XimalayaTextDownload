#include "Ximalaya.hpp"
#include "SHA1.hpp"
#include "MD5.hpp"
string XIMALAYA::GetCurrentTime()
{
    GetDataToFile("https://www.ximalaya.com/revision/time");
    return GetDataFromFileToString();
}
string XIMALAYA::GetNonce()
{
    GetDataToFile("https://passport.ximalaya.com/web/nonce/" + GetCurrentTime());
    json NonceJSON = json::parse(GetDataFromFileToString());
    if (NonceJSON["ret"] != 0)
        TRIGGER_ERROR("获得随机数值失败 " + NonceJSON["msg"].as_string());
    return NonceJSON["nonce"].as_string();
}
string XIMALAYA::ToString36(int Input)
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
string XIMALAYA::UpperCase(string Input)
{
    for (unsigned int i = 0; i < Input.length(); i++)
        if (Input[i] >= 'a' && Input[i] <= 'z')
            Input[i] -= 32;
    return Input;
}
curl_slist *XIMALAYA::GetBasicHeaderList(string Host, string Referrer, bool AddXM_SIGN)
{
    curl_slist *HeaderList = NULL;
    HeaderList = curl_slist_append(HeaderList, string("Host: " + Host + ".ximalaya.com").c_str());
    HeaderList = curl_slist_append(HeaderList, "Accept: */*");
    HeaderList = curl_slist_append(HeaderList, "Accept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2");
    HeaderList = curl_slist_append(HeaderList, "Accept-Encoding: deflate, br");
    HeaderList = curl_slist_append(HeaderList, string("Referrer: " + Referrer).c_str());
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
        HeaderList = curl_slist_append(HeaderList, string("xm-sign: {" + MD5Encoder.encode("himalaya-" + CurrentTime) + "}" +
                                                          "(" + to_string(rand() % 100) + ")" +
                                                          CurrentTime +
                                                          "(" + to_string(rand() % 100) + ")" +
                                                          to_string(time(NULL)))
                                                       .c_str());
    }
    return HeaderList;
}

string XIMALAYA::SolveCaptcha()
{
    bool CaptchaSuccess = false;
    unsigned short CaptchaCounter = 0;
    json CaptchaResultJSON;
    while (!CaptchaSuccess)
    {
        if (CaptchaCounter == 5)
        {
            cout << "多次失败，请重新运行程序重试" << endl;
            return 0;
        }
        cout << "正在获取验证码……" << flush;
        string CaptchaSessionId = "xm_" + ToString36(time(NULL)) + "000000";
        GetDataToFile("https://mobile.ximalaya.com/captcha-web/check/slide/get?bpId=139&sessionId=" + CaptchaSessionId);
        json CaptchaJSON = json::parse(GetDataFromFileToString());
        if (!CaptchaJSON["msg"].is_null())
            TRIGGER_ERROR("获得验证码失败 " + CaptchaJSON["msg"].as_string());
        GetDataToFile(CaptchaJSON["data"]["bgUrl"].as_string(), "", "/tmp/Captcha-Background.jpg");
        GetDataToFile(CaptchaJSON["data"]["fgUrl"].as_string(), "", "/tmp/Captcha-Foreground.png");
        cout << "成功" << endl;
        cout << "正在识别验证码……" << flush;
        PICTURE JPEGPicture;
        JPEGPicture.ReadJPEGFile("/tmp/Captcha-Background.jpg");
        PICTURE PNGPicture;
        PNGPicture.ReadPNGFile("/tmp/Captcha-Foreground.png");
        CAPTCHA Captcha;
        Captcha.SetBackgroundPicture(JPEGPicture);
        Captcha.SetForegroundPicture(PNGPicture);
        int SlideLength = Captcha.Solve();
        cout << "成功" << endl;
        cout << "正在提交验证码……" << flush;
        json CaptchaSubmitJSON;
        CaptchaSubmitJSON["bpId"] = 139;
        CaptchaSubmitJSON["sessionId"] = CaptchaSessionId;
        CaptchaSubmitJSON["startTime"] = GetCurrentTime();
        CaptchaSubmitJSON["startX"] = 673;
        CaptchaSubmitJSON["startY"] = 148;
        CaptchaSubmitJSON["type"] = "slider";
        CaptchaSubmitJSON["captchaText"] = to_string(SlideLength + 30) + ",0";
        GetDataToFile("https://mobile.ximalaya.com/captcha-web/valid/slider", "", "", true, CaptchaSubmitJSON.dump());
        CaptchaResultJSON = json::parse(GetDataFromFileToString());
        if (CaptchaResultJSON["msg"].as_string() != "captcha check success")
        {
            cout << "验证码校验失败 " << CaptchaResultJSON["msg"].as_string() << endl
                 << "匹配到的滑块位置为 " << SlideLength << endl
                 << "正在重试" << endl;
            CaptchaCounter++;
        }
        else
        {
            cout << "成功" << endl;
            CaptchaSuccess = true;
        }
        remove("/tmp/Captcha-Background.jpg");
        remove("/tmp/Captcha-Foreground.png");
    }
    return CaptchaResultJSON["token"].as_string();
}
bool XIMALAYA::IsLoggedIn()
{
    cout << "正在检查登录状态……" << flush;
    GetDataToFile("https://www.ximalaya.com/revision/my/getCurrentUserInfo",
                  "", "", false, "",
                  GetBasicHeaderList("www", "https://www.ximalaya.com/my/subscribed", true));
    bool IsLoggedIn = json::parse(GetDataFromFileToString())["ret"].as_integer() == 200;
    cout << (IsLoggedIn ? "已登录" : "未登录") << endl;
    return IsLoggedIn;
}
void XIMALAYA::SendSMSVerifyCode(string PhoneNumber)
{
    string CaptchaToken = SolveCaptcha();
    string Nonce = GetNonce();
    json AuthJSON;
    AuthJSON["mobile"] = PhoneNumber;
    AuthJSON["nonce"] = Nonce;
    AuthJSON["sendType"] = 1;
    AuthJSON["signature"] = SHA1(UpperCase("mobile=" + PhoneNumber + "&nonce=" + Nonce + "&sendType=1&" + WEBSign));
    cout << "正在发送短信验证码……" << flush;
    GetDataToFile("https://passport.ximalaya.com/web/sms/send",
                  "", "", true, AuthJSON.dump(),
                  GetBasicHeaderList("passport"), NULL, "application/json", ".ximalaya.com\tTRUE\t/\tFALSE\t0\tfds_otp\t" + CaptchaToken);
    json AuthJSONResponse = json::parse(GetDataFromFileToString());
    if (AuthJSONResponse["ret"].as_integer() != 0)
        TRIGGER_ERROR("短信验证码发送失败 " + AuthJSONResponse["msg"].as_string());
    cout << "成功" << endl;
    this->PhoneNumber = PhoneNumber;
}
bool XIMALAYA::LoginBySMS(string Code)
{
    string Nonce = GetNonce();
    cout << "正在校验短信验证码……" << flush;
    json CheckCodeJSON;
    CheckCodeJSON["code"] = Code;
    CheckCodeJSON["mobile"] = PhoneNumber;
    CheckCodeJSON["nonce"] = Nonce;
    CheckCodeJSON["signature"] = SHA1(UpperCase("code=" + Code + "&mobile=" + PhoneNumber + "&nonce=" + Nonce + "&" + WEBSign));
    GetDataToFile("https://passport.ximalaya.com/web/sms/verify",
                  "", "", true, CheckCodeJSON.dump(),
                  GetBasicHeaderList("passport"));
    json CheckCodeJSONResponse = json::parse(GetDataFromFileToString());
    if (CheckCodeJSONResponse["ret"].as_integer() != 0)
    {
        cout << "短信验证码校验失败 " << CheckCodeJSONResponse["msg"].as_string() << endl;
        return false;
    }
    cout << "成功" << endl;
    Nonce = GetNonce();
    cout << "正在登录……" << flush;
    json LoginJSON;
    LoginJSON["mobile"] = PhoneNumber;
    LoginJSON["nonce"] = Nonce;
    LoginJSON["smsKey"] = CheckCodeJSONResponse["bizKey"].as_string();
    LoginJSON["signature"] = SHA1(UpperCase("mobile=" + PhoneNumber + "&nonce=" + Nonce + "&smsKey=" + CheckCodeJSONResponse["bizKey"].as_string() + "&" + WEBSign));
    GetDataToFile("https://passport.ximalaya.com/web/login/quick/v1",
                  "", "", true, LoginJSON.dump(),
                  GetBasicHeaderList("passport"));
    json LoginJSONResponse = json::parse(GetDataFromFileToString());
    if (LoginJSONResponse["ret"].as_integer() != 0)
        TRIGGER_ERROR("登录失败 " + LoginJSONResponse["msg"].as_string());
    cout << "成功" << endl;
    return true;
}
