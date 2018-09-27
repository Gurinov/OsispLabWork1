#include <iostream>
#include <windows.h>
#include <cmath>
#include <tchar.h>


PAINTSTRUCT ps;
OPENFILENAME FileOpenDialog;
char szFile[MAX_PATH];
HDC hdc;
const int hight = 100;
const int width = 100;
const int speed = 15;
HDC hdcbitmap;
HBITMAP hbitmap;

static int x = 10;
static int y = 10;
static double angle = 0;
bool isPicture = false;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL isCursorInSprite(int xPos, int yPos);
BOOL isMousePressed;
void DrawBitmap(HDC hDC, int x, int y, HBITMAP hBitmap);

void Transform(HDC hdc, double angle)
{
    XFORM xf;
    xf.eM11 = 1;
    xf.eM12 = 0;
    xf.eM21 = 0;
    xf.eM22 = 1;
    xf.eDx = (float)-(x + width / 2);
    xf.eDy = (float)-(y + hight / 2);
    ModifyWorldTransform(hdc, &xf, MWT_RIGHTMULTIPLY);

    xf.eM11 = (float)cos(angle);
    xf.eM12 = (float)sin(angle);
    xf.eM21 = (float)-sin(angle);;
    xf.eM22 = (float)cos(angle);
    xf.eDx = 0;
    xf.eDy = 0;
    ModifyWorldTransform(hdc, &xf, MWT_RIGHTMULTIPLY);

    xf.eM11 = 1;
    xf.eM12 = 0;
    xf.eM21 = 0;
    xf.eM22 = 1;
    xf.eDx = (float)(x + width / 2);
    xf.eDy = (float)(y + hight / 2);
    ModifyWorldTransform(hdc, &xf, MWT_RIGHTMULTIPLY);
}


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex; HWND hWnd; MSG msg;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_DBLCLKS;
    wcex.lpfnWndProc = (WNDPROC) WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "HelloWorldClass";
    wcex.hIconSm = wcex.hIcon;

    RegisterClassEx(&wcex);

    hWnd = CreateWindow("HelloWorldClass", "Hello, World!",
                        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
                        CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    ZeroMemory(&FileOpenDialog, sizeof(FileOpenDialog));
    FileOpenDialog.lStructSize = sizeof(FileOpenDialog);
    FileOpenDialog.hwndOwner = hWnd;
    FileOpenDialog.lpstrFile = szFile;
    FileOpenDialog.lpstrFile[0] = '\0';
    FileOpenDialog.nMaxFile = sizeof(szFile);
    FileOpenDialog.lpstrFilter = _T("Images\0*.bmp;\0\0");
    FileOpenDialog.nFilterIndex = 1;
    FileOpenDialog.lpstrFileTitle = NULL;
    FileOpenDialog.nMaxFileTitle = 255;
    FileOpenDialog.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HBRUSH solidBrush = CreateSolidBrush(RGB(0, 0, 250));
    int prevGraphicsMode;

    switch (message)
    {
        case WM_PAINT :
            hdc = BeginPaint(hWnd,&ps);
            prevGraphicsMode = SetGraphicsMode(hdc, GM_ADVANCED);
            Transform(hdc, angle);

            if (!isPicture)
            {
                RECT rect;
                rect.top = y;
                rect.left = x;
                rect.right = x + width;
                rect.bottom = y + hight;
                FillRect(hdc, &rect, solidBrush);
            } else{
                BLENDFUNCTION bf;
                BitBlt(hdc,x,y,width,hight,hdcbitmap,0,0,MERGECOPY);
            }

            ModifyWorldTransform(hdc, NULL, MWT_IDENTITY);
            SetGraphicsMode(hdc, prevGraphicsMode);
            ReleaseDC(hWnd, hdc);
            break;

        case WM_MOUSEWHEEL:
            if (wParam & MK_SHIFT) { ;
                x += (int)(GET_WHEEL_DELTA_WPARAM(wParam)/5);
            } else {
                y -= (int)(GET_WHEEL_DELTA_WPARAM(wParam)/5);
            }
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case WM_LBUTTONUP:
            isMousePressed = false;
            break;
        case WM_LBUTTONDOWN:
            if (isCursorInSprite(LOWORD(lParam), HIWORD(lParam))) {
                isMousePressed = true;
            }
            break;
        case WM_MOUSEMOVE:
            if (isMousePressed) {
                x = LOWORD(lParam);
                y = HIWORD(lParam);
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;

        case WM_KEYDOWN:
            switch(wParam)
            {
                case VK_LEFT:
                    x -= speed;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;

                case VK_UP:
                    y -= speed;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;

                case VK_RIGHT:
                    x += speed;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;

                case VK_DOWN:
                    y += speed;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;

                case VK_ESCAPE:
                    if (MessageBox(hWnd, "Close a window?", "Message", MB_YESNO) == IDYES){
                        PostQuitMessage(0);
                    }
                    break;

                case VK_SPACE:
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;

                case VK_ADD:
                    angle += M_PI / 20;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;

                case VK_SUBTRACT:
                    angle -= M_PI / 20;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;

                case VK_TAB:
                    if (GetOpenFileName(&FileOpenDialog) == TRUE)
                    {
                        isPicture = true;
                        BITMAP bm;
                        hdcbitmap = CreateCompatibleDC(GetDC(hWnd));
                        hbitmap = (HBITMAP) LoadImage(NULL,FileOpenDialog.lpstrFile, IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
                        GetObject(hbitmap, sizeof(BITMAP),&bm);
                        SelectObject(hdcbitmap,hbitmap);
                        InvalidateRect(hWnd, NULL, TRUE);
                    }
                    break;

                case VK_SHIFT:
                   isPicture = false;
                   InvalidateRect(hWnd, NULL, TRUE);
                    break;

            }
            break;

        case WM_CREATE:
            break;
        case WM_RBUTTONDOWN:
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


BOOL isCursorInSprite(int xPos, int yPos) {
    return (xPos >= x)&&(xPos <= x+100) && (yPos >= y)&&(yPos <= y+100);
}





