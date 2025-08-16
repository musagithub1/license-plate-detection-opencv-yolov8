#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace dnn;
using namespace std;

int main() {
    // Load ONNX YOLOv8 model
    Net net = readNetFromONNX("yolov8n.onnx");
    if (net.empty()) {
        cerr << "❌ Could not load YOLOv8n.onnx" << endl;
        return -1;
    }
    cout << "✅ Model loaded successfully!" << endl;

    // Load input image
    Mat img = imread("Plates/car.jpg");
    if (img.empty()) {
        cerr << "❌ Could not load image!" << endl;
        return -1;
    }

    // Preprocess (resize + normalize)
    Mat blob;
    Size inputSize(640, 640);  // YOLOv8 default
    blobFromImage(img, blob, 1/255.0, inputSize, Scalar(), true, false);

    // Forward pass
    net.setInput(blob);
    Mat output = net.forward();

    // Output shape: [batch, anchors, values]
    // YOLOv8 format: [x, y, w, h, conf, class scores...]
    const int dimensions = output.size[2];
    const int rows = output.size[1];
    cout << "Output shape: " << rows << " x " << dimensions << endl;

    // Convert to 2D Mat
    Mat outputMat(rows, dimensions, CV_32F, output.ptr<float>());

    // Thresholds
    float confThreshold = 0.5;
    float nmsThreshold = 0.4;

    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;

    // Post-processing
    for (int i = 0; i < rows; i++) {
        float confidence = outputMat.at<float>(i, 4);
        if (confidence >= confThreshold) {
            float x = outputMat.at<float>(i, 0);
            float y = outputMat.at<float>(i, 1);
            float w = outputMat.at<float>(i, 2);
            float h = outputMat.at<float>(i, 3);

            int left = int((x - 0.5 * w) * img.cols / inputSize.width);
            int top = int((y - 0.5 * h) * img.rows / inputSize.height);
            int width = int(w * img.cols / inputSize.width);
            int height = int(h * img.rows / inputSize.height);

            boxes.push_back(Rect(left, top, width, height));
            confidences.push_back(confidence);
            classIds.push_back(0); // dummy (if you only care about detection)
        }
    }

    // Non-maximum suppression
    vector<int> indices;
    NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);

    // Draw boxes
    for (int i : indices) {
        rectangle(img, boxes[i], Scalar(0, 255, 0), 2);
        putText(img, "Plate", boxes[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
    }

    // Show result
    imshow("YOLOv8 Detection", img);
    waitKey(0);

    return 0;
}
