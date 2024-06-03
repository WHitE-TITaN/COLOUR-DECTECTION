#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "COLOUR_DETECTION.h"


bool searchColorInFrame(COLORREF targetColor, int tolerance = 0) {
    BITMAP bmp;
    if (!GetObject(hBitmap, sizeof(BITMAP), &bmp)) {
        std::cerr << "GetObject failed" << std::endl;
        return false;
    }

    BITMAPINFOHEADER bi = { 0 };
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = bmp.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32; // We will use a 32-bit bitmap (RGB + Alpha channel)
    bi.biCompression = BI_RGB;

    int bmpSize = bmp.bmWidth * bmp.bmHeight * 4;
    std::vector<BYTE> pixels(bmpSize);

    if (!GetDIBits(hMemoryDC, hBitmap, 0, bmp.bmHeight, pixels.data(), reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS)) {
        std::cerr << "GetDIBits failed" << std::endl;
        return false;
    }
    HDC hdc = GetDC(NULL);
    bool colorFound = false;

    for (int y = 0; y < bmp.bmHeight; ++y) {
        for (int x = 0; x < bmp.bmWidth; ++x) {
            int index = (y * bmp.bmWidth + x) * 4;
            BYTE blue = pixels[index];
            BYTE green = pixels[index + 1];
            BYTE red = pixels[index + 2];

            COLORREF color = RGB(red, green, blue);

            if (abs(GetRValue(targetColor) - red) <= tolerance &&
                abs(GetGValue(targetColor) - green) <= tolerance &&
                abs(GetBValue(targetColor) - blue) <= tolerance) {
                std::cout << "Color detected at (" << x << ", " << y << ")" << std::endl;


                RECT rect = { x - 5, y - 5, x + 5, y + 5 }; // Modify rectangle size as needed
                DrawEdge(hdc, &rect, BDR_RAISEDINNER, BF_RECT); // Draw rectangle
                colorFound = true;
            }
        }
    }

    if (!colorFound) {
        std::cout << "Target color not found." << std::endl;
    }

    return colorFound;
}







// calling the dectaction ->>


bool initializeDetect() {
    mainCapture();
    COLORREF targetColor = RGB(255, 0, 0); // Red color
    int tolerance = 10;


    //calling the detect function->>
    if (searchColorInFrame(targetColor, tolerance)) {
        std::cout << "Target color found!" << std::endl;
    }
    else {
        std::cout << "Target color not found." << std::endl;
    }

    // Cleanup
    SelectObject(hMemoryDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);

    return 0;

}









// converting the hbitmap to cv::mat data for processing ->
cv::Mat HBITMAPToMat(HBITMAP hBitmap) {
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = -bmp.bmHeight;  // negative to indicate top-down bitmap
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    cv::Mat mat(bmp.bmHeight, bmp.bmWidth, CV_8UC4);
    GetDIBits(GetDC(0), hBitmap, 0, bmp.bmHeight, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    return mat; // returning the cv mat converted data->
}














// searching the colour ->>
bool colorSearchOpenCv(cv::Mat& frame, const cv::Scalar& targetColor, int tolerance = 10) {
    cv::Mat hsvFrame;
    cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV); // Convert frame to HSV color space

    cv::Scalar lowerBound(targetColor[0] - tolerance, 100, 100);
    cv::Scalar upperBound(targetColor[0] + tolerance, 255, 255);

    cv::Mat mask;
    cv::inRange(hsvFrame, lowerBound, upperBound, mask); // Create binary mask of target color

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    if (contours.empty()) {
        return false;
    }

    for (const auto& contour : contours) {
        cv::Rect boundingRect = cv::boundingRect(contour);
        cv::rectangle(frame, boundingRect, cv::Scalar(0, 255, 0), 2); // Draw green rectangle
    }



    return true;
}

