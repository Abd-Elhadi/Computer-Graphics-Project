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
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

COLORREF color = RGB(0,0,0);
vector<string> allPoints;



void drawPixel(HDC hdc, int x, int y, COLORREF color) {
	COLORREF black = RGB(0, 0, 0);
	COLORREF red = RGB(200, 0, 0);
	COLORREF blue = RGB(0, 0, 200);
	COLORREF green = RGB(0, 200, 0);

	SetPixel(hdc, x, y, color);

	string xString = to_string(x);
	string yString = to_string(y);
	string cString = "0";
	if (color == black) {
		cString = "0";
	}
	else if (color == red) {
		cString = "1";
	}
	else if (color == blue) {
		cString = "2";
	}
	else if (color == green) {
		cString = "3";
	}
	string line = xString + "," + yString + "," + cString;
	allPoints.push_back(line);
}

vector<string> split(string line, char token) {
	vector<string> splitVector;
	string temp = "";
	for (int i = 0; i < line.length(); i++) {
		if (line[i] != token) {
			temp += line[i];
		} else {
			splitVector.push_back(temp);
			temp = "";
		}
	}
	if (temp != "") {
		splitVector.push_back(temp);
	}
	return splitVector;
}

void saveData() {
	ofstream file("data.txt");
	for (auto i = allPoints.begin(); i != allPoints.end(); ++i) {
		file << *i << "\n";
	}
	file.close();
}

void loadData(HDC hdc) {
	string line;
	vector<string> splitLine;
	ifstream file("data.txt");

	while (getline(file, line)) {
		splitLine = split(line, ',');
		int x = stol(splitLine.at(0));
		int y = stol(splitLine.at(1));
		int c = stol(splitLine.at(2));
		if (c == 0) {
			drawPixel(hdc, x, y, RGB(0, 0, 0));
		}
		else if (c == 1) {
			drawPixel(hdc, x, y, RGB(200, 0, 0));
		}
		else if (c == 2) {
			drawPixel(hdc, x, y, RGB(0, 0, 200));
		}
		else if (c == 3) {
			drawPixel(hdc, x, y, RGB(0, 200, 0));
		}
	}

	file.close();
}

void Draw4Points(HDC hdc, int xc, int yc, int a, int b)
{
	drawPixel(hdc, xc + a, yc + b, color);
	drawPixel(hdc, xc - a, yc + b, color);
	drawPixel(hdc, xc - a, yc - b, color);
	drawPixel(hdc, xc + a, yc - b, color);
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
			drawPixel(hdc, round(x), round(y), color);
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
			drawPixel(hdc, x, y, color);

            while (x < endPoint.x) {
                x++;
                y += slope;
				drawPixel(hdc, x, y, color);
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
			drawPixel(hdc, x, y, color);

            while (y < endPoint.y) {
                y++;
                x += slope;
				drawPixel(hdc, x, y, color);
            }
        }
    }
};


class MidpointLine {
    int counter = 0;
    Point p1, p2;
public:
    void lbutton(LPARAM lParam, HWND hwnd) {
        if (counter == 0) {
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
    //Bresenham (midpoint) algorithm (integer DDA) 
    void midpointLine(HDC hdc, Point startPoint, Point endPoint) {
        //convert all doubles to int (the algorithm works on int only)
        int xs = (int)startPoint.x; 
        int ys = (int)startPoint.y;
        int xe = (int)endPoint.x;
        int ye = (int)endPoint.y;

        int deltaX = xe - xs;
        int deltaY = ye - ys;

        if (abs(deltaY) <= abs(deltaX)) {
            if (xs > xe) {
                swap(xs, xe);
                swap(ys, ye);
            }

            deltaX = abs(deltaX);
            deltaY = abs(deltaY);
            int error = 2 * deltaY - deltaX;
            int d1 = 2 * deltaY;
            int d2 = 2 * (deltaY - deltaX);

            int x = xs;
            int y = ys;

            int increment;
            if (ys < ye)
                increment = 1;
            else
                increment = -1;

			drawPixel(hdc, x, y, color);
            while (x < xe) {
                if (error < 0)
                    error += d1;
                else {
                    error += d2;
                    y += increment;
                }
                x++;
				drawPixel(hdc, x, y, color);
            }
        }
        else {
            if (ys > ye) {
                swap(xs, xe);
                swap(ys, ye);
            }
            deltaX = abs(deltaX);
            deltaY = abs(deltaY);

            int error = 2 * deltaX - deltaY;
            int d1 = 2 * deltaX;
            int d2 = 2 * (deltaX - deltaY);

            int x = xs;
            int y = ys;

            int increment;
            if (xs < xe)
                increment = 1;
            else
                increment = -1;

			drawPixel(hdc, x, y, color);
            while (y < ye) {
                if (error < 0)
                    error += d1;
                else {
                    error += d2;
                    x += increment;
                }
                y++;
				drawPixel(hdc, x, y, color);
            }
        }
    }
};

void draweight(HDC hdc, int x, int y, int xc, int yc) {
	drawPixel(hdc, xc + x, yc + y, color);
	drawPixel(hdc, xc - x, yc + y, color);
	drawPixel(hdc, xc + x, yc - y, color);
	drawPixel(hdc, xc - x, yc - y, color);

	drawPixel(hdc, xc - y, yc + x, color);
	drawPixel(hdc, xc + y, yc - x, color);
	drawPixel(hdc, xc + y, yc + x, color);
	drawPixel(hdc, xc - y, yc - x, color);
}

class DirectCircle {
public:

    int a = 80, xc, yc;
    HDC hdc;
    void lbutton(LPARAM lParam, HWND hwnd) {

        xc = LOWORD(lParam);
        yc = HIWORD(lParam);
        hdc = GetDC(hwnd);
        Direct(hdc, xc, yc, a);
        ReleaseDC(hwnd, hdc);
    }
    void Direct(HDC hdc, int xc, int yc, int r)
    {
        int x = r;
        int y = 0;
        int R2 = r * r;
        draweight(hdc, xc, yc, x, y);
        while (x > y)
        {
            y++;
            x = (sqrt((double)(R2 - y * y)));
            draweight(hdc, x, y, xc, yc);
        }

    }
   

private:

};

class PolarCircle {
public:

    int a = 80, xc, yc;
    HDC hdc;
    void lbutton(LPARAM lParam, HWND hwnd) {

        xc = LOWORD(lParam);
        yc = HIWORD(lParam);
        hdc = GetDC(hwnd);
        Polar(hdc, xc, yc, a);
        ReleaseDC(hwnd, hdc);
    }
    void Polar(HDC hdc, int xc, int yc, int R)
    {
        int x = R, y = 0;
        double theta = 0, dtheta = 1.0 / R;
        draweight(hdc, xc, yc, x, y);
        while (x > y)
        {
            theta += dtheta;
            x = round(R * cos(theta));
            y = round(R * sin(theta));
            draweight(hdc, x , y ,xc, yc);
        }
    }

private:
};

class InteractiveCircle {
public:

    int a = 80, xc, yc;
    HDC hdc;
    void lbutton(LPARAM lParam, HWND hwnd) {

        xc = LOWORD(lParam);
        yc = HIWORD(lParam);
        hdc = GetDC(hwnd);
        CircleIterativePolar(hdc, xc, yc, a);
        ReleaseDC(hwnd, hdc);
    }
    void CircleIterativePolar(HDC hdc, int xc, int yc, int R)
    {
        double x = R, y = 0;
        double dtheta = 1.0 / R;
        double cdtheta = cos(dtheta), sdtheta = sin(dtheta);
        draweight(hdc, xc, yc, R, 0);
        while (x > y)
        {
            double x1 = x * cdtheta - y * sdtheta;
            y = x * sdtheta + y * cdtheta;
            x = x1;
            draweight(hdc, round(x), round(y), xc, yc);
        }
    }

private:

};

class MidpointCircle {
public:
    int a = 80, xc, yc;
    HDC hdc;
    void lbutton(LPARAM lParam, HWND hwnd) {

        xc = LOWORD(lParam);
        yc = HIWORD(lParam);
        hdc = GetDC(hwnd);
        midpoint(hdc, xc, yc, a);
        ReleaseDC(hwnd, hdc);
    }

    void midpoint(HDC hdc, int xc, int yc, int r) {
        int x = 0;
        int y = r;
        double d = 1 - r;
        while (x < y) {
            if (d <= 0) {
                d = d + 2 * x + 3;
                x++;
            }
            else {
                d = d + 2 * (x - y) + 5;
                x++;
                y--;
            }
            draweight(hdc, x, y, xc, yc);
        }
    }
};

class ModifiedMidpointCircle
{
public:

    int a = 80, xc, yc;
    HDC hdc;
    void lbutton(LPARAM lParam, HWND hwnd) {

        xc = LOWORD(lParam);
        yc = HIWORD(lParam);
        hdc = GetDC(hwnd);
        ModifiedMidpoint(hdc, xc, yc, a);
        ReleaseDC(hwnd, hdc);
    }

    void ModifiedMidpoint(HDC hdc, int xc, int yc, int R)
    {
        int x = 0, y = R;
        int d = 1 - R;
        int c1 = 3, c2 = 5 - 2 * R;
        draweight(hdc, xc, yc, x, y);
        while (x < y)
        {
            if (d < 0)
            {
                d += c1;
                c2 += 2;
            }
            else
            {
                d += c2;
                c2 += 4;
                y--;
            }
            c1 += 2;
            x++;
            draweight(hdc, x, y, xc, yc);
        }
    }

private:

};


class PointClipping
{
public:
    int counter = 0;
    Point p1;
    Point p2;
    Point p3;

    void lbutton(LPARAM lParam, HWND hwnd) {
        HDC hdc = GetDC(hwnd);
        if (counter == 0) {
            p1.x = LOWORD(lParam);
            p1.y = HIWORD(lParam);
            counter++;
        }
        else if(counter==1){
            p2.x = LOWORD(lParam);
            p2.y = HIWORD(lParam);
            Rectangle(hdc, p1.x, p1.y , p2.x , p2.y);
            counter++;
        }
        else if(counter == 2) {
            p3.x = LOWORD(lParam);
            p3.y = HIWORD(lParam);
            pointclipping(hdc, p3.x, p3.y, p1.x, p1.y, p2.x, p2.y);
            counter = 0;

        }
    }
    void pointclipping(HDC hdc, int x, int y, int xleft, int ytop, int xright, int ybottom)
    {
        if (x >= xleft && x <= xright && y >= ytop && y <= ybottom)
			drawPixel(hdc, x, y, color);
    }
};


union OutCode
{
    unsigned All : 4;
    struct
    {
        unsigned left : 1, top : 1, right : 1, bottom : 1;
    };
};
OutCode GetOutCode(double x, double y, int xleft, int ytop, int xright, int ybottom)
{
    OutCode out;
    out.All = 0;
    if (x < xleft)
        out.left = 1;
    else if (x > xright)
        out.right = 1;
    if (y < ytop)
        out.top = 1;
    else if (y > ybottom)
        out.bottom = 1;
    return out;
}

class lineClipping
{
public:
    int counter = 0;
    Point p1;
    Point p2;
    Point p3;
    Point p4;

    void lbutton(LPARAM lParam, HWND hwnd) {
        HDC hdc = GetDC(hwnd);
        if (counter == 0) {
            p1.x = LOWORD(lParam);
            p1.y = HIWORD(lParam);
            counter++;
        }
        else if (counter == 1) {
            p2.x = LOWORD(lParam);
            p2.y = HIWORD(lParam);
            Rectangle(hdc, p1.x, p1.y, p2.x, p2.y);
            counter++;
        }
        else if (counter == 2) {
            p3.x = LOWORD(lParam);
            p3.y = HIWORD(lParam);
            counter++;
        }
        else if (counter == 3) {
            p4.x = LOWORD(lParam);
            p4.y = HIWORD(lParam);
            CohenSuth(hdc, p3.x, p3.y, p4.x, p4.y, p1.x, p1.y, p2.x, p2.y);
            counter = 0;

        }
    }

    void VIntersect(double xs, double ys, double xe, double ye, int x, double* xi, double* yi)
    {
        *xi = x;
        *yi = ys + (x - xs) * (ye - ys) / (xe - xs);
    }
    void HIntersect(double xs, double ys, double xe, double ye, int y, double* xi, double* yi)
    {
        *yi = y;
        *xi = xs + (y - ys) * (xe - xs) / (ye - ys);
    }
    void CohenSuth(HDC hdc, int xs, int ys, int xe, int ye, int xleft, int ytop, int xright, int ybottom)
    {
        double x1 = xs, y1 = ys, x2 = xe, y2 = ye;
        OutCode out1 = GetOutCode(x1, y1, xleft, ytop, xright, ybottom);
        OutCode out2 = GetOutCode(x2, y2, xleft, ytop, xright, ybottom);
        while ((out1.All || out2.All) && !(out1.All & out2.All))
        {
            double xi, yi;
            if (out1.All)
            {
                if (out1.left)
                    VIntersect(x1, y1, x2, y2, xleft, &xi, &yi);
                else if (out1.top)
                    HIntersect(x1, y1, x2, y2, ytop, &xi, &yi);
                else if (out1.right)
                    VIntersect(x1, y1, x2, y2, xright, &xi, &yi);
                else
                    HIntersect(x1, y1, x2, y2, ybottom, &xi, &yi);
                x1 = xi;
                y1 = yi;
                out1 = GetOutCode(x1, y1, xleft, ytop, xright, ybottom);
            }
            else
            {
                if (out2.left)
                    VIntersect(x1, y1, x2, y2, xleft, &xi, &yi);
                else if (out2.top)
                    HIntersect(x1, y1, x2, y2, ytop, &xi, &yi);
                else if (out2.right)
                    VIntersect(x1, y1, x2, y2, xright, &xi, &yi);
                else
                    HIntersect(x1, y1, x2, y2, ybottom, &xi, &yi);
                x2 = xi;
                y2 = yi;
                out2 = GetOutCode(x2, y2, xleft, ytop, xright, ybottom);
            }
        }
        if (!out1.All && !out2.All)
        {

            MoveToEx(hdc, round(x1), round(y1), NULL);
            LineTo(hdc, round(x2), round(y2));
        }
    }

};

DirectEllipse directtellipse;
DirectCircle directcircle;
PolarCircle polarcircle;
InteractiveCircle interactivecircle;
MidpointCircle midpointcircle;
ModifiedMidpointCircle modifiedmidpointcircle;
PolarEllipse polarEllipse;
ParametricLine parLine;
MidpointLine midpoLine;
DDALine dLine;
PointClipping pointclipping;
lineClipping lineclipping;


class Filling
{

public:
    int counter = 0;
    Point p1;
    Point p2;

    void lbutton(LPARAM lParam, HWND hwnd) {
        HDC hdc = GetDC(hwnd);
        if (counter == 0) {
            midpointcircle.lbutton(lParam, hwnd);
            p1.x = LOWORD(lParam);
            p1.y = HIWORD(lParam);
            counter++;
        }
        else if (counter == 1) {
            p2.x = LOWORD(lParam);
            p2.y = HIWORD(lParam);
            findQuarter(hdc,p2.x, p2.y, p1.x, p1.y);
            counter = 0;
        }
    }

   
    void drawLine(HDC hdc, int x1, int y1, int x2, int y2) {
        double dx = x2 - x1;
        double dy = y2 - y1;
        for (double t = 0; t < 1; t += 0.001) {
            int x = x1 + (dx * t);
            int y = y1 + (dy * t);
            SetPixel(hdc, x, y, color);
        }

    }

    void findQuarter(HDC hdc,int x, int y, int xc, int yc)
    {

        if ((x >= 0 and y > 0) || (x == 0 & y == 0))
        {
            fill(hdc, xc, yc, 1);

        }

        else if (x <= 0 and y > 0)
        {
            fill(hdc, xc, yc, 2);

        }

        else if (x < 0 and y <= 0)
        {
            fill(hdc, xc, yc, 3);

        }
        else if (x > 0 and y <= 0)
        {
            fill(hdc, xc, yc, 4);

        }
    }


    void fill(HDC hdc, int xc, int yc, int quarter) {
        int r = 80;
        int x = 0;
        int y = r;
        double d = 1 - r;
        while (x < y) {
            if (d <= 0) {
                d = d + 2 * x + 3;
                x++;
            }
            else {
                d = d + 2 * (x - y) + 5;
                x++;
                y--;
            }
            draw(hdc, x, y, xc, yc, quarter);
        }
    }
    void draw(HDC hdc, int x, int y, int xc, int yc, int quarter) {

        if (quarter == 1) {
            drawLine(hdc, xc,  yc , xc + x, yc - y); //1
            drawLine(hdc, xc, yc, xc + y, yc - x); //2
        }
        else if (quarter == 2) {
            drawLine(hdc, xc, yc, xc + y, yc + x); //4
            drawLine(hdc, xc, yc, xc - y, yc - x); //3

        }
        else if (quarter == 3) {
            drawLine(hdc, xc, yc, xc - x, yc + y);//5
            drawLine(hdc, xc, yc, xc - y, yc + x); //6
        }
        else if (quarter == 4) {
            drawLine(hdc, xc, yc, xc + x, yc + y); //7
            drawLine(hdc, xc, yc, xc - x, yc - y); //8
        }
    }

};

void printData() {
    cout << "Hello World";
}

Filling circleFilling;



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

/*  This functions are called by the Windows function DispatchMessage()  */

int flag = 0;
int var;

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    /* handle the messages */
    switch (message) {
    case WM_LBUTTONDOWN: {
        printData();
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
                break;
            }
            case 4:{
                dLine.lbutton(lParam, hwnd);
                break;
            }
            case 5: {
                midpoLine.lbutton(lParam, hwnd);
                break;
            }
            case 6: {
               midpointcircle.lbutton(lParam, hwnd);
               break;
            }
            case 7: {
                directcircle.lbutton(lParam, hwnd);
                break;
            }
            case 8: {
                polarcircle.lbutton(lParam, hwnd);
                break;
            }
            case 9: {
                interactivecircle.lbutton(lParam, hwnd);
                break;
            }
            case 10: {
                modifiedmidpointcircle.lbutton(lParam, hwnd);
                break;
            }
            case 11: {
                pointclipping.lbutton(lParam, hwnd);
                break;
            }
            case 12: {
                lineclipping.lbutton(lParam, hwnd);
                break;
            }
            case 13: {
                circleFilling.lbutton(lParam, hwnd);
                break;
            }

        }
        break;
    }
    case WM_COMMAND: {
        var = LOWORD(wParam);
        switch (var) {
        case Save_ID:
			saveData();
            break;
		case Load_ID: {
			HDC hdc = GetDC(hwnd);
			loadData(hdc);
			break;
		}
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
            flag = 3;
            break;
        case LineDDA_ID:
            flag = 4;
            break;
        case MidPointLine_ID:
            flag = 5;
			break;
        case MidpointCircle_ID:
            flag = 6;
            break;
        case DirectCircle_ID:
            flag = 7;
            break;
        case PolarCircle_ID:
            flag = 8;
            break;
        case InterActivePolarCircle_ID:
            flag = 9;
            break;
        case ModifiedMidpointCircle_ID:
            flag = 10;
            break;
        case PointClipping_ID:
            flag = 11;
            break;
        case LineClipping_ID:
            flag = 12;
            break;
        case Filling_ID:
            flag = 13;
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
		case Clear_ID :
			InvalidateRect(hwnd, NULL, true);
			allPoints.clear();
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