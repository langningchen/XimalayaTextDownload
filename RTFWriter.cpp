#include "RTFWriter.hpp"
#include "Curl.hpp"
wstring RTF_WRITER::StringToWString(string Input)
{
    setlocale(LC_CTYPE, "");
    wchar_t *Buffer = new wchar_t[Input.size() * 2];
    mbstowcs(Buffer, Input.c_str(), Input.size() * 2);
    wstring Output(Buffer);
    delete[] Buffer;
    return Output;
}
string RTF_WRITER::EncodeWString(wstring Input)
{
    string Output;
    for (size_t i = 0; i < Input.size(); i++)
        if (Input[i] < 128)
            Output.push_back((char)Input[i]);
        else
            Output += "\\u" + to_string((int)Input[i]) + "?";
    return Output;
}

void RTF_WRITER::SetTitle(string Title)
{
    this->Title = EncodeWString(StringToWString(Title));
}
void RTF_WRITER::SetContent(string Content)
{
    Content = StringReplaceAll(Content, "\r", "");
    Content = StringReplaceAll(Content, "</p>", "\n");
    Content = StringReplaceAll(Content, "<br>", "\n");
    Content = EraseHTMLElement(Content);
    Content = StringReplaceAll(Content, "\n\n", "\n");
    while (Content[0] == '\n' && Content.size() > 0)
        Content.erase(0, 1);
    while (Content[Content.size() - 1] == '\n' && Content.size() > 0)
        Content.erase(Content.size() - 1, 1);
    this->Content = Content;
}
void RTF_WRITER::WriteToFile(string FileName)
{
    cout << "正在写入RTF文件……" << flush;
    ofstream OutputFileStream(FileName);
    if (!OutputFileStream.is_open())
        TRIGGER_ERROR("打开输出文件失败");
    OutputFileStream << "{\\rtf1\\paperw12240\\paperh15840\\margl720\\margr720\\margt360\\margb360" << endl
                     << Title << endl;
    auto ContentLines = SpiltString(Content, "\n");
    for (auto i : ContentLines)
        OutputFileStream << "\\par" << EncodeWString(StringToWString(i)) << endl;
    OutputFileStream << "\\par}" << endl;
    OutputFileStream.close();
    cout << "成功" << endl;
}
