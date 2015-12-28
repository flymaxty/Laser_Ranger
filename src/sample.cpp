#include <iostream>
#include "opencv2/opencv.hpp"
#include "sample_config.hpp"

void drawDetectLines(cv::Mat& image,const std::vector<cv::Vec4i>& lines,cv::Scalar color)
{
    // 将检测到的直线在图上画出来
    std::vector<cv::Vec4i>::const_iterator it=lines.begin();
    while(it!=lines.end())
    {
        cv::Point pt1((*it)[0],(*it)[1]);
        cv::Point pt2((*it)[2],(*it)[3]);
        cv::line(image,pt1,pt2,cv::Scalar(rand()&255, rand()&255, rand()&255) ,2); //  线条宽度设置为2
        ++it;
    }
}

int main(void)
{
	cv::Mat sceneImage = cv::imread(SCENE_IMG);
	cv::resize(sceneImage, sceneImage, cv::Size(320,240));
	cv::Mat grayImage, binImage, outputImage;

	cv::Mat channel[3];

	cv::split(sceneImage, channel);
	channel[0]=cv::Mat::zeros(sceneImage.rows, sceneImage.cols, CV_8UC1);
	channel[1]=cv::Mat::zeros(sceneImage.rows, sceneImage.cols, CV_8UC1);
	cv::merge(channel, 3, sceneImage);

	cv::cvtColor(sceneImage, grayImage, cv::COLOR_RGB2GRAY);
	/*double cannyMaxThreshold = cv::threshold(grayImage, adaptImage, 0, 255,
									cv::THRESH_BINARY | cv::THRESH_OTSU);
	double cannyMinThreshold = 0.1*cannyMaxThreshold;

	cv::Canny(grayImage, cannyImage, cannyMinThreshold, cannyMaxThreshold);*/

	uchar a;
	cv::Point2d aPose;
	binImage = cv::Mat(grayImage.size(), CV_8UC1);
	outputImage = cv::Mat(grayImage.size(), CV_8UC3);
	//outputImage.setTo(0);

	std::cout << grayImage.cols << std::endl;
	std::cout << grayImage.rows << std::endl;
	for(uint16_t i = 0; i < grayImage.cols; i++)
	{
		a = 0;
	    for(uint16_t j = 0; j < grayImage.rows; j++)
	    {
	    	//if(sceneImage.at<cv::Vec2b>(j, i)[2] > a)
	    	if(grayImage.at<uchar>(j, i) > a)
	    	{
	    		a = grayImage.at<uchar>(j, i);
	    		aPose.x = i;
	    		aPose.y = j;
	    	}
	    }
	    binImage.at<uchar>(aPose.y, aPose.x) = 255;
	}

	std::vector<cv::Vec4i> lines;
	cv::HoughLinesP(binImage, lines, 1, CV_PI/180, 8, 5, 3);
	drawDetectLines(sceneImage, lines, cv::Scalar(0));
	drawDetectLines(sceneImage, lines, cv::Scalar(0));

	cv::imshow("Scene", sceneImage);
	cv::imshow("Bin", binImage);
	cv::imshow("Gray", grayImage);
	cv::imshow("Output", outputImage);
	cv::waitKey(0);
	return 0;
}
