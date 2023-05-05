#include <opencv2/opencv.hpp>
#include <cmath>

/** linearTransform.cpp
 * 
 *  Contains linear transformation functions for cv::Mat objects.
 *  Each function will return a 3x3 transform matrix.
 *  Uses homogeneous co-ordinates to simplify complex 2D transforms.
 */

cv::Mat translationMatrix(double dx, double dy) {
    //initializes a 3x3 identity matrix
    cv::Mat T = cv::Mat::eye(3, 3, CV_64F);
    //add the translation vector to the identity matrix
    T.at<double>(0, 2) = dx;
	T.at<double>(1, 2) = dy;

    return T;
}

cv::Mat scaleMatrix(double s) {
    //initializes a 3x3 identity matrix
    cv:: Mat T = cv::Mat::eye(3, 3, CV_64F);
    //adds in the scale values
    T.at<double>(0, 0) = s;
    T.at<double>(1, 1) = s;

    return T;
}

bool isValid(const cv::Vec3d& xyz) {
    // Check for NaNs etc. in Z
    if (!isnormal(xyz[2]) || !isnormal(xyz[1]) || !isnormal(xyz[0])) {
        return false;
    }
    // Other checks can go here
    return true;
}

cv::Mat rotationMatrix(double angle){
    //converts angle to radians
    const double pi = 3.14159265358979311599796346854;
    double rad = (angle * pi) / 180;

    //sets up a 3x3 identity matrix
    cv::Mat T = cv::Mat::eye(3, 3, CV_64F);
    //adds in the rotation matrix
    T.at<double>(0, 0) = cos(rad);
    T.at<double>(1, 1) = cos(rad);
    T.at<double>(0, 1) = -sin(rad);
    T.at<double>(1, 0) = sin(rad);

    return T;
    
}

cv::Mat combineTask(double dx, double dy, double angle) {
    cv::Mat T = cv::Mat::eye(3, 3, CV_64F); 
    T = T * translationMatrix(dx, dy);
    T = T * rotationMatrix(angle);
    T = T * translationMatrix(-dx, -dy);

    return T;
}

//Converts 2d point to a 3d homogeneous vector
cv::Mat homogen(double x, double y){
    cv::Mat T(3, 1, CV_64F);

    T.at<double>(0, 0) = x;
    T.at<double>(1, 0) = y;
    T.at<double>(2, 0) = 1;

    return T;
}