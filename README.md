# Screen Capture and Color Detection using OpenCV

## Overview

This project demonstrates how to capture the screen, process the captured frames to detect specific colors, and display the results using OpenCV in C++. The primary goal is to detect objects of a specified color within the captured screen frames and highlight them with bounding rectangles.

## Features

- **Screen Capture**: Uses Win32 API to capture the screen content.
- **Color Detection**: Uses OpenCV to detect a specified color within the captured frames.
- **Real-time Processing**: Continuously captures and processes frames in real-time.
- **Visualization**: Displays the processed frames in an OpenCV window with detected objects highlighted.

## Requirements

- Windows OS
- Microsoft Visual Studio
- OpenCV (version 4.9.0 or compatible)

## Installation

1. **Clone the repository**:
    ```bash
    git clone https://github.com/WHitE-TITaN/COLOUR-DECTECTION.git
    cd screen-capture-color-detection
    ```

2. **Install OpenCV**:
    - Download and install OpenCV from [OpenCV's official website](https://opencv.org/).
    - Follow the installation instructions for your version of Visual Studio.

3. **Configure Visual Studio**:
    - Open the project in Visual Studio.
    - Add OpenCV include and library directories to the project settings.
    - Link against OpenCV libraries (`opencv_world490.lib` or the appropriate version).

## Usage

1. **Build and Run**:
    - Build the project in Visual Studio.
    - Run the executable.

2. **Adjust Parameters**:
    - Modify the target color and tolerance in the `main` function to detect different colors.
    - Adjust the delay in the `cv::waitKey` function for different frame processing speeds.

## Code Overview

- **main.cpp**:
    - Contains the main function that sets up the screen capture and processing loop.
    - Captures the screen using `captureScreen`.
    - Processes each frame to detect a specified color using `colorSearchOpenCv`.
    - Displays the processed frame in an OpenCV window.

- **captureScreen**:
    - Captures the current screen content and returns it as an OpenCV `cv::Mat`.

- **colorSearchOpenCv**:
    - Converts the captured frame to HSV color space.
    - Applies a color threshold to create a binary mask.
    - Finds contours in the binary mask and draws bounding rectangles around detected objects.
 
## Contributing
Contributions are welcome! If you have any improvements or suggestions, feel free to open an issue or create a pull request.
