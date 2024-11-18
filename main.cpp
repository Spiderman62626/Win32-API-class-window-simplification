#define byte BYTE
#define WINVER 0x0602
#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cctype>
#include <locale>
#include <codecvt>
#include <unordered_map>

struct node
{
    std::string name;
    std::unordered_map<std::string, std::vector<std::wstring>> value;
    std::vector<std::shared_ptr<node>> children;
    node(const std::string &nodeName)
        : name(nodeName) {}
    void newc(std::shared_ptr<node> child)
    {
        children.push_back(child);
    }
};

namespace translate
{
    std::wstring stow(const std::string &str)
    {
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), NULL, 0);
        std::wstring wstr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.size()), &wstr[0], size_needed);
        return wstr;
    }
    std::string wtos(const std::wstring &wstr)
    {
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
        std::string str(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &str[0], size_needed, nullptr, nullptr);
        return str;
    }
    std::wstring readfile()
    {
        std::ifstream file("input.txt", std::ios::binary);
        std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        std::wstring text;
        text.reserve(buffer.size() / 2);
        for (size_t i = 0; i < buffer.size(); i += 2)
        {
            wchar_t wchar = *reinterpret_cast<wchar_t *>(&buffer[i]);
            text += wchar;
        }
        return text;
    }
    std::shared_ptr<node> nodify(std::wstring s)
    {
        std::shared_ptr<node> result;
        std::vector<std::shared_ptr<node>> parents;
        std::string sel;
        std::vector<std::wstring> val;
        bool swap = false, space = false, quote = false;
        int count = 0;
        for (const auto &i : s)
        {
            if ((quote && i != 34) || std::iswalpha(i) || std::iswdigit(i) || i > 127 || i == L'.' || i == L'/' || (i == L' ' && space))
            {
                if (swap)
                {
                    if (count >= static_cast<int>(val.size()))
                        val.resize(count + 1);
                    val[count] += i;
                }
                else
                {
                    sel += wtos(std::wstring(1, i));
                }
                space = true;
            }
            else if (i == L'{')
            {
                std::shared_ptr<node> next = std::make_shared<node>(sel);
                if (parents.empty())
                {
                    result = next;
                    parents.push_back(next);
                }
                else
                {
                    parents.back()->newc(next);
                    parents.push_back(next);
                }
                sel.clear();
                val.clear();
                space = false;
            }
            else if (i == L'}' && !parents.empty())
                parents.pop_back();
            else if (i == L':')
            {
                swap = true;
                space = false;
            }
            else if (i == L',' || i == L'/')
            {
                ++count;
                space = false;
            }
            else if (i == L';')
            {
                if (!sel.empty() && !val.empty())
                    parents.back()->value.insert({std::move(sel), std::move(val)});
                swap = space = false;
                sel.clear();
                val.clear();
                count = 0;
            }
            else if (i == L'"')
                quote ^= 1;
        }
        return result;
    }
};

namespace winprops
{
    struct pair_hash
    {
        template <typename T1, typename T2>
        std::size_t operator()(const std::pair<T1, T2> &p) const
        {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };
    std::unordered_map<std::pair<std::string, std::string>, HWND, winprops::pair_hash> hWnds;
    inline static const std::unordered_map<std::string, DWORD> cstyles = {{"doubleclicks", CS_DBLCLKS}, {"horizontalredraw", CS_HREDRAW}, {"verticalredraw", CS_VREDRAW}, {"owndevicecontext", CS_OWNDC}, {"classdevicecontext", CS_CLASSDC}, {"noclose", CS_NOCLOSE}, {"savebits", CS_SAVEBITS}, {"parentdevicecontext", CS_PARENTDC}, {"globalclass", CS_GLOBALCLASS}, {"dropshadow", CS_DROPSHADOW}, {"inputmethodeditor", CS_IME}}, wstyles = {{"overlapped", WS_OVERLAPPED}, {"popup", WS_POPUP}, {"child", WS_CHILD}, {"minimize", WS_MINIMIZE}, {"visible", WS_VISIBLE}, {"disabled", WS_DISABLED}, {"clipsiblings", WS_CLIPSIBLINGS}, {"clipchildren", WS_CLIPCHILDREN}, {"maximize", WS_MAXIMIZE}, {"caption", WS_CAPTION}, {"border", WS_BORDER}, {"dialogframe", WS_DLGFRAME}, {"verticalscroll", WS_VSCROLL}, {"horizontalscroll", WS_HSCROLL}, {"systemmenu", WS_SYSMENU}, {"thickframe", WS_THICKFRAME}, {"group", WS_GROUP}, {"tabstop", WS_TABSTOP}, {"minimizebox", WS_MINIMIZEBOX}, {"maximizebox", WS_MAXIMIZEBOX}, {"tiled", WS_TILED}, {"iconic", WS_ICONIC}, {"sizebox", WS_SIZEBOX}, {"tiledwindow", WS_TILEDWINDOW}, {"overlappedwindow", WS_OVERLAPPEDWINDOW}, {"popupwindow", WS_POPUPWINDOW}, {"childwindow", WS_CHILDWINDOW}}, wexstyles = {{"dialogmodalframe", WS_EX_DLGMODALFRAME}, {"noparentnotify", WS_EX_NOPARENTNOTIFY}, {"topmost", WS_EX_TOPMOST}, {"acceptfiles", WS_EX_ACCEPTFILES}, {"transparent", WS_EX_TRANSPARENT}, {"multidocumentinterfacechild", WS_EX_MDICHILD}, {"toolwindow", WS_EX_TOOLWINDOW}, {"windowedge", WS_EX_WINDOWEDGE}, {"clientedge", WS_EX_CLIENTEDGE}, {"contexthelp", WS_EX_CONTEXTHELP}, {"rightaligned", WS_EX_RIGHT}, {"leftaligned", WS_EX_LEFT}, {"righttoleftreading", WS_EX_RTLREADING}, {"lefttorightreading", WS_EX_LTRREADING}, {"leftscrollbar", WS_EX_LEFTSCROLLBAR}, {"rightscrollbar", WS_EX_RIGHTSCROLLBAR}, {"controlparent", WS_EX_CONTROLPARENT}, {"staticedge", WS_EX_STATICEDGE}, {"appwindow", WS_EX_APPWINDOW}, {"palettewindow", WS_EX_PALETTEWINDOW}, {"layered", WS_EX_LAYERED}, {"noinheritlayout", WS_EX_NOINHERITLAYOUT}, {"layoutwithrighttoleft", WS_EX_LAYOUTRTL}, {"composited", WS_EX_COMPOSITED}, {"noactivate", WS_EX_NOACTIVATE}, {"noredirectionbitmap", WS_EX_NOREDIRECTIONBITMAP}};
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CLOSE)
    {
        DestroyWindow(hWnd);
    }
    else if (message == WM_DESTROY)
    {
        PostQuitMessage(0);
    }
    else
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

class windowmanager
{
private:
    HINSTANCE hInstance;
    std::wstring classname, title, icon, smicon, cursor;
    std::string cid, wid;
    DWORD wstyle = 0, wexstyle = 0, cstyle = 0;
    int width = CW_USEDEFAULT, height = CW_USEDEFAULT, xpos = CW_USEDEFAULT, ypos = CW_USEDEFAULT, extracb = 0, extrawb = 0;
    void regclass()
    {
        WNDCLASSEXW wc = {0};
        wc.cbClsExtra = extracb;
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.cbWndExtra = extrawb;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInstance;
        wc.hIcon = (HICON)LoadImageW(NULL, icon.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
        wc.hIconSm = (HICON)LoadImageW(NULL, smicon.c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
        wc.hCursor = (HCURSOR)LoadImageW(NULL, cursor.c_str(), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = classname.c_str();
        wc.style = cstyle;
        RegisterClassExW(&wc);
    }
    void makewindow()
    {
        HWND hWnd = CreateWindowExW(wexstyle, classname.c_str(), title.c_str(), wstyle, xpos, ypos, width, height, NULL, NULL, hInstance, NULL);
        winprops::hWnds[{cid, wid}] = hWnd;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        ShowWindow(hWnd, SW_SHOWNORMAL);
        UpdateWindow(hWnd);
    }

public:
    windowmanager(HINSTANCE instance) : hInstance(instance) {}
    void init(const std::shared_ptr<node> &root)
    {
        for (const auto &cls : root->children)
        {
            cid = cls->name;
            classname = cls->value["classname"][0];
            extracb = std::stoi(translate::wtos(cls->value["extracb"][0]));
            extrawb = std::stoi(translate::wtos(cls->value["extrawb"][0]));
            icon = L"resources/" + cls->value["icon"][0];
            smicon = L"resources/" + cls->value["iconsm"][0];
            cursor = L"resources/" + cls->value["cursor"][0];
            for (const auto &l : cls->value["styles"])
            {
                if (winprops::wstyles.find(translate::wtos(l)) != winprops::wstyles.end())
                    cstyle |= winprops::cstyles.at(translate::wtos(l));
            }
            regclass();
            for (const auto &wdw : cls->children)
            {
                wid = wdw->name;
                title = wdw->value["title"][0];
                for (const auto &l : wdw->value["styles"])
                {
                    if (winprops::wstyles.find(translate::wtos(l)) != winprops::wstyles.end())
                    {
                        wstyle |= winprops::wstyles.at(translate::wtos(l));
                    }
                    else if (winprops::wexstyles.find(translate::wtos(l)) != winprops::wexstyles.end())
                    {
                        wexstyle |= winprops::wexstyles.at(translate::wtos(l));
                    }
                }
                width = std::stoi(translate::wtos(wdw->value["width"][0]));
                height = std::stoi(translate::wtos(wdw->value["height"][0]));
                xpos = std::stoi(translate::wtos(wdw->value["xpos"][0]));
                ypos = std::stoi(translate::wtos(wdw->value["ypos"][0]));
                makewindow();
            }
        }
    }
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    std::wstring text = translate::readfile();
    std::shared_ptr<node> root = translate::nodify(text);
    std::shared_ptr<windowmanager> windows = std::make_shared<windowmanager>(hInstance);
    windows->init(root);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return static_cast<int>(msg.wParam);
}