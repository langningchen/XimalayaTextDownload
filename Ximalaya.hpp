#pragma once
#include "Curl.hpp"
#include "Captcha.hpp"
class XIMALAYA
{
private:
    const string WEBSign = "WEB-V1-PRODUCT-E7768904917C4154A925FBE1A3848BC3E84E2C7770744E56AFBC9600C267891F";
    string PhoneNumber;
    static string GetCurrentTime();
    static string GetNonce();
    string ToString36(int Input);
    string UpperCase(string Input);
    static curl_slist *GetBasicHeaderList(string Host = "www", string Referrer = "https://www.ximalaya.com/", bool AddXM_SIGN = false);
    string SolveCaptcha();

public:
    bool IsLoggedIn();
    void SendSMSVerifyCode(string PhoneNumber);
    bool LoginBySMS(string Code);
    class ALBUM;
    class TRACK;
};
