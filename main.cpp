#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <stack>

#define PI 3.141592
using namespace std;

int grid_size = 4; // размер ячейки
const int grid_width = 400; // Ширина таблицы
const int grid_height = 200; // Высота таблицы
COLORREF colorMass[grid_width][grid_height] = {0 };
HWND handleWindow;
HDC handleDC;
HBRUSH brush;
HPEN hPen;


void draw_grid(int w, int h, int size)
{
    HWND handleWindow;
    handleWindow = GetConsoleWindow();
    HDC handleDC;
    handleDC = GetDC(handleWindow);             // получили контекст отрисовки окна
    //handleDC = GetDC();             // получили контекст отрисовки окна

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


    for (int i = 0; i <= h; i++)                    // рисуем горизонтальные линии
    {
        SelectObject(handleDC, penRegular);
        if (i % 5 == 0) {
            SelectObject(handleDC, penMain);
        }
        MoveToEx(handleDC, 0, i * size, NULL);
        LineTo(handleDC, w * size, i * size);
    }
    for (int i = 0; i <= w; i++)                    // Рисуем вертикальные линии
    {
        SelectObject(handleDC, penRegular);
        if (i % 5 == 0) {
            SelectObject(handleDC, penMain);
        }
        MoveToEx(handleDC, i * size, 0, NULL);
        LineTo(handleDC, i * size, h * size);
    }
    DeleteObject(penRegular);                                                               // Удалим
    DeleteObject(penMain);                                                               // Удалим
}

void setPixel(int x, int y, int sizePixel = 15, COLORREF color = RGB(255, 0, 0))
{
    colorMass[x][y] = color;
    //Переведем координаты круга так чтобы начало было в левом нижем углу
    x = x - 1;
    y = grid_height - y;
    brush = CreateSolidBrush(color);                                   // Создадим кисть
    hPen = CreatePen(PS_SOLID, 1, color);

    SelectObject(handleDC, brush);
    SelectObject(handleDC, hPen);
    Rectangle(handleDC, x * sizePixel, y * sizePixel, x * sizePixel + sizePixel, y * sizePixel + sizePixel);
    DeleteObject(brush);                                                               // Удалим
    DeleteObject(hPen);                                                               // Удалим

}

void line_bresenham(int x1, int y1, int x2, int y2, COLORREF color = RGB(255, 0, 0)) {

    //cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<endl;
    int stepX = 1;
    int stepY = 1;
    int sizePixel = grid_size;
    if (x1 > x2) {
        stepX = -1;
    }
    if (y1 > y2) {
        stepY = -1;
    }
    setPixel(x2, y2, sizePixel, color); //Поставим пиксель в конечной точке
    if ((x1 == x2) & (y1 == y2)) { //Если начальная и конечная точки совпадают работа окончена
        return;
    }



    int deltax = abs(x2 - x1);
    int deltay = abs(y2 - y1);
    int error = 0;
    if (deltax > deltay) { //Если если накло меньше 45 то мы приращиваем по X, если нет то Y
        int deltaerr = (deltay + 1);
        for (int i = x1; stepX * x1 < stepX * x2; i += stepX) { //Цикл по X, делаем домножение на setpX чтобы двигаться в правильную сторону
            setPixel(x1, y1, sizePixel, color);
            error = error + deltaerr;
            if (error >= (deltax + 1)) {
                y1 += stepY;
                error = error - (deltax + 1);
                setPixel(x1, y1, sizePixel, color);  //Вот здесь делаем наплыв, хотя это не классический алгоритм
            }
            x1 += stepX;
        }
    }
    else {
        int deltaerr = (deltax + 1);
        for (int i = y1; stepY * y1 < stepY * y2; i += stepY) {
            setPixel(x1, y1, sizePixel, color);
            error = error + deltaerr;
            if (error >= (deltay + 1)) {
                x1 += stepX;
                error = error - (deltay + 1);
                setPixel(x1, y1, sizePixel, color); //Вот здесь делаем наплыв, хотя это не классический алгоритм
            }
            y1 += stepY;
        }
    }
}

void drawLineDDA(int x1, int y1, int x2, int y2, COLORREF color = RGB(255, 0, 0)) {
    int sizePixel = grid_size;
    // вычисляем dx & dy
    int dx = x2 - x1;
    int dy = y2 - y1;
    // вычисляем шаги, необходимые для генерации пикселей
    int steps;
    if (abs(dx) > abs(dy)) {
        steps = abs(dx);
    }
    else {
        steps = abs(dy);
    }
    // вычисляем приращение x & y для каждого шага
    float stepX = dx / (float)steps;
    float stepY = dy / (float)steps;
    // Положить пиксель для каждого шага
    float X = x1;
    float Y = y1;
    for (int i = 0; i <= steps; i++)
    {
        setPixel(X, Y, sizePixel, color);  // поместить пиксель в (X, Y)
        X += stepX;           // увеличиваем x на каждом шаге
        Y += stepY;           // увеличиваем y на каждом шаге
        // шаг за шагом
    }
}

void circle_bresenham(int x1, int y1, int r, COLORREF color = RGB(255, 0, 0)) {
    int stepX = 0;
    int stepY = r;
    int delta = 1 - 2 * r;
    int error = 0;
    int sizePixel = grid_size;
    while (stepY >= stepX) {
        setPixel(x1 + stepX, y1 + stepY, sizePixel, color);
        setPixel(x1 + stepX, y1 - stepY, sizePixel, color);
        setPixel(x1 - stepX, y1 + stepY, sizePixel, color);
        setPixel(x1 - stepX, y1 - stepY, sizePixel, color);
        setPixel(x1 + stepY, y1 + stepX, sizePixel, color);
        setPixel(x1 + stepY, y1 - stepX, sizePixel, color);
        setPixel(x1 - stepY, y1 + stepX, sizePixel, color);
        setPixel(x1 - stepY, y1 - stepX, sizePixel, color);
        error = 2 * (delta + stepY) - 1;
        if ((delta < 0) && (error <= 0)) {
            stepX = stepX + 1;
            delta = delta + (2 * stepX + 1);
        }
        else if ((delta > 0) && (error > 0)) {
            stepY = stepY - 1;
            delta -= 2 * stepY + 1;
        }
        else {
            stepX = stepX + 1;
            stepY = stepY - 1;
            delta = delta + 2 * (stepX - stepY);
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

void draw_polygon(int mnogo[2][6]) {
    for (int i = 0; i < 5; i++) {
        drawLineDDA(mnogo[0][i], mnogo[1][i], mnogo[0][i + 1], mnogo[1][i + 1], RGB(227, 255, 70));
    }
    drawLineDDA(mnogo[0][0], mnogo[1][0], mnogo[0][5], mnogo[1][5], RGB(227, 255, 70));
}

COLORREF getPixel(int x, int y) {
    return colorMass[x][y];
    int xx = (x - 1) * grid_size - 1;
    int yy = (grid_height - y) * grid_size - 1;
    HWND hwnd = FindWindowA("ConsoleWindowClass", NULL); // Get the HWND
    HDC hdc = GetDC(hwnd); // Get the DC from that HWND
    COLORREF res = GetPixel(hdc, xx, yy);
    return res;
}


void checkAndAdd(stack<int>* stX, stack<int>* stY, int x, int y, COLORREF bColor, COLORREF color)
{
    COLORREF pixelColor;
    pixelColor = getPixel(x, y);
    //cout<<" pCol "<<pixelColor<<" bCol "<<bColor<<" col "<<color;
    if ((pixelColor != bColor) && (pixelColor != color)) {
        setPixel(x, y, grid_size, color);
        stX->push(x);
        stY->push(y);
        //cout<<" add: x="<<x<<" y="<<y;
    }

}
void fill_seed(int xx, int yy, COLORREF bColor, COLORREF color)
{
    stack <int> stX;  // создаем стек
    stack <int> stY;  // создаем стек
    COLORREF pixelColor;
    int x1;
    int y1;
    stX.push(xx);
    stY.push(yy);
    HANDLE handleWindow = GetStdHandle(STD_OUTPUT_HANDLE);
    while (!stY.empty()) {
        x1 = stX.top();
        y1 = stY.top();
        stX.pop();
        stY.pop();
        setPixel(x1, y1, grid_size, color);
        //char m=getchar();
        //    SetConsoleCursorPosition(handleWindow, {0,static_cast<short>(grid_size*grid_height/16)});
        //    cout<<"                                                                                                                                                ";
        //    SetConsoleCursorPosition(handleWindow, {0,static_cast<short>(grid_size*grid_height/16)});
        //    cout<<pixelColor<<" "<<bColor<<" "<<color<<"*";
        //cout<<"x:"<<x1<<" y:"<<y1<<"= Size stack:"<<stX.size()<<"                                                                                                 ";


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
    //cout<<c<<" ";//<<bColor<<" "<<color<<"*";
    //1248519 4653027 14145533
    if ((c != bColor) && (c != color))
    {
        setPixel(x, y, grid_size, color);
        PixelFill(x + 1, y, bColor, color);
        PixelFill(x - 1, y, bColor, color);

        PixelFill(x, y - 1, bColor, color);
        PixelFill(x, y + 1, bColor, color);
    }
}


int LineFill(int x, int y, int dir, int PrevXl, int PrevXr, COLORREF bColor, COLORREF color)
{
    int xl = x, xr = x, c;
    // Find line segment
    do c = getPixel(--xl, y);    while ((c != bColor) && (c != color));
    do c = getPixel(++xr, y);  while ((c != bColor) && (c != color));
    xl++; xr--;
    //line (xl, y,  xr, y); // fill segment
    drawLineDDA(xl, y, xr, y, color); // fill segment
    // Fill adjacent segments in the same direction
    for (x = xl; x <= xr; x++)
    {
        c = getPixel(x, y + dir);
        if ((c != bColor) && (c != color)) x = LineFill(x, y + dir, dir, xl, xr, bColor, color);
    }
    for (x = xl; x < PrevXl; x++)
    {
        c = getPixel(x, y - dir);
        if ((c != bColor) && (c != color)) x = LineFill(x, y - dir, -dir, xl, xr, bColor, color);
    }
    for (x = PrevXr; x < xr; x++)
    {
        c = getPixel(x, y - dir);
        if ((c != bColor) && (c != color)) x = LineFill(x, y - dir, -dir, xl, xr, bColor, color);
    }
    return xr;
}

void Fill(int x, int y, COLORREF bColor, COLORREF color) {
    LineFill(x, y, 1, x, x, bColor, color);
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
    int polygon[2][6] = {50,80,80,50,20,20,
                         0,30,80,100,80,30 };

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

        bool a=true;
        while(a){
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
                    Fill(50, 50, RGB(227, 255, 70), RGB(253, 215, 215));
                    break;}
                case 8:{
                    draw_polygon(polygon);
                    fill_seed(50, 50, RGB(227, 255, 70), RGB(253, 215, 215));
                    break;}
                case 9:{a=false;}

            }
        }


    std::cin.get();

    std::cin.get();
    return 0;
}