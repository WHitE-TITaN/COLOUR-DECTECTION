#include <iostream>
#include <windows.h>
#include <sstream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include "COLOUR_DETECTION.h"





// Global variables ->>


HDC hScreenDC = NULL;
HDC hMemoryDC = NULL;


// Get the width and height of the screen
int screenWidth = 0;
int screenHeight = 0;


// Select the compatible bitmap into the memory DC
HBITMAP hBitmap = NULL;


// At this point, hBitmap contains the screen capture
HBITMAP hOldBitmap = NULL;

HWND hWnd = NULL;





int initializeDc() {
    // Get the device context of the screen
    hScreenDC = GetDC(NULL);
    if (!hScreenDC) {
        std::cerr << "Failed to get screen device context" << std::endl;
        return 1;
    }

    // Create a device context to use as a target for the bit block transfer
    hMemoryDC = CreateCompatibleDC(hScreenDC);
    if (!hMemoryDC) {
        std::cerr << "Failed to create memory device context" << std::endl;
        ReleaseDC(NULL, hScreenDC);
        return 1;
    }

    screenWidth = GetDeviceCaps(hScreenDC, HORZRES);
    screenHeight = GetDeviceCaps(hScreenDC, VERTRES);

    // Create a compatible bitmap from the Screen DC
    hBitmap = CreateCompatibleBitmap(hScreenDC, screenWidth, screenHeight);
    if (!hBitmap) {
        std::cerr << "Failed to create compatible bitmap" << std::endl;
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return 1;
    }

    hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);
    return 0;
}







// main capturing frames ->>
bool mainCapture() {
    if (!BitBlt(hMemoryDC, 0, 0, screenWidth, screenHeight, hScreenDC, 0, 0, SRCCOPY)) {
        std::cerr << "BitBlt has failed" << std::endl;
        return false;
    }
    std::cout << "\ncaptured Successfull!";
    return true;
}












// screen capture for cv->>
cv::Mat captureScreenForCv() {


    HDC hScreenDC = GetDC(NULL); // Get the device context of the screen
    int screenWidth = GetSystemMetrics(SM_CXSCREEN); // Get the screen width
    int screenHeight = GetSystemMetrics(SM_CYSCREEN); // Get the screen height

    // Create a compatible memory device context
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    if (!hMemoryDC) {
        ReleaseDC(NULL, hScreenDC);
        return cv::Mat();
    }

    // Create a compatible bitmap
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, screenWidth, screenHeight);
    if (!hBitmap) {
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return cv::Mat();
    }

    // Select the bitmap into the memory device context
    HGDIOBJ hOldBitmap = SelectObject(hMemoryDC, hBitmap);

    // Copy the screen content to the memory device context
    if (!BitBlt(hMemoryDC, 0, 0, screenWidth, screenHeight, hScreenDC, 0, 0, SRCCOPY)) {
        SelectObject(hMemoryDC, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return cv::Mat();
    }

    // Create an OpenCV Mat from the bitmap
    cv::Mat frame(screenHeight, screenWidth, CV_8UC4);
    GetBitmapBits(hBitmap, frame.total() * frame.elemSize(), frame.data);

    // Release resources
    SelectObject(hMemoryDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);

    // Convert BGR to RGB format
    cv::cvtColor(frame, frame, cv::COLOR_BGRA2BGR);

    return frame;

}















// defining SaveBitmap function ->> 
void SaveBitmapToFile(HBITMAP hBitmap, HDC hDC, const std::wstring& filePath) {
    BITMAP bmp;
    PBITMAPINFO pbmi;
    WORD cClrBits;
    HANDLE hf;                  // file handle
    BITMAPFILEHEADER hdr;       // bitmap file-header
    PBITMAPINFOHEADER pbih;     // bitmap info-header
    LPBYTE lpBits;              // memory pointer
    DWORD dwTotal;              // total count of bytes
    DWORD cb;                   // incremental count of bytes
    BYTE* hp;                   // byte pointer
    DWORD dwTmp;



    if (!GetObject(hBitmap, sizeof(BITMAP), &bmp)) {
        std::cerr << "GetObject failed" << std::endl;
        return;
    }

    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if (cClrBits == 1) {
        cClrBits = 1;
    }
    else if (cClrBits <= 4) {
        cClrBits = 4;
    }
    else if (cClrBits <= 8) {
        cClrBits = 8;
    }
    else if (cClrBits <= 16) {
        cClrBits = 16;
    }
    else if (cClrBits <= 24) {
        cClrBits = 24;
    }
    else {
        cClrBits = 32;
    }

    pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));
    if (!pbmi) {
        std::cerr << "LocalAlloc failed" << std::endl;
        return;
    }

    pbih = (PBITMAPINFOHEADER)pbmi;
    memset(pbih, 0, sizeof(BITMAPINFOHEADER)); // Initialize pbih to all zeros

    pbih->biSize = sizeof(BITMAPINFOHEADER);
    pbih->biWidth = bmp.bmWidth;
    pbih->biHeight = bmp.bmHeight;
    pbih->biPlanes = bmp.bmPlanes;
    pbih->biBitCount = bmp.bmBitsPixel;
    if (cClrBits < 24) {
        pbih->biClrUsed = (1 << cClrBits);
    }

    if (cClrBits != 24) {
        pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (pbih->biClrUsed = static_cast<DWORD>(1) << cClrBits));
    }
    else {
        pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));
    }

    if (!pbmi) {
        std::cerr << "LocalAlloc failed" << std::endl;
        return;
    }

    // initilization of pbih ->>
    pbih = (PBITMAPINFOHEADER)pbmi;

    pbih->biSize = sizeof(BITMAPINFOHEADER);
    pbih->biWidth = bmp.bmWidth;
    pbih->biHeight = bmp.bmHeight;
    pbih->biPlanes = bmp.bmPlanes;
    pbih->biBitCount = bmp.bmBitsPixel;
    if (cClrBits < 24) {
        pbih->biClrUsed = (1 << cClrBits);
    }

    pbih->biCompression = BI_RGB;
    pbih->biSizeImage = ((pbih->biWidth * cClrBits + 31) & ~31) / 8 * pbih->biHeight;
    pbih->biClrImportant = 0;

    lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
    if (!lpBits) {
        std::cerr << "GlobalAlloc failed" << std::endl;
        return;
    }

    if (!GetDIBits(hDC, hBitmap, 0, (WORD)pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS)) {
        std::cerr << "GetDIBits failed" << std::endl;
        return;
    }

    hf = CreateFile(filePath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hf == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateFile failed" << std::endl;
        return;
    }

    hdr.bfType = 0x4d42;  // "BM"
    hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;
    hdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD);

    if (!WriteFile(hf, &hdr, sizeof(BITMAPFILEHEADER), &dwTmp, NULL)) {
        std::cerr << "WriteFile (BITMAPFILEHEADER) failed" << std::endl;
        return;
    }

    if (!WriteFile(hf, pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD), &dwTmp, NULL)) {
        std::cerr << "WriteFile (BITMAPINFOHEADER) failed" << std::endl;
        return;
    }

    dwTotal = cb = pbih->biSizeImage;
    hp = lpBits;
    if (!WriteFile(hf, hp, cb, &dwTmp, NULL)) {
        std::cerr << "WriteFile (bitmap data) failed" << std::endl;
        return;
    }

    if (!CloseHandle(hf)) {
        std::cerr << "CloseHandle failed" << std::endl;
        return;
    }

    GlobalFree(lpBits);
    LocalFree(pbmi);


}















// finializing save and returning values ->>
int SaveBitmapToFile() {
    // initialize variables to store 
    initializeDc();
    // Bit block transfer into our compatible memory DC
    for (int i = 0; i < 10; ++i) {

        mainCapture();
        std::wostringstream oss;
        oss << L"screenshot_" << std::setw(2) << std::setfill(L'0') << i << L".bmp";
        std::wstring filePath = oss.str();
        SaveBitmapToFile(hBitmap, hMemoryDC, filePath);
    }

    SelectObject(hMemoryDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);
    return 0;
}









// to update the frames to the window ->>
void PaintFrame(HDC hdc) {
    // Select the captured bitmap into the memory device context
    HGDIOBJ hOld = SelectObject(hMemoryDC, hBitmap);

    // Determine the size of the captured bitmap
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);
    int width = bmp.bmWidth;
    int height = bmp.bmHeight;

    // Paint the captured bitmap onto the window's device context
    BitBlt(hdc, 0, 0, width, height, hMemoryDC, 0, 0, SRCCOPY);


    //converting the frames ->>
    cv::Mat frame = HBITMAPToMat(hBitmap);

    // Define target color (HSV values for red)
    cv::Scalar targetColor(0, 255, 255); // Example: Hue value for red
    int tolerance = 10;

    // Search for the target color in the captured frame and outline it
    if (colorSearchOpenCv(frame, targetColor, tolerance)) {
        std::cout << "Target color found!" << std::endl;
    }
    else {
        std::cout << "Target color not found." << std::endl;
    }

    // Show the processed frame (optional for debugging)
    cv::imshow("Detected Frame", frame);
    cv::waitKey(1);





    // printing custom message on the screen ->>>
    SetTextColor(hdc, RGB(255, 0, 0)); // Red text color
    SetBkMode(hdc, TRANSPARENT); // Transparent background for text

    // Define the custom message
    LPCWSTR message = L"Custom Message: Frame captured by WHITE_TITAN ->> successfully";

    // Draw the custom message on the window's device context
    TextOut(hdc, 10, 10, message, wcslen(message));

    // Clean up
    SelectObject(hMemoryDC, hOld);
}





// to create the new windows ->>
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        if (mainCapture()) {
            PaintFrame(hdc); // Paint the captured frame onto the window
        }
        else {
            std::cerr << "mainCapture failed" << std::endl;
        }

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);

        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}





// regestire windows ->>>
int showContent() {
    const wchar_t CLASS_NAME[] = L"MyWindowClass";
    initializeDc();

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClass(&wc)) {
        std::cerr << "Failed to register window class" << std::endl;
        return 1;
    }

    HWND hWnd = CreateWindowEx(0, CLASS_NAME, L"Captured Frame", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, screenWidth, screenHeight,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    if (!hWnd) {
        std::cerr << "Failed to create window" << std::endl;
        return 1;
    }

    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        InvalidateRect(hWnd, NULL, FALSE);  // Force window to repaint
    }

    SelectObject(hMemoryDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);

    return 0;
}