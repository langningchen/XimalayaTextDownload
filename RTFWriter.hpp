#pragma once
#include <string>
#include <ostream>
using namespace std;
class RTF_WRITER
{
private:
    string Title;
    string Content;
    string FileName;
    wstring StringToWString(string Input);
    string EncodeWString(wstring Input);

public:
    void SetTitle(string Title);
    void SetContent(string Content);
    void WriteToFile(string FileName);
};
