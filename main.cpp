#include <iostream>
#include <windows.h>
#include <cmath>


PAINTSTRUCT ps;
HDC hdc;
const int hight = 100;
const int width = 100;
const int speed = 15;

static int x = 10;
static int y = 10;
static double angle = 0;
bool ispicture = false;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL isCursorInSprite(int xPos, int yPos);
BOOL isMousePressed;
void DrawBitmap(HDC hDC, int x, int y, HBITMAP hBitmap);

void TransformSprite(HDC hdc,double angle)
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
    static HBRUSH solidBrush = CreateSolidBrush(RGB(0, 255, 0));
    int prevGraphicsMode;

    switch (message)
    {
        case WM_PAINT :
            hdc = GetDC(hWnd);
            prevGraphicsMode = SetGraphicsMode(hdc, GM_ADVANCED);
            TransformSprite(hdc, angle);

            long bitWidth = 0, bitHight = 0;static HBITMAP hBitmap;
            if (!ispicture)
            {
                RECT rect;
                rect.top = y;
                rect.left = x;
                rect.right = x + width;
                rect.bottom = y + hight;
                FillRect(hdc, &rect, solidBrush);
            } else{
                HDC sDC = CreateCompatibleDC(hdc);
                HGDIOBJ prevobj = SelectObject(sDC, hBitmap);
                StretchBlt(hdc, x, y, width, hight, sDC, 0, 0, width, hight, SRCCOPY);
                SelectObject(sDC, prevobj);
                DeleteDC(sDC);
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
                    ispicture = true;
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
            }
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

void DrawBitmap(HDC hDC, int x, int y, HBITMAP hBitmap)
{
    HBITMAP hbm, hOldbm;
    HDC hMemDC;
    BITMAP bm;
    POINT ptSize, ptOrg;

    // Создаем контекст памяти, совместимый
    // с контекстом отображения
    hMemDC = CreateCompatibleDC(hDC);

    // Выбираем изображение bitmap в контекст памяти
    hOldbm = (HBITMAP)SelectObject(hMemDC, hBitmap);

    // Если не было ошибок, продолжаем работу
    if (hOldbm)
    {
        // Для контекста памяти устанавливаем тот же
        // режим отображения, что используется в
        // контексте отображения
        SetMapMode(hMemDC, GetMapMode(hDC));

        // Определяем размеры изображения
        GetObject(hBitmap, sizeof(BITMAP), (LPSTR) &bm);

        ptSize.x = bm.bmWidth;  // ширина
        ptSize.y = bm.bmHeight; // высота

        // Преобразуем координаты устройства в логические
        // для устройства вывода
        DPtoLP(hDC, &ptSize, 1);

        ptOrg.x = 0;
        ptOrg.y = 0;

        // Преобразуем координаты устройства в логические
        // для контекста памяти
        DPtoLP(hMemDC, &ptOrg, 1);

        // Рисуем изображение bitmap
        BitBlt(hDC, x, y, ptSize.x, ptSize.y,
               hMemDC, ptOrg.x, ptOrg.y, SRCCOPY);

        // Восстанавливаем контекст памяти
        SelectObject(hMemDC, hOldbm);
    }

    // Удаляем контекст памяти
    DeleteDC(hMemDC);
}




