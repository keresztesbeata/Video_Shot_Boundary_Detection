#include "stdafx.h"
#include "EdgeDetection.h"

Mat cannyEdgeDetector(Mat src) {

    if (src.channels() == 3) {
        // colour image should be converted to grayscale
        cvtColor(src, src, COLOR_BGR2GRAY);
    }
    blur(src, src, Size(3, 3));
    int lowThreshold = 0;
    int ratio = 3;
    int kernel_size = 3;
    Mat edges, detected_edges;
    edges = Scalar::all(0);
    Canny(src, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);
    src.copyTo(edges, detected_edges);
    
    return edges;
}


bool isEdgePixel(Mat img, int i, int j) {
    return img.at<uchar>(i, j) > 0;
}