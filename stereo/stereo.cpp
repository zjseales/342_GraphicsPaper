#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include "CalibrationIO.h"

/** stereo.cpp
 *  COSC342 Assignment 2
 *  
 *  Computes and displays the disparity map of rectified image pairs.
 *  The matching method used is dependent on the 'bm' boolean.
 *  Zac Seales, seaza886, 6687905
 */

int main(int argc, char *argv[]) {
	// original image size
	cv::Size imageSize(1920, 1080);
	//the disparity matrix
	cv::Mat disparity;
	//the maximum disparity
	int maxDisparity = 64;
	//The blockSize parameter
	int blockSize = 21;
	//true if blockmatching, false if semi-global matching
	bool bm = false;

	//file names
	std::string calibrationFile = "../calibration.txt";
	std::string image1File = "../pics/bell_left.jpg";
	std::string image2File = "../pics/bell_right.jpg";

	//the Semi-Global Matching method
	cv::Ptr<cv::StereoSGBM> sgMatcher = cv::StereoSGBM::create(0, maxDisparity, blockSize);
	sgMatcher->setMode(2);
	sgMatcher->setP1(8 * blockSize * blockSize);
	sgMatcher->setP2(32 * blockSize * blockSize);
	//the Block matching method
	cv::Ptr<cv::StereoBM> blockMatcher = cv::StereoBM::create(maxDisparity, blockSize);

	// Camera Calibration
	cv::Mat K1, K2, R, T;
	std::vector<double> d1, d2;
	readStereoCalibration(calibrationFile, K1, d1, K2, d2, R, T);

	std::cout << "K1" << std::endl << K1 << std::endl;
	std::cout << "K2" << std::endl << K2 << std::endl;
	std::cout << "R" << std::endl << R << std::endl;
	std::cout << "T" << std::endl << T << std::endl;

	cv::Mat I1 = cv::imread(image1File);
	cv::Mat I2 = cv::imread(image2File);

	//Rectify Images
	cv::Mat R1, R2, P1, P2, Q;
	cv::stereoRectify(K1, d1, K2, d2, imageSize, R, T, R1, R2, P1, P2, Q);

	// distortion correction
	cv::Mat map1a, map1b, map2a, map2b;
	cv::initUndistortRectifyMap(K1, d1, R1, P1, imageSize, CV_32F, map1a, map1b);
	cv::initUndistortRectifyMap(K2, d2, R2, P2, imageSize, CV_32F, map2a, map2b);

	//warp images to rectified pair
	cv::Mat rect1, rect2;
	cv::remap(I1, rect1, map1a, map1b, cv::INTER_LINEAR);
	cv::remap(I2, rect2, map2a, map2b, cv::INTER_LINEAR);

	//rescale images
	cv::Mat small1, small2;
	cv::resize(rect1, small1, cv::Size(), 0.25, 0.25, cv::INTER_AREA);
	cv::resize(rect2, small2, cv::Size(), 0.25, 0.25, cv::INTER_AREA);

	//convert to greyscale
	cv::Mat grey1, grey2;
	cv::cvtColor(small1, grey1, cv::COLOR_BGR2GRAY);
	cv::cvtColor(small2, grey2, cv::COLOR_BGR2GRAY);

	// Disparity compute method
	if (bm){
		blockMatcher->compute(grey1, grey2, disparity);
	} else {
		sgMatcher->compute(grey1, grey2, disparity);
	}
	
	// Scale disparity up to full size
	cv::Mat disparityFull;
	cv::resize(disparity, disparityFull, imageSize, 0, 0, cv::INTER_NEAREST);
	// Convert to floating point fractions for disparity
	cv::Mat disparityFloat(imageSize, CV_32F);
	disparityFull.convertTo(disparityFloat, CV_32F, 1./16.0);
	//Compute 3D points
	cv::Mat points3D(imageSize, CV_32FC3);
	cv::reprojectImageTo3D(disparityFloat, points3D, Q);

	std::ofstream fout("model.ply");
	fout << "ply\n"
		<< "format ascii 1.0\n"
		<< "element vertex " << points3D.size().area() << "\n"
		<< "property float x\n"
		<< "property float y\n"
		<< "property float z\n"
		<< "property uchar red\n"
		<< "property uchar green\n"
		<< "property uchar blue\n"
		<< "end_header\n";

	for (int y = 0; y < points3D.size().height; ++y) {
		for (int x = 0; x < points3D.size().width; ++x) {
			if (isValid(points3D.at<cv::Vec3f>(y, x))){
				// Write location data for this point
				cv::Vec3f xyz = points3D.at<cv::Vec3f>(y, x);
				fout << xyz[0] << " " << xyz[1] << " " << xyz[2] << " ";
				// Write colour data for this point
				cv::Vec3b rgb = I1.at<cv::Vec3b>(y, x);
				// Note: OpenCV uses BGR colour
				fout << int(rgb[2]) << " " << int(rgb[1])
				<< " " << int(rgb[0]) << "\n";
			} else {
				// Write location data for this point
				cv::Vec3f xyz(0,0,0);
				fout << xyz[0] << " " << xyz[1] << " " << xyz[2] << " ";
				// Write colour data for this point
				cv::Vec3b rgb = I1.at<cv::Vec3b>(y, x);
				// Note: OpenCV uses BGR colour
				fout << int(rgb[2]) << " " << int(rgb[1])
				<< " " << int(rgb[0]) << "\n";
			}
		}
	}
	fout.close();

	cv::waitKey();

	return 0;

}
