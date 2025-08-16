#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <iostream>
#include <fstream>
#include <ctime>

using namespace cv;
using namespace std;

/////////////// License Plate Detector + OCR //////////////////////

int main() {
    Mat img;
    VideoCapture cap(0);   // open webcam
    Mat imgGray, imgBlur, imgThresh;
    CascadeClassifier plateCascade;
    plateCascade.load("Resources/haarcascade_russian_plate_number.xml");

    if (plateCascade.empty()) {
        cout << "XML file not loaded" << endl;
        return -1;
    }

    // Initialize Tesseract
    tesseract::TessBaseAPI *ocr = new tesseract::TessBaseAPI();
    if (ocr->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY)) {
        cout << "Could not initialize tesseract." << endl;
        return -1;
    }

    vector<Rect> plates;

    while (true) {
        cap.read(img);
        if (img.empty()) break;

        plateCascade.detectMultiScale(img, plates, 1.1, 10);

        for (int i = 0; i < plates.size(); i++) {
            Mat imgCrop = img(plates[i]);

            // Preprocessing
            cvtColor(imgCrop, imgGray, COLOR_BGR2GRAY);
            GaussianBlur(imgGray, imgBlur, Size(3, 3), 0);
            threshold(imgBlur, imgThresh, 0, 255, THRESH_BINARY + THRESH_OTSU);

            // OCR
            ocr->SetImage(imgThresh.data, imgThresh.cols, imgThresh.rows, 1, imgThresh.step);
            string plateText = string(ocr->GetUTF8Text());

            // Draw detection
            rectangle(img, plates[i], Scalar(0, 255, 0), 2);
            putText(img, plateText, Point(plates[i].x, plates[i].y - 10),
                FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);

            // Save plate crop
            imwrite("Resources/Plates/" + to_string(i) + ".png", imgCrop);

            // Save to CSV with timestamp
            ofstream file("plates.csv", ios::app);
            time_t now = time(0);
            file << plateText << "," << ctime(&now);
            file.close();
        }

        imshow("Camera", img);

        if (waitKey(1) == 27) break; // Press ESC to exit
    }

    ocr->End();
    return 0;
}
