#pragma once
#include "ViewItemCar.h"
#include "ViewImage.h"
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <ctime>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <iostream>
#include <string>
#include "Model.h"
#include <opencv2/calib3d/calib3d.hpp>
using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;
using namespace autopilot;
class RobustMatcher {

private:

	// pointer to the feature point detector object
	cv::Ptr<cv::FeatureDetector> detector;
	// pointer to the feature descriptor extractor object
	cv::Ptr<cv::DescriptorExtractor> extractor;
	cv::Ptr<cv::DescriptorMatcher> matcher;
	float ratio; // max ratio between 1st and 2nd NN
	bool refineF; // if true will refine the F matrix
	double distance; // min distance to epipolar
	double confidence; // confidence level (probability)
	bool match_judge = false;
public:
	//distance，ratio越大精度越小,confidence相反
	RobustMatcher() : ratio(0.65f), refineF(true), confidence(0.99), distance(100.0) {

		int minHessian = 400;
		detector = SURF::create(minHessian);
		cv::Ptr<cv::DescriptorExtractor> extractor = SURF::create();
		//Ptr<xfeatures2d::SURF> detector = xfeatures2d::SURF::create();
		/*detector = new cv::Ptr<cv::FeatureDetector>();*/
	}

	// Set the feature detector
	void setFeatureDetector(cv::Ptr<cv::FeatureDetector>& detect) {

		detector = detect;
	}

	// Set descriptor extractor
	void setDescriptorExtractor(cv::Ptr<cv::DescriptorExtractor>& desc) {

		extractor = desc;
	}

	// Set the minimum distance to epipolar in RANSAC
	void setMinDistanceToEpipolar(double d) {

		distance = d;
	}

	// Set confidence level in RANSAC
	void setConfidenceLevel(double c) {

		confidence = c;
	}

	// Set the NN ratio
	void setRatio(float r) {

		ratio = r;
	}

	// if you want the F matrix to be recalculated
	void refineFundamental(bool flag) {

		refineF = flag;
	}

	// Clear matches for which NN ratio is > than threshold
	// return the number of removed points 
	// (corresponding entries being cleared, i.e. size will be 0)
	int ratioTest
	(std::vector<std::vector<cv::DMatch>>& matches) {

		int removed = 0;

		// for all matches
		for (std::vector<std::vector<cv::DMatch>>::iterator matchIterator = matches.begin();
			matchIterator != matches.end(); ++matchIterator) {

			// if 2 NN has been identified
			if (matchIterator->size() > 1) {

				// check distance ratio 跟最近点的距离要小于次近点的距离一定的比例才认为是匹配的
				if ((*matchIterator)[0].distance / (*matchIterator)[1].distance > ratio) {

					matchIterator->clear(); // remove match
					removed++;
				}

			}
			else { // does not have 2 neighbours

				matchIterator->clear(); // remove match
				removed++;
			}
		}

		return removed;
	}

	// Insert symmetrical matches in symMatches vector
	void symmetryTest(const std::vector<std::vector<cv::DMatch>>& matches1,
		const std::vector<std::vector<cv::DMatch>>& matches2,
		std::vector<cv::DMatch>& symMatches) {

		// for all matches image 1 -> image 2
		for (std::vector<std::vector<cv::DMatch>>::const_iterator matchIterator1 = matches1.begin();
			matchIterator1 != matches1.end(); ++matchIterator1) {

			if (matchIterator1->size() < 2) // ignore deleted matches 
				continue;

			// for all matches image 2 -> image 1
			for (std::vector<std::vector<cv::DMatch>>::const_iterator matchIterator2 = matches2.begin();
				matchIterator2 != matches2.end(); ++matchIterator2) {

				if (matchIterator2->size() < 2) // ignore deleted matches 
					continue;

				// Match symmetry test
				if ((*matchIterator1)[0].queryIdx == (*matchIterator2)[0].trainIdx &&
					(*matchIterator2)[0].queryIdx == (*matchIterator1)[0].trainIdx) {

					// add symmetrical match
					symMatches.push_back(cv::DMatch((*matchIterator1)[0].queryIdx,
						(*matchIterator1)[0].trainIdx,
						(*matchIterator1)[0].distance));
					break; // next match in image 1 -> image 2
				}
			}
		}
	}

	// Identify good matches using RANSAC
	// Return fundemental matrix
	cv::Mat ransacTest(const std::vector<cv::DMatch>& matches,
		const std::vector<cv::KeyPoint>& keypoints1,
		const std::vector<cv::KeyPoint>& keypoints2,
		std::vector<cv::DMatch>& outMatches) {

		// Convert keypoints into Point2f	
		std::vector<cv::Point2f> points1, points2;
		for (std::vector<cv::DMatch>::const_iterator it = matches.begin();
			it != matches.end(); ++it) {

			// Get the position of left keypoints
			float x = keypoints1[it->queryIdx].pt.x;
			float y = keypoints1[it->queryIdx].pt.y;
			points1.push_back(cv::Point2f(x, y));
			// Get the position of right keypoints
			x = keypoints2[it->trainIdx].pt.x;
			y = keypoints2[it->trainIdx].pt.y;
			points2.push_back(cv::Point2f(x, y));
		}

		// Compute F matrix using RANSAC
		std::vector<uchar> inliers(points1.size(), 0);
		cv::Mat fundemental = cv::findFundamentalMat(
			cv::Mat(points1), cv::Mat(points2), // matching points
			inliers,      // match status (inlier ou outlier)  
			CV_FM_RANSAC, // RANSAC method
			distance,     // distance to epipolar line
			confidence);  // confidence probability

		// extract the surviving (inliers) matches
		std::vector<uchar>::const_iterator itIn = inliers.begin();
		std::vector<cv::DMatch>::const_iterator itM = matches.begin();
		// for all matches
		for (; itIn != inliers.end(); ++itIn, ++itM) {

			if (*itIn) { // it is a valid match

				outMatches.push_back(*itM);
			}
		}
		if (outMatches.size() == 0)
		{
			return fundemental;
		}
		std::cout << "Number of matched points (after cleaning): " << outMatches.size() << std::endl;
		if (refineF) {
			// The F matrix will be recomputed with all accepted matches

				// Convert keypoints into Point2f for final F computation	
			points1.clear();
			points2.clear();

			for (std::vector<cv::DMatch>::const_iterator it = outMatches.begin();
				it != outMatches.end(); ++it) {

				// Get the position of left keypoints
				float x = keypoints1[it->queryIdx].pt.x;
				float y = keypoints1[it->queryIdx].pt.y;
				points1.push_back(cv::Point2f(x, y));
				// Get the position of right keypoints
				x = keypoints2[it->trainIdx].pt.x;
				y = keypoints2[it->trainIdx].pt.y;
				points2.push_back(cv::Point2f(x, y));
			}
			fundemental = cv::findFundamentalMat(
				cv::Mat(points1), cv::Mat(points2), // matching points
				CV_FM_8POINT); // 8-point method
			match_judge = true;
			// Compute 8-point F from all accepted matches
		}

		return fundemental;
	}

	viewVector match(std::string leftFilePath, std::string rightFilePath, // input images 
		std::vector<cv::DMatch>& matches, // output matches and keypoints
		std::vector<cv::KeyPoint>& keypoints1, std::vector<cv::KeyPoint>& keypoints2);
};
viewVector RobustMatcher::match(std::string leftFilePath, std::string rightFilePath, // input images 
	std::vector<cv::DMatch>& matches, // output matches and keypoints
	std::vector<cv::KeyPoint>& keypoints1, std::vector<cv::KeyPoint>& keypoints2) {
	cv::Mat image1, image2;
	cv::Mat descriptors1, descriptors2;
	// 1a. Detection of the SURF features
	clock_t start = clock();
	/*detector->detect(image1,keypoints1);
	detector->detect(image2,keypoints2);*/
	cv::Mat img1 = cv::imread(leftFilePath);
	cv::Mat img2 = cv::imread(rightFilePath);
	resize(img1, image1, Size(380, 507));
	resize(img2, image2, Size(380, 507));
	detector->detectAndCompute(image1, Mat(), keypoints1, descriptors1);
	detector->detectAndCompute(image2, Mat(), keypoints2, descriptors2);
	clock_t end = clock();
	// 2. Match the two image descriptors
	cv::BFMatcher matcher;

	std::vector<std::vector<cv::DMatch>> matches1;
	matcher.knnMatch(descriptors1, descriptors2,
		matches1, // vector of matches (up to 2 per entry) 
		2);		  // return 2 nearest neighbours

	std::vector<std::vector<cv::DMatch>> matches2;
	matcher.knnMatch(descriptors2, descriptors1,
		matches2, // vector of matches (up to 2 per entry) 
		2);		  // return 2 nearest neighbours


	// 3. Remove matches for which NN ratio is > than threshold

	// clean image 1 -> image 2 matches
	int removed = ratioTest(matches1);
	removed = ratioTest(matches2);
	std::vector<cv::DMatch> symMatches;
	symmetryTest(matches1, matches2, symMatches);
	cv::Mat fundemental = ransacTest(symMatches, keypoints1, keypoints2, matches);


	if (match_judge == true)
	{
		size_t matchesSize = matches.size(); viewVector vec{ 0,0,0 };
		for (auto i = matches.begin(); i != matches.end(); i++) {
			//归一化并转换到中心坐标系, x大于0代表R相对于L相机向右移动，同理y是向上移动
			//center为正代表R相对于L放大，反之是缩小
			//相机移动方向和物体移动方向相反
			cv::Point2f p1{ keypoints1[i->queryIdx].pt.x / image1.cols - 0.5f, keypoints1[i->queryIdx].pt.y / image1.rows - 0.5f };
			cv::Point2f p2{ keypoints2[i->trainIdx].pt.x / image2.cols - 0.5f, keypoints2[i->trainIdx].pt.y / image2.rows - 0.5f };
			vec.x += p1.x - p2.x;
			vec.y += p2.y - p1.y;
			vec.center += ((p2 - p1).dot(p1) / sqrtf(p1.x * p1.x + p1.y * p1.y));
		}
		vec.x /= matchesSize;
		vec.y /= matchesSize;
		vec.center /= matchesSize;
		cout << "normalized delta x=" << vec.x << endl;
		cout << "normalized delta y=" << vec.y << endl;
		cout << "normalized center=" << vec.center << endl;
		Mat img_maches;
		drawMatches(image1, keypoints1, image2, keypoints2, matches, img_maches);
		Mat dst; double scale = 0.5;
		resize(img_maches, dst, Size(img_maches.cols*scale, img_maches.rows*scale));
		imshow("匹配效果", dst);
		return vec;
	}
	else
	{
		Model a;
		a.SURF(0.12, rightFilePath, leftFilePath);
	}
	
}
//因为要用到viewVector vec{ 0,0,0 }和surf函数所以加了#include "Model.h"等
//经测试，没有缩放的两图center为+-0.02,缩放的图center在0.18以上