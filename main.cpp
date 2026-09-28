#define UNICODE
#define _UNICODE
#include <windows.h>
#include <windowsx.h>
#include <string>
#include <chrono>
#include "engine.h"

using namespace sudoku;
struct State {Board board{};std::array<unsigned,81> notes{};};
Puzzle puzzle; State state;std::vector<State> history;
std::mt19937 rng(std::random_device{}());
int selected=0,difficulty=0;bool dark=true,noteMode=false,won=false;
HWND level;std::wstring status=L"Select a cell and type 1-9. You've got this.";
std::chrono::steady_clock::time_point started;int finalSeconds=0;
constexpr int left=34,top=132,cell=56;
COLORREF bg,ink,panel,muted,accent;
void palette(){bg=dark?RGB(18,24,38):RGB(242,245,250);panel=dark?RGB(29,39,57):RGB(255,255,255);ink=dark?RGB(233,240,250):RGB(26,39,58);muted=dark?RGB(158,174,198):RGB(86,102,125);accent=dark?RGB(112,209,190):RGB(0,116,101);}
void fill(HDC dc,RECT r,COLORREF c){HBRUSH b=CreateSolidBrush(c);FillRect(dc,&r,b);DeleteObject(b);}
void label(HDC dc,const std::wstring& text,RECT r,int size,COLORREF color,bool bold=false,UINT align=DT_LEFT|DT_VCENTER|DT_SINGLELINE){
    HFONT font=CreateFontW(-size,0,0,0,bold?FW_SEMIBOLD:FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Segoe UI");
    auto old=SelectObject(dc,font);SetTextColor(dc,color);SetBkMode(dc,TRANSPARENT);DrawTextW(dc,text.c_str(),-1,&r,align);SelectObject(dc,old);DeleteObject(font);
}
void fresh(){puzzle=generate(rng,difficulty==0?42:difficulty==1?34:28);state={};state.board=puzzle.clues;history.clear();selected=0;won=false;started=std::chrono::steady_clock::now();status=L"New puzzle. Take it one square at a time.";}
void complete(){if(state.board==puzzle.solution){won=true;finalSeconds=int(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now()-started).count());status=L"Beautifully solved! Start a new puzzle to play again.";}}
void enter(int n){if(won)return;if(puzzle.clues[selected]){status=L"That is a starting clue. Choose an empty cell.";return;}
    if(noteMode&&n){if(state.board[selected])return;history.push_back(state);state.notes[selected]^=1u<<n;return;}
    if(n&&!allowed(state.board,selected,n)){status=L"That number already appears in this row, column or box.";return;}
    history.push_back(state);state.board[selected]=n;state.notes[selected]=0;status=L"Use Notes to pencil in possible numbers.";complete();}
void paint(HWND hwnd,HDC dc){RECT client;GetClientRect(hwnd,&client);palette();fill(dc,client,bg);
    label(dc,L"SUDOKU STUDIO",{34,20,550,66},32,ink,true);
    label(dc,L"A little focus. A fresh perspective.",{36,68,570,98},16,muted);
    int secs=won?finalSeconds:int(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now()-started).count());
    wchar_t timer[40];swprintf(timer,40,L"%02d:%02d",secs/60,secs%60);label(dc,timer,{600,30,790,75},30,accent,true);
    for(int p=0;p<81;++p){int r=p/9,c=p%9;RECT box{left+c*cell,top+r*cell,left+(c+1)*cell,top+(r+1)*cell};
        COLORREF color=panel;
        if(r==selected/9||c==selected%9||(r/3==selected/27&&c/3==selected%9/3))color=dark?RGB(37,51,71):RGB(233,242,247);
        if(state.board[p]&&state.board[p]==state.board[selected])color=dark?RGB(43,70,83):RGB(213,236,234);
        if(p==selected)color=dark?RGB(44,102,100):RGB(171,224,214);
        fill(dc,box,color);
        if(state.board[p])label(dc,std::to_wstring(state.board[p]),box,28,puzzle.clues[p]?ink:accent,puzzle.clues[p]!=0,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        else for(int n=1;n<=9;++n)if(state.notes[p]&(1u<<n)){RECT small{box.left+(n-1)%3*18,box.top+(n-1)/3*18,box.left+(n-1)%3*18+20,box.top+(n-1)/3*18+20};label(dc,std::to_wstring(n),small,12,muted,false,DT_CENTER|DT_VCENTER|DT_SINGLELINE);}
    }
    for(int i=0;i<=9;++i){HPEN pen=CreatePen(PS_SOLID,i%3==0?2:1,dark?RGB(88,107,131):RGB(148,165,184));auto old=SelectObject(dc,pen);MoveToEx(dc,left+i*cell,top,nullptr);LineTo(dc,left+i*cell,top+9*cell);MoveToEx(dc,left,top+i*cell,nullptr);LineTo(dc,left+9*cell,top+i*cell);SelectObject(dc,old);DeleteObject(pen);}
    label(dc,L"YOUR NEXT MOVE",{568,132,798,162},15,accent,true);
    label(dc,L"Difficulty",{568,169,798,197},15,muted);
    label(dc,noteMode?L"Notes are ON (N)":L"Notes are OFF (N)",{568,495,798,525},16,accent,true);
    label(dc,L"Arrow keys: move\n1-9: enter a number\nDelete: clear a cell\nN: toggle pencil notes\nCtrl+Z: undo",{568,535,800,650},15,muted,false,DT_LEFT|DT_WORDBREAK);
    int filled=0;for(int n:state.board)if(n)++filled;
    label(dc,std::to_wstring(filled)+L" / 81 filled",{34,646,535,674},14,muted);
    label(dc,status,{34,690,802,742},16,ink,false,DT_LEFT|DT_WORDBREAK);
    label(dc,L"Progress lasts for this session. Close the window to exit.",{34,754,800,780},13,muted);
}
LRESULT CALLBACK proc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){switch(msg){
case WM_CREATE:{level=CreateWindowW(L"COMBOBOX",L"",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,568,202,220,180,hwnd,(HMENU)100,nullptr,nullptr);
    for(auto s:{L"Easy",L"Medium",L"Hard"})SendMessageW(level,CB_ADDSTRING,0,(LPARAM)s);SendMessage(level,CB_SETCURSEL,0,0);
    const wchar_t* names[]={L"New puzzle",L"Pencil notes (N)",L"Hint",L"Check answers",L"Undo",L"Switch theme"};
    for(int i=0;i<6;++i)CreateWindowW(L"BUTTON",names[i],WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,568,248+i*40,220,32,hwnd,(HMENU)(INT_PTR)(101+i),nullptr,nullptr);
    fresh();SetTimer(hwnd,1,1000,nullptr);return 0;}
case WM_COMMAND:{int id=LOWORD(wp);if(id==100&&HIWORD(wp)==CBN_SELCHANGE){difficulty=int(SendMessage(level,CB_GETCURSEL,0,0));status=L"Difficulty selected. Press New puzzle to apply.";}
    if(id==101){if(history.empty()||won||MessageBoxW(hwnd,L"Start a new puzzle and discard this board?",L"New puzzle",MB_YESNO|MB_ICONQUESTION)==IDYES)fresh();}
    if(id==102)noteMode=!noteMode;
    if(id==103&&!won){int p=selected;if(puzzle.clues[p]||state.board[p]==puzzle.solution[p]){p=-1;for(int i=0;i<81;++i)if(state.board[i]!=puzzle.solution[i]){p=i;break;}}
        if(p>=0){history.push_back(state);selected=p;state.board[p]=puzzle.solution[p];state.notes[p]=0;status=L"One cell revealed. Keep going!";complete();}}
    if(id==104){int errors=0;for(int i=0;i<81;++i)if(state.board[i]&&state.board[i]!=puzzle.solution[i])++errors;status=errors?std::to_wstring(errors)+L" filled cells need another look.":L"Every filled cell is correct so far.";}
    if(id==105&&!history.empty()){state=history.back();history.pop_back();won=false;status=L"Last move undone.";}
    if(id==106)dark=!dark;SetFocus(hwnd);InvalidateRect(hwnd,nullptr,FALSE);return 0;}
case WM_LBUTTONDOWN:{int x=GET_X_LPARAM(lp)-left,y=GET_Y_LPARAM(lp)-top;if(x>=0&&y>=0&&x<504&&y<504){selected=y/cell*9+x/cell;SetFocus(hwnd);InvalidateRect(hwnd,nullptr,FALSE);}return 0;}
case WM_KEYDOWN:if(wp>='1'&&wp<='9')enter(int(wp-'0'));else if(wp>=VK_NUMPAD1&&wp<=VK_NUMPAD9)enter(int(wp-VK_NUMPAD0));else if(wp==VK_DELETE||wp==VK_BACK||wp=='0')enter(0);else if(wp=='N')noteMode=!noteMode;
    else if(wp=='Z'&&(GetKeyState(VK_CONTROL)&0x8000))SendMessage(hwnd,WM_COMMAND,105,0);
    else if(wp==VK_LEFT&&selected%9>0)--selected;else if(wp==VK_RIGHT&&selected%9<8)++selected;else if(wp==VK_UP&&selected>=9)selected-=9;else if(wp==VK_DOWN&&selected<72)selected+=9;
    InvalidateRect(hwnd,nullptr,FALSE);return 0;
case WM_TIMER:InvalidateRect(hwnd,nullptr,FALSE);return 0;
case WM_ERASEBKGND:return 1;
case WM_PAINT:{PAINTSTRUCT ps;HDC dc=BeginPaint(hwnd,&ps);RECT r;GetClientRect(hwnd,&r);HDC mem=CreateCompatibleDC(dc);HBITMAP bitmap=CreateCompatibleBitmap(dc,r.right,r.bottom);auto old=SelectObject(mem,bitmap);paint(hwnd,mem);BitBlt(dc,0,0,r.right,r.bottom,mem,0,0,SRCCOPY);SelectObject(mem,old);DeleteObject(bitmap);DeleteDC(mem);EndPaint(hwnd,&ps);return 0;}
case WM_DESTROY:PostQuitMessage(0);return 0;
}return DefWindowProcW(hwnd,msg,wp,lp);}
int WINAPI WinMain(HINSTANCE instance,HINSTANCE,LPSTR,int show){WNDCLASSW wc{};wc.lpfnWndProc=proc;wc.hInstance=instance;wc.lpszClassName=L"SudokuStudio";wc.hCursor=LoadCursor(nullptr,IDC_ARROW);wc.hIcon=LoadIcon(nullptr,IDI_APPLICATION);RegisterClassW(&wc);
    RECT r{0,0,830,800};AdjustWindowRect(&r,WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,FALSE);
    HWND hwnd=CreateWindowW(wc.lpszClassName,L"Sudoku Studio",WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,CW_USEDEFAULT,CW_USEDEFAULT,r.right-r.left,r.bottom-r.top,nullptr,nullptr,instance,nullptr);
    if(!hwnd)return 1;ShowWindow(hwnd,show);MSG msg{};while(GetMessageW(&msg,nullptr,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);}return int(msg.wParam);}
