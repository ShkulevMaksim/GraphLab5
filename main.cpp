#include <windows.h>
#include <iostream>
#include <cmath>
#include <stack>
#define PI 3.141592

const int grid_size = 4;
const int grid_width = 400;
const int grid_height = 200;

COLORREF colorMass[grid_width][grid_height] = { 0 };
HWND handleWindow;
HDC handleDC;
HBRUSH brush;
HPEN hPen;

void draw_grid(int w, int h, int size)
{
    handleWindow = GetConsoleWindow();
    handleDC = GetDC(handleWindow);
    HPEN penRegular;
    penRegular = CreatePen(PS_SOLID, 1, RGB(20, 20, 20));
    HPEN penMain;
    penMain = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD coord = { 0, 0 };
    DWORD written;

    if (GetConsoleScreenBufferInfo(handle, &csbi))
    {
        DWORD nChars = csbi.dwSize.X * csbi.dwSize.Y;
        FillConsoleOutputCharacter(handle, ' ', nChars, coord, &written);
        FillConsoleOutputAttribute(handle, csbi.wAttributes, nChars, coord, &written);
    }

    SetConsoleCursorPosition(handle, coord);
    for (int i = 0; i <= h; i++)
    {
        SelectObject(handleDC, penRegular);
        if (i % 5 == 0) {
            SelectObject(handleDC, penMain);
        }
        MoveToEx(handleDC, 0, i * size, nullptr);
        LineTo(handleDC, w * size, i * size);
    }

    for (int i = 0; i <= w; i++)
    {
        SelectObject(handleDC, penRegular);
        if (i % 5 == 0) {
            SelectObject(handleDC, penMain);
        }
        MoveToEx(handleDC, i * size, 0, nullptr);
        LineTo(handleDC, i * size, h * size);
    }
    DeleteObject(penRegular);
    DeleteObject(penMain);
}

void set_pixel(int x, int y, int sizePixel = 15, COLORREF color = RGB(255, 0, 0))
{
    colorMass[x][y] = color;
    x = x - 1;
    y = grid_height - y;
    brush = CreateSolidBrush(color);
    hPen = CreatePen(PS_SOLID, 1, color);
    SelectObject(handleDC, brush);
    SelectObject(handleDC, hPen);
    Rectangle(handleDC, x * sizePixel, y * sizePixel, x * sizePixel + sizePixel, y * sizePixel + sizePixel);
    DeleteObject(brush);
    DeleteObject(hPen);
}

void line_bresenham(int x1, int y1, int x2, int y2, COLORREF color = RGB(255, 0, 0)) {
    int stepX = 1;
    int stepY = 1;
    int sizePixel = grid_size;
    if (x1 > x2) {
        stepX = -1;
    }
    if (y1 > y2) {
        stepY = -1;
    }
    set_pixel(x2, y2, sizePixel, color);
    if ((x1 == x2) & (y1 == y2)) {
        return;
    }
    int delta_x = abs(x2 - x1);
    int delta_y = abs(y2 - y1);
    int error = 0;
    if (delta_x > delta_y) {
        int delta_error = (delta_y + 1);
        for (int i = x1; stepX * x1 < stepX * x2; i += stepX) {
            set_pixel(x1, y1, sizePixel, color);
            error = error + delta_error;
            if (error >= (delta_x + 1)) {
                y1 += stepY;
                error = error - (delta_x + 1);
                set_pixel(x1, y1, sizePixel, color); //Вот здесь делаем наплыв, хотя это не классический алгоритм
            }
            x1 += stepX;
        }
    }

    else {
        int delta_error = (delta_x + 1);
        for (int i = y1; stepY * y1 < stepY * y2; i += stepY) {
            set_pixel(x1, y1, sizePixel, color);
            error = error + delta_error;
            if (error >= (delta_y + 1)) {
                x1 += stepX;
                error = error - (delta_y + 1);
                set_pixel(x1, y1, sizePixel, color); //Вот здесь делаем наплыв, хотя это не классический алгоритм
            }
            y1 += stepY;
        }
    }
}

void drawLineDDA(int x1, int y1, int x2, int y2, COLORREF color = RGB(255, 0, 0)) {
    int sizePixel = grid_size;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps;
    if (abs(dx) > abs(dy)) {
        steps = abs(dx);
    }
    else {
        steps = abs(dy);
    }

    float stepX = dx / (float)steps;
    float stepY = dy / (float)steps;

    float X = x1;
    float Y = y1;
    for (int i = 0; i <= steps; i++)
    {
        set_pixel(X, Y, sizePixel, color);
        X += stepX;
        Y += stepY;
    }
}
void circle_bresenham(int x1, int y1, int r, COLORREF color = RGB(255, 0, 0)) {
    int step_x = 0;
    int step_y = r;
    int delta = 1 - 2 * r;
    int error;
    int sizePixel = grid_size;
    while (step_y >= step_x) {
        set_pixel(x1 + step_x, y1 + step_y, sizePixel, color);
        set_pixel(x1 + step_x, y1 - step_y, sizePixel, color);
        set_pixel(x1 - step_x, y1 + step_y, sizePixel, color);
        set_pixel(x1 - step_x, y1 - step_y, sizePixel, color);
        set_pixel(x1 + step_y, y1 + step_x, sizePixel, color);
        set_pixel(x1 + step_y, y1 - step_x, sizePixel, color);
        set_pixel(x1 - step_y, y1 + step_x, sizePixel, color);
        set_pixel(x1 - step_y, y1 - step_x, sizePixel, color);
        error = 2 * (delta + step_y) - 1;
        if ((delta < 0) && (error <= 0)) {
            step_x = step_x + 1;
            delta = delta + (2 * step_x + 1);
        }

        else if ((delta > 0) && (error > 0)) {
            step_y = step_y - 1;
            delta -= 2 * step_y + 1;
        }

        else {
            step_x = step_x + 1;
            step_y = step_y - 1;
            delta = delta + 2 * (step_x - step_y);
        }
    }
}

void triangle(int Ax, int Ay, int Bx, int By, int Cx, int Cy)
{
    int tmp;
    int X[3] = { Ax,Bx,Cx };
    int Y[3] = { Ay,By,Cy };
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            if (Y[j] < Y[j + 1])
            {
                tmp = Y[j + 1];
                Y[j + 1] = Y[j];
                Y[j] = tmp;
                tmp = X[j + 1];
                X[j + 1] = X[j];
                X[j] = tmp;
            }
    Cy = Y[0]; By = Y[1]; Ay = Y[2];
    Cx = X[0]; Bx = X[1]; Ax = X[2];
    int x1, x2;
    for (int sy = Ay; sy <= Cy; sy++)
    {
        x1 = Ax + (sy - Ay) * (Cx - Ax) / (Cy - Ay);
        if (sy < By)
            x2 = Ax + (sy - Ay) * (Bx - Ax) / (By - Ay);
        else
        {
            if (Cy == By)
                x2 = Bx;
            else
                x2 = Bx + (sy - By) * (Cx - Bx) / (Cy - By);
        }
        if (x1 > x2)
        {
            tmp = x1;
            x1 = x2;
            x2 = tmp;
        }
        drawLineDDA(x1, sy + 1, x2, sy + 1, RGB(0, 0, 255));
    }

}
void draw_polygon(int polygon[2][6]) {
    for (int i = 0; i < 5; i++) {
        drawLineDDA(polygon[0][i], polygon[1][i], polygon[0][i + 1], polygon[1][i + 1], RGB(227, 255, 70));
    }
    drawLineDDA(polygon[0][0], polygon[1][0], polygon[0][5], polygon[1][5], RGB(227, 255, 70));
}

COLORREF getPixel(int x, int y) {
    return colorMass[x][y];
}

void checkAndAdd(std::stack<int>* stX, std::stack<int>* stY, int x, int y, COLORREF bColor, COLORREF color)
{
    COLORREF pixelColor;
    pixelColor = getPixel(x, y);
    if ((pixelColor != bColor) && (pixelColor != color)) {
        set_pixel(x, y, grid_size, color);
        stX->push(x);
        stY->push(y);
    }
}

void fill_seed(int xx, int yy, COLORREF bColor, COLORREF color)
{
    std::stack <int> stX;
    std::stack <int> stY;
    int x1;
    int y1;
    stX.push(xx);
    stY.push(yy);
    while (!stY.empty()) {
        x1 = stX.top();
        y1 = stY.top();
        stX.pop();
        stY.pop();
        set_pixel(x1, y1, grid_size, color);
        checkAndAdd(&stX, &stY, x1, y1 - 1, bColor, color);
        checkAndAdd(&stX, &stY, x1, y1 + 1, bColor, color);
        checkAndAdd(&stX, &stY, x1 + 1, y1, bColor, color);
        checkAndAdd(&stX, &stY, x1 - 1, y1, bColor, color);
    }
}

void PixelFill(int x, int y, COLORREF bColor, COLORREF color)
{
    if (x < 0) return;
    if (y < 0) return;
    if (x > grid_width) return;
    if (y > grid_height) return;
    COLORREF c = getPixel(x, y);

    if ((c != bColor) && (c != color))
    {
        set_pixel(x, y, grid_size, color);
        PixelFill(x + 1, y, bColor, color);
        PixelFill(x - 1, y, bColor, color);
        PixelFill(x, y - 1, bColor, color);
        PixelFill(x, y + 1, bColor, color);
    }
}
int line_fill(int x, int y, int dir, int PrevXl, int PrevXr, COLORREF bColor, COLORREF color)
{
    int xl = x, xr = x, c;
    do c = getPixel(--xl, y); while ((c != bColor) && (c != color));
    do c = getPixel(++xr, y); while ((c != bColor) && (c != color));
    xl++; xr--;
    drawLineDDA(xl, y, xr, y, color);
    for (x = xl; x <= xr; x++)
    {
        c = getPixel(x, y + dir);
        if ((c != bColor) && (c != color)) x = line_fill(x, y + dir, dir, xl, xr, bColor, color);
    }
    for (x = xl; x < PrevXl; x++)
    {
        c = getPixel(x, y - dir);
        if ((c != bColor) && (c != color)) x = line_fill(x, y - dir, -dir, xl, xr, bColor, color);
    }

    for (x = PrevXr; x < xr; x++)
    {
        c = getPixel(x, y - dir);
        if ((c != bColor) && (c != color)) x = line_fill(x, y - dir, -dir, xl, xr, bColor, color);
    }
    return xr;
}

void Fill(int x, int y, COLORREF bColor, COLORREF color) {
    line_fill(x, y, 1, x, x, bColor, color);
}

int main()
{
    handleWindow = GetConsoleWindow();
    handleDC = GetDC(handleWindow);
    int x = 10;
    int y = 10;
    int x1 = 70;
    int y1 = 20;
    int x2 = 45;
    int y2 = 40;
    int polygon[2][6] = {10, 50, 90, 70, 45, 5, 10, 40, 20, 90, 70, 110 };

    draw_grid(grid_width, grid_height, grid_size);
    SetConsoleCursorPosition(handleWindow, { 0,static_cast<short>(grid_size * grid_height / 16) });

    std::cout << "Choose an option:" << '\n'
    <<"1. Grid" << '\n'
    <<"2. Triangle"<< '\n'
    <<"3. DDA "<< '\n'
    <<"4. Circle out of bresenham lines"<< '\n'
    <<"5. Circle out of DDA lines"<< '\n'
    <<"6. bresenham circle" << '\n'
    <<"7. Partition fill" << '\n'
    <<"8. Seed fill" << '\n'
    <<"9. Exit" << '\n';

    int ans = 0;
    std::cin >> ans;
    switch (ans){
            default:{std::cout<<"Not an option"<<'\n';break;}
            case 1:{
                draw_grid(grid_width, grid_height, grid_size);
                SetConsoleCursorPosition(handleWindow, { 0,static_cast<short>(grid_size * grid_height / 16) });
                break;}
        case 2:{
            triangle(x, y, x1, y1, x2, y2);
            drawLineDDA(x, y, x1, y1);
            drawLineDDA(x1, y1, x2, y2);
            drawLineDDA(x, y, x2, y2);
            break;}
        case 3:{
            drawLineDDA(x + 10, y, x1 + 10, y1)
            ;break;}
        case 4:{
            for (int i = 0; i < 360; i = i + 10) {
                line_bresenham(30, 50, (30 + 30 * cos(PI / 180 * i)), (50 + 30 * sin(PI / 180 * i)), RGB(255, i, i));
            }
            break;}
        case 5:{
            for (int i = 0; i < 360; i = i + 10) {
                drawLineDDA(100, 50, (100 + 30 * cos(PI / 180 * i)), (50 + 30 * sin(PI / 180 * i)), RGB(i, 255, i));
            }
            break;}
        case 6:{
            circle_bresenham(160, 50, 25, RGB(0, 0, 255));
            break;}
        case 7:{
            draw_polygon(polygon);
            Fill(20, 20, RGB(227, 255, 70), RGB(253, 215, 215));
            break;}
        case 8:{
            draw_polygon(polygon);
            fill_seed(20, 20, RGB(227, 255, 70), RGB(253, 215, 215));
            break;}
        case 9:{break;}

    }
    std::cin.get();
    std::cin.get();
    return 0;
}