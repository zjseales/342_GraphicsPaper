#include <opencv2/xfeatures2d.hpp>
#include <opencv2/opencv.hpp>
#include "Timer.h"
#include "MyCode.cpp"

int main(int argc, char* argv[]) {

	Timer myTimer;
	myTimer.reset();

	cv::Mat image1 = loadImage("../img7a.jpeg");
	cv::Mat image2 = loadImage("../img7b.jpeg");
	//cv::Mat image1 = loadImage("../../../Assignment1/img6b.jpeg");
	//cv::Mat image2 = loadImage("../../../Assignment1/img6a.jpeg");
	std::cout << "Took " << myTimer.read() << " seconds to load the 2 images" << std::endl;

	//create a smart pointer feature detector
	cv::Ptr<cv::FeatureDetector> detector = cv::SIFT::create();
	//Create a FlannBased feature matcher
	cv::Ptr<cv::DescriptorMatcher> matcher2 = cv::FlannBasedMatcher::create();

	std::vector<cv::KeyPoint> keypoints1, keypoints2;
	cv::Mat descriptors1, descriptors2;

	//reset timer
	myTimer.reset();
	//detect features from image1
	detector->detectAndCompute(image1, cv::noArray(), keypoints1, descriptors1);
	//output total number found
	std::cout << "Found " << keypoints1.size() << " features on image 1." << std::endl;
/**	
	//draw image 1 features
	cv::drawKeypoints(image1, keypoints1, image1, cv::Scalar(0,255,0),
				cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
*/
	//detect image2 features
	detector->detectAndCompute(image2, cv::noArray(), keypoints2, descriptors2);
	//output total features found in image2
	std::cout << "Found " << keypoints2.size() << " features on image 2." << std::endl;
/**
	//draw the features onto image 2
	cv::drawKeypoints(image2, keypoints2, image2, cv::Scalar(0,255,0),
				cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
*/
	//output time
	std::cout << "Took " << myTimer.read() << " seconds to detect and draw features of both images." << std::endl;

	//RESET TIMER TO TEST FLANN BASED MATCHING
	std::vector<std::vector<cv::DMatch>> matches2;
	myTimer.reset();
	//find the 2 best matches using flann based matching
	matcher2->knnMatch(descriptors1, descriptors2, matches2, 2);
	//holds locations of matching features
	std::vector<cv::Point2f> goodPts1, goodPts2;
	//the best match of the 2 matches found

	//iterate all matches to filter out the worst of the 2 matches
	std::vector<cv::DMatch> goodMatches2;
	for (const auto& match : matches2) {
		if (match[0].distance < 0.8 * match[1].distance) {
			goodMatches2.push_back(match[0]);
			goodPts1.push_back(keypoints1[match[0].queryIdx].pt);
			goodPts2.push_back(keypoints2[match[0].trainIdx].pt);
		}
	}
	//OUTPUT Flann Results
	std::cout << "Took " << myTimer.read() << " seconds for Flann-Based feature matching." << std::endl;
	std::cout << "FlannBased Matching resulted in " << matches2.size() << " matches before filtering." << std::endl;
	std::cout << "FlannBased Matching resulted in " << goodMatches2.size() << " matches after filtering." << std::endl;
/**
	//draw FB Matches
	cv::Mat matchImg2;
	cv::drawMatches(image1, keypoints1, image2, keypoints2, goodMatches2, matchImg2);
	cv::namedWindow("Flann Matches");
	cv::imshow("Flann Matches", matchImg2);
*/
	//NOTE:
	//queryIdx refers to the det of features from the queried image
	//trainIdx refers to the features of the training image
	//we want to match features from the queried image, to the set of features in the training image.

	/** Find the homography that warps image2 to image1. */
	std::vector<unsigned char> inliers;
	cv::Mat H = cv::findHomography(goodPts2, goodPts1, inliers, cv::RANSAC, 3.0);
	std::cout << H << std::endl;

	int count = 0;
	for (int i = 0; i < inliers.size(); i++){
		if (inliers.at(i) == 1){
			count++;
		}
	}

	std::cout << "total inliers = " << inliers.size() << std::endl;

	for (auto i : inliers){
		std::cout << i << " " << std::endl;
	}

    //stores corners of the image so resulting frame will fit
	cv::Mat corners[4];
	corners[0] = homogen(0,0);
	corners[1] = homogen(image2.size().width, 0);
	corners[2] = homogen(image2.size().width, image2.size().height);
	corners[3] = homogen(0, image2.size().height);
/**
	//transform the corners
	for(int i = 0; i < 4; i++){
		corners[i] = H * corners[i];
	}
	//checks for smallest and largest (u,v) co-ordinates
	double minU = corners[0].at<double>(0, 0);
	double maxU = corners[0].at<double>(0, 0);
	double minV = corners[0].at<double>(0, 1);
	double maxV = corners[0].at<double>(0, 1);
	for(int i = 1; i < 4; i++){
		if(corners[i].at<double>(0, 0) < minU){
			minU = corners[i].at<double>(0, 0);
		}
		if(corners[i].at<double>(0, 0) > maxU){
			maxU = corners[i].at<double>(0, 0);
		}
		if(corners[i].at<double>(0, 1) < minV){
			minV = corners[i].at<double>(0, 1);
		}
		if(corners[i].at<double>(0, 0) > maxV){
			maxV = corners[i].at<double>(0, 1);
		}
	}
	cv::Mat T = translationMatrix(-minU, -minV);

	//clone image1 to display final result
	cv::Mat mosaic(maxV - minV, image1.size().width - minU, CV_8UC3);
	cv::warpPerspective(image1, mosaic, T, mosaic.size(),
						cv::INTER_NEAREST, cv::BORDER_CONSTANT, cv::Scalar(255, 0, 0));
	cv::warpPerspective(image2, mosaic,T * H , mosaic.size(),
						cv::INTER_NEAREST, cv::BORDER_TRANSPARENT);
	//OUTPUT MOSAIC
	cv::imwrite("../mosaic", mosaic);
	cv::namedWindow("Mosaic Image");
	cv::imshow("Mosaic Image", mosaic);
	cv::waitKey();
*/
	return 0;
}