#ifndef COLOUR_DECTECTION_H
#define COLOUR_DECTECTION_H

#include <windows.h>
#include <string>
#include <opencv2/opencv.hpp>


// global variables ->> 
extern HDC hScreenDC;
extern HDC hMemoryDC;
extern HBITMAP hBitmap;   //<<- the main bitmap data that contains pixel data
extern HBITMAP hOldBitmap;
extern int screenWidth;
extern int screenHeight;
extern POINT p;



//functions ->>
void SaveBitmapToFile(HBITMAP hBitmap, HDC hDC, const std::wstring& filePath); // screen capture file ->>
bool mainCapture();
cv::Mat captureScreenForCv();
int SaveBitmapToFile(); int showContent();
int initializeDc();
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


bool searchColorInFrame(COLORREF targetColor, int tolerance); //dectionColout file 
bool initializeDetect();
bool colorSearchOpenCv(cv::Mat& frame, const cv::Scalar& targetColor, int tolerance);
cv::Mat HBITMAPToMat(HBITMAP hBitmap);

#endif