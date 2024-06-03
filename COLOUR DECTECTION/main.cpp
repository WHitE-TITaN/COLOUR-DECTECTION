#include <iostream>
#include "COLOUR_DETECTION.h"



int main() {
    int num;
    std::cout << "\n\n\t\t 1 - For Capture And Save \n\t\t 2 - for Showing the Captured data \n\t\t 3 - To dectect the specified colour \n\t\t 4 - to Capture Screen and show the Dectected Colours \n\n\t\tenter the no. -> ";
    std::cin >> num;
    if (num == 1) {
        if (SaveBitmapToFile() == 0) {
            std::cout << "\n \n \t\t<< THE DATA HAS BEEN SAVED SUCCESSFULLY >>";
        }
        else {
            std::cout << "\n \n \t\t<<   !!!! UNSUCCESSFULL !!!!   >>";
            return 1;
        }
    }
    else if (num == 2) {
        if (initializeDc() != 0) {
            std::cout << "<<FAIL";
            return 1;
        }
        else {
            return showContent();
        }
    }
    else if (num == 3) {
        initializeDc();
        initializeDetect();
        return 0;
    }
    else if (num == 4) {

        while (true) {
            //capture the screen and convert it into cvmatFormat ->>
            cv::Mat frame = captureScreenForCv();
            cv::Scalar targetColor(0, 255, 255); // colour red to search ->>
            int tolerance = 100;

            // Process the frame to search for the target color
            colorSearchOpenCv(frame, targetColor, tolerance);

            // Show the processed frame in an OpenCV window
            cv::imshow("Processed Frame", frame);

            if (cv::waitKey(10) == 'q') {
                break;
            }
        }
    }
}