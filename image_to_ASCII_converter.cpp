#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <Windows.h>
#include <math.h>

using namespace cv;
using namespace std;

HANDLE pen = GetStdHandle(STD_OUTPUT_HANDLE);

using byte = unsigned char;

class Pixel
{
public:
    Pixel(byte B, byte G, byte R) : B(B), G(G), R(R), color({B, G, R}), 
        lum(0.2126 * R + 0.7152 * G + 0.0722 * B)
    {}

    ~Pixel()
    {}

    string BGRstr() { 
        return "(" + to_string(B) + ", " + to_string(G) + ", " + to_string(R) + ")";
    }
    //$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,"^`'.
    char as_ASCII() { //255 224 192 160 128 96 64 32 0
        if (lum == 255) return ' ';
        if (lum >= 224) return '^';
        if (lum >= 192) return ';';
        if (lum >= 160) return '~';
        if (lum >= 128) return 'n';
        if (lum >= 96) return 'X';
        if (lum >= 64) return '#';
        if (lum >= 32) return '&';
        if (lum >= 0) return '$';
    }

//private:
    byte B, G, R, lum;
    vector<byte> color;
};

string operator* (string str, int num) {
    string s = str;
    for (int i = 0; i < num; i++)
    {
        str += s;
    }
    return str;
}

void setTextColor(int r, int g, int b) {
    cout << "\x1b[38;2;" << r << ";" << g << ";" << b << "m";
}
void setBackgroundColor(int r, int g, int b) {
    cout << "\x1b[48;2;" << r << ";" << g << ";" << b << "m";
}

int intlen(int num) {
    return to_string(num).length();
}

string normalize_int(int num, int len) {
    return to_string(num) + (string("+") * len);
}

void printASCIIimage(vector<vector<Pixel>> matrix, bool flag_is_pixelart = false, 
    bool flag_is_table = true, byte color = 15) {
    int enumerate_row = 0, enumerate_col = 0;
    cout << "\n\n\n";
    SetConsoleTextAttribute(pen, color);
    if (flag_is_table) cout << string("-") * (matrix[0].size() - 1) << '+' << "\n";
    for (vector<Pixel> vec : matrix) {
        for (Pixel pxl : vec) {
            //SetConsoleTextAttribute(pen, pxl.lum);
            switch (flag_is_pixelart)
            {
            case true:
                setBackgroundColor(pxl.R, pxl.G, pxl.B);
                cout << ' ';
                break;
            case false:
                setTextColor(pxl.R, pxl.G, pxl.B);
                cout << pxl.as_ASCII();
                break;
            }
            SetConsoleTextAttribute(pen, color);
        }
        if (flag_is_table) cout << "|" << enumerate_col++;
        cout << "\n";
    }
    if (flag_is_table) {
        cout << string("-") * (matrix[0].size() - 1) << '+' << "\n";
        for (int i = 0; i < intlen(matrix[0].size()); i++) {
            //cout << string(" ") * (pow(10, i));
            while (enumerate_row < matrix[0].size()) {
                cout <<
                    (normalize_int(enumerate_row, 3)[i] != '+' ? to_string(enumerate_row)[i] : ' ');
                enumerate_row++;
            }
            cout << "\n";
            enumerate_row = 0;
        }
    }
    cout << "\n\n\n";
    SetConsoleTextAttribute(pen, 7);
}

vector<vector<Pixel>> cut(vector<vector<Pixel>> matrix, int x1, int y1, int x2, int y2) {
    vector<vector<Pixel>> new_matrix(y2 - y1);
    int cnt = 0;
    for (int i = y1; i < matrix.size(); i++) {
        for (int j = x1; j < x2; j++) {
            new_matrix[cnt].push_back(matrix[i][j]);
        }
        cnt++;
    }
    return new_matrix;
}

void print_matrix_stat(vector<vector<Pixel>> matrix) {
    cout << "matrix: {\n\t";
    for (vector<Pixel> vec : matrix) {
        for (Pixel pxl : vec) {
            cout << pxl.BGRstr() << ", ";
        }
        cout << "\n\t";
    }
    cout << "}\n";
}

int main()
{
    Mat image = imread("C:/Users/Kaverin Vladimir/Pictures/cat.jpg");

    if (image.empty())
    {
        cout << "Image Not Found!!!" << endl;
        cin.get(); //wait for any key press
        return -1;
    }

    cout << image.size() << "\n";

    resize(image, image, Size(180, 50)); //шаблоны: квадрат - 120, 50, во весь экран - 200, 50
    Mat flat = image.reshape(1, image.total() * image.channels());
    cout << "rows: " << image.rows << " cols: " << image.cols << "\n";

    vector<vector<Pixel>> matrix(image.rows);
    vector<byte> matrix1D;

    matrix1D = flat;

    //cout << "matrix1D: { ";
    //for (byte num : matrix1D)
    //    cout << (int)num << " ";
    //cout << "}\n";

    int glob_i = 0;
    for_each(matrix.begin(), matrix.end(), //создание матрицы из классов Pixel
        [&image, &matrix1D, &glob_i](vector<Pixel>& vec) {
            int i;
            for (i = 0; i < image.cols * 3; i += 3) {
                vec.push_back(Pixel(
                    matrix1D[glob_i + i], 
                    matrix1D[glob_i + i + 1],
                    matrix1D[glob_i + i + 2]
                ));
            }
            glob_i += i;
        });

    //print_matrix_stat(matrix);

    printASCIIimage(matrix);
    //printASCIIimage(matrix, true);


    // Wait for any keystroke in the window
    waitKey(0);
    return 0;
}