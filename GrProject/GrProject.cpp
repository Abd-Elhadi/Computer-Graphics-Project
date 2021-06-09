#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <vector>
#include <math.h>
#include "Resource.h"
using namespace std;

COLORREF color = RGB(0,0,0);

void Draw4Points(HDC hdc, int xc, int yc, int a, int b)
{
    SetPixel(hdc, xc + a, yc + b, color);
    SetPixel(hdc, xc - a, yc + b, color);
    SetPixel(hdc, xc - a, yc - b, color);
    SetPixel(hdc, xc + a, yc - b, color);

}

//midpoint ellepse 
class DirectEllipse {
public:
    int a = 100, b = 80, xc, yc;
    HDC hdc;
    void lbutton(HWND hwnd, LPARAM lParam) {

        xc = LOWORD(lParam);
        yc = HIWORD(lParam);
        hdc = GetDC(hwnd);
        EllipseDirect(hdc, xc, yc, a, b);
        ReleaseDC(hwnd, hdc);
    }
    void EllipseDirect(HDC hdc, int xc, int yc, int a, int b) {
        int x = 0, y = b;
        double b2 = b * b, a2 = a * a;
        Draw4Points(hdc, xc, yc, x, y);
        while (b * b * x < a * a * y) {
            x++;
            y = round(sqrt(b2 * (double)(1.0 - 1.0 * x * x / a2)));
            Draw4Points(hdc, xc, yc, x, y);
        }
        while (y > 0) {
            y--;
            x = round(sqrt(a2 * (double)(1.0 - 1.0 * y * y / b2)));
            Draw4Points(hdc, xc, yc, x, y);
        }
    }
};


class PolarEllipse {
public:
    int a = 80, b = 60, xc, yc;
    HDC hdc;
    void lbutton(LPARAM lParam, HWND hwnd) {

        xc = LOWORD(lParam);
        yc = HIWORD(lParam);
        hdc = GetDC(hwnd);
        polarellipse(hdc, xc, yc, a, b);
        ReleaseDC(hwnd, hdc);
    }
    void polarellipse(HDC hdc, int xc, int yc, int  a, int  b) {
        int x = a, y = 0;
        Draw4Points(hdc, xc, yc, x, y);
        double f = sqrt(a * a - b * b), e = f / a, b2 = b * b;
        for (double theta = 0.001; theta <= 3.14 / 2; theta += 0.001) {
            double R = a * b / sqrt((b * b * cos(theta) * cos(theta)) + (a * a * sin(theta) * sin(theta)));
            x = round(R * cos(theta));
            y = round(R * sin(theta));
            Draw4Points(hdc, xc, yc, x, y);
        }
    }
};


class Point {
public:
    double x;
    double y;

    Point() {}

    Point(double x, double y);
};
class ParametricLine {
public:
    int counter = 0;
    Point p1;
    Point p2;
    void lbutton(LPARAM lParam, HWND hwnd) {
        if (counter == 0) {
            p1.x = LOWORD(lParam);
            p1.y = HIWORD(lParam);
        }
        else {
            p2.x = LOWORD(lParam);
            p2.y = HIWORD(lParam);
            HDC hdc = GetDC(hwnd);
            Parametric(hdc, p1, p2);
            counter = -1;
        }
        ++counter;
    }
    void Parametric(HDC hdc, Point startPoint, Point endPoint) {
        int numberOfPoints = max(abs(startPoint.x - endPoint.x), abs(startPoint.y - endPoint.y));
        double dt = 1.0 / numberOfPoints;
        double x = startPoint.x, y = startPoint.y;
        double dx = dt * (endPoint.x - startPoint.x);
        double dy = dt * (endPoint.y - startPoint.y);
        for (double t = 1; t <= numberOfPoints; t++) {
            SetPixel(hdc, round(x), round(y), color);
            x += dx;
            y += dy;
        }
    }
};
class DDALine {

public:
    int counter = 0;
    Point p1, p2;
    void lbutton(LPARAM lParam, HWND hwnd) {
        if (counter == 0) {
            p1.x = LOWORD(lParam);
            p1.y = HIWORD(lParam);
        }
        else {
            p2.x = LOWORD(lParam);
            p2.y = HIWORD(lParam);
            HDC hdc = GetDC(hwnd);
            DDA(hdc, p1, p2);
            counter = -1;
        }
        ++counter;
    }
    void DDA(HDC hdc, Point startPoint, Point endPoint) {
        double dx = endPoint.x - startPoint.x;
        double dy = endPoint.y - startPoint.y;

        // slope < 1, x is the independent variable
        if (abs(dy) < abs(dx)) {
            double slope = dy / dx;

            if (startPoint.x > endPoint.x) {
                swap(startPoint.x, endPoint.x);
                swap(startPoint.y, endPoint.y);
            }

            int x = startPoint.x;
            double y = startPoint.y;
            SetPixel(hdc, x, y, color);

            while (x < endPoint.x) {
                x++;
                y += slope;
                SetPixel(hdc, x, y, color);
            }
        }

        // slope > 1, y is the independent variable
        else {
            double slope = dx / dy;

            if (startPoint.y > endPoint.y) {
                swap(startPoint.x, endPoint.x);
                swap(startPoint.y, endPoint.y);
            }

            double x = startPoint.x;
            int y = startPoint.y;
            SetPixel(hdc, x, y, color);

            while (y < endPoint.y) {
                y++;
                x += slope;
                SetPixel(hdc, x, y, color);
            }
        }
    }
};

class MidpointLine {
public:
    int counter = 0;
    Point p1, p2;
    void lbutton(LPARAM lParam, HWND hwnd) {
        if (counter == 0){
            p1.x = LOWORD(lParam);
            p1.y = HIWORD(lParam);
        }
        else {
            p2.x = LOWORD(lParam);
            p2.y = HIWORD(lParam);
            HDC hdc = GetDC(hwnd);
            midpointLine(hdc, p1, p2);
            counter = -1;
        }
        ++counter;
    }
    void midpointLine(HDC hdc, Point startPoint, Point endPoint) {
        double dx = endPoint.x - startPoint.x;
        double dy = endPoint.y - startPoint.y;

        // slope < 1, x is the independent variable
        if (abs(dy) < abs(dx)) {
            int d = dy - (dx / 2);
            int x = startPoint.x;
            double y = startPoint.y;
            SetPixel(hdc, x, y, color);

            while (x < endPoint.x) {
                x++;
                if (d < 0)
                    d = d + dy;

                else {
                    d += (dy - dx);
                    y++;
                }
                SetPixel(hdc, x, y, color);
            }
        }
        else if (dx < dy) {
            int d = dx - (dy / 2);
            int x = startPoint.x, y = startPoint.y;

            while (y < endPoint.y) {
                y++;
                if (d < 0)
                    d = d + dx;

                else {
                    d += (dx - dy);
                    x++;
                }
                SetPixel(hdc, x, y, color);
            }
        }
    }
};


class DirectCircle {
public:
    DirectCircle();
    ~DirectCircle();

private:

};

class PolarCircle {
public:
    PolarCircle() {

    }
    ~PolarCircle() {

    }
private:
};

class InteractiveCircle {
public:
    InteractiveCircle();
    ~InteractiveCircle();

private:

};

class MidpointCircle {
public:
    MidpointCircle();
    ~MidpointCircle();

private:

};

class ModifiedMidpointCircle
{
public:
    ModifiedMidpointCircle();
    ~ModifiedMidpointCircle();

private:

};


/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain(HINSTANCE hThisInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpszArgument,
    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_HAND);
    wincl.lpszMenuName = MAKEINTRESOURCE(MY_MENU);                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Change background color to white */
    wincl.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx(&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx(
        0,                   /* Extended possibilites for variation */
        szClassName,         /* Classname */
        _T("Code::Blocks Template Windows App"),       /* Title Text */
        WS_OVERLAPPEDWINDOW, /* default window */
        CW_USEDEFAULT,       /* Windows decides the position */
        CW_USEDEFAULT,       /* where the window ends up on the screen */
        544,                 /* The programs width */
        375,                 /* and height in pixels */
        HWND_DESKTOP,        /* The window is a child-window to desktop */
        NULL,                /* No menu */
        hThisInstance,       /* Program Instance handler */
        NULL                 /* No Window Creation data */
    );

    /* Make the window visible on the screen */
    ShowWindow(hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage(&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

BOOL CALLBACK DestoryChildCallback(
	HWND   hwnd,
	LPARAM lParam
)
{
	if (hwnd != NULL) {
		DestroyWindow(hwnd);
	}

	return TRUE;
}

/*  This functions are called by the Windows function DispatchMessage()  */
DirectEllipse directtellipse;
//direct_circle directcircle;
//polar_circle polarcircle;
//interactive_circle interactivecircle;
//midpoint_circle midpointcircle;
//modified_midpoint_circle modifiedmidpointcircle;
PolarEllipse polarEllipse;
ParametricLine parLine;
MidpointLine midpoLine;
DDALine dLine;

int flag = 0;
int var;

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    /* handle the messages */
    switch (message) {
    case WM_LBUTTONDOWN: {
        switch (flag) {
            case 1: {
                directtellipse.lbutton(hwnd, lParam);
                break;
            }
            case 2: {
                polarEllipse.lbutton(lParam, hwnd);
                break;
            }
            case 3: {
                parLine.lbutton(lParam, hwnd);
            }
            case 4:{
                dLine.lbutton(lParam, hwnd);
            }
            case 5: {
                midpoLine.lbutton(lParam, hwnd);
            }
        }
        break;
    }
    case WM_LBUTTONUP: {
        //break;
    }
    case WM_RBUTTONDOWN:
    {
        //clipping
        /*
        switch (flag) {
            case 3: {
                //clip.rbutton();
                break;
            }
        }
        */
        //break;
        
    }
    case WM_RBUTTONUP: {
        break;
    }
    case WM_COMMAND: {
        var = LOWORD(wParam);
        switch (var) {
        case Save_ID:
            break;
        case Load_ID:
            break;
        case Exit_ID:
            PostQuitMessage(0);
            break;
        case DirectEllipse_ID:
            flag = 1;
            break;
        case PolarEllipse_ID:
            flag = 2;
            break;
        case ParametricLine_ID:
            //lineParametric = true;
            flag = 3;
            break;
        case LineDDA_ID:
            flag = 4;
            break;
        case MidPointLine_ID:
            flag = 5;
			break;
		case Black_ID:
			color = RGB(0, 0, 0);
			break;
		case Red_ID:
			color = RGB(200, 0, 0);
			break;
		case Blue_ID:
			color = RGB(0, 0, 200);
			break;
		case Green_ID:
			color = RGB(0, 200, 0);
			break;
		case Clear_ID:
			DestroyWindow(hwnd);
			break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}