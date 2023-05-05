#include <opencv2/opencv.hpp>

/** Holds commonly used functions
 *  Simplifies the main method.
 */

/* Reads in and returns the filename as an image matrix */
cv::Mat loadImage(char* filename){
	cv::Mat image = cv::imread(filename);
	if (image.empty()) {
		std::cerr << "Could not load image from image1.jpg" << std::endl;
		return cv::Mat::eye(1, 1, CV_64F);
	}
    return image;
}

cv::Mat homogen(double x, double y){
    cv::Mat T(3, 1, CV_64F);

    T.at<double>(0, 0) = x;
    T.at<double>(1, 0) = y;
    T.at<double>(2, 0) = 1;

    return T;
}

cv::Mat translationMatrix(double dx, double dy) {
    //initializes a 3x3 identity matrix
    cv::Mat T = cv::Mat::eye(3, 3, CV_64F);
    //add the translation vector to the identity matrix
    T.at<double>(0, 2) = dx;
	T.at<double>(1, 2) = dy;

    return T;
}