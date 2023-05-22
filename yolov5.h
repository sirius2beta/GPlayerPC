#ifndef YOLOV5_H
#define YOLOV5_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <exception>
#include <QMessageBox>
#include <QDebug>
struct DetectResult {
    int classId;
    float score;
    cv::Rect box;
};

class YOLOV5 {
public:
    YOLOV5(){};
    void initConfig(std::string onnxpath, int iw, int ih, float threshold);
    void detect2(cv::Mat & frame);


private:
    std::string classeName[80] = {"person", "bicycle", "car", "motorbike", "aeroplane", "bus",
                              "train", "truck", "boat", "traffic light", "fire hydrant",
                              "stop sign", "parking meter", "bench", "bird", "cat", "dog",
                              "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe",
                              "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
                              "skis", "snowboard", "sports ball", "kite", "baseball bat",
                              "baseball glove", "skateboard", "surfboard", "tennis racket",
                              "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl",
                              "banana", "apple", "sandwich", "orange", "broccoli", "carrot",
                              "hot dog", "pizza", "donut", "cake", "chair", "sofa", "pottedplant",
                              "bed", "diningtable", "toilet", "tvmonitor", "laptop", "mouse",
                              "remote", "keyboard", "cell phone", "microwave", "oven", "toaster",
                              "sink", "refrigerator", "book", "clock", "vase", "scissors",
                              "teddy bear", "hair drier", "toothbrush"};

    int input_w = 640;
    int input_h = 640;
    cv::dnn::Net net;
    int threshold_score = 0.25;
};

#endif // YOLOV5_H
