#include <iostream>
#include <exception>
#include "opencv2/opencv.hpp"
#include "sample_config.hpp"

int lineThreshold = 0;
std::vector<float> baseline;

void drawDetectLines(cv::Mat& image,const std::vector<cv::Vec4i>& lines,cv::Scalar color)
{
	baseline.assign(CAM_FRAME_WIDTH, 0);
    // 将检测到的直线在图上画出来
    std::vector<cv::Vec4i>::const_iterator it=lines.begin();
    float minValue, maxValue;
    while(it!=lines.end())
    {
        cv::Point pt1((*it)[0],(*it)[1]);
        cv::Point pt2((*it)[2],(*it)[3]);
        if(abs(pt1.y - lineThreshold) < 5 && abs(pt2.y - lineThreshold) < 5)
        {
        	minValue = std::min(pt1.x, pt2.x);
        	maxValue = std::max(pt1.x, pt2.x);
        	for(int i = minValue; i<=maxValue; i++)
        	{
        		baseline[i] += 50;
        	}
        }
        cv::line(image,pt1,pt2,cv::Scalar(rand()&255, rand()&255, rand()&255) ,2); //  线条宽度设置为2
        ++it;
    }
    for(int i = 0; i<CAM_FRAME_WIDTH; i++)
    {
		std::cout << baseline[i] << std::endl;
    	image.at<cv::Vec3b>(100, i)[1] = std::min(255, (int)baseline[i]);
    	image.at<cv::Vec3b>(101, i)[1] = std::min(255, (int)baseline[i]);
    	image.at<cv::Vec3b>(102, i)[1] = std::min(255, (int)baseline[i]);
    }
}

int main(void)
{
	std::cout << "Initialize....." << std::endl;
	std::cout << "Clean the area between the robot and laser line, then press enter.";
	std::getchar();

	cv::VideoCapture cam(CAMERA_NUM);
	cam.set(cv::CAP_PROP_FRAME_WIDTH, CAM_FRAME_WIDTH);
	cam.set(cv::CAP_PROP_FRAME_HEIGHT, CAM_FRAME_HEIGHT);
	baseline.resize(CAM_FRAME_WIDTH);
	baseline.clear();

	cv::Mat sceneImage, grayImage, binImage, lineImage, outputImage;
	cv::Mat channel[3];
	std::vector<cv::Vec4i> lines;

	cam >> sceneImage;
	binImage = cv::Mat(CAM_FRAME_HEIGHT, CAM_FRAME_WIDTH, CV_8UC1);
	lineImage = cv::Mat(CAM_FRAME_HEIGHT, CAM_FRAME_WIDTH, CV_8UC3);
	outputImage = cv::Mat(CAM_FRAME_HEIGHT, CAM_FRAME_WIDTH, CV_8UC3);

	cam >> sceneImage;
	sceneImage.copyTo(outputImage);
	cv::imshow("Output", outputImage);
	cv::waitKey(0);
	cv::cvtColor(sceneImage, grayImage, cv::COLOR_BGR2HSV);
	cv::split(grayImage, channel);
	//channel[0]=cv::Mat::zeros(grayImage.rows, sceneImage.cols, CV_8UC1);
	//channel[1]=cv::Mat::zeros(grayImage.rows, sceneImage.cols, CV_8UC1);
	//cv::merge(channel, 3, grayImage);

	//cv::cvtColor(channel[2], grayImage, cv::COLOR_BGR2GRAY);
	grayImage = channel[2];
	//cv::equalizeHist(grayImage, grayImage);

	uchar as;
	cv::Point2d aPose(0,0);
	uint16_t i,j;

	binImage.setTo(0);
	lineImage.setTo(0);
	for(i = 0; i < grayImage.cols; i++)
	{
		as = 0;
		for(j = 0; j < grayImage.rows; j++)
		{
			//if(sceneImage.at<cv::Vec2b>(j, i)[2] > a)
			if(grayImage.at<uchar>(j, i) > as)
			{
				as = grayImage.at<uchar>(j, i);
				aPose.x = i;
				aPose.y = j;
			}
		}
		binImage.at<uchar>(aPose.y, aPose.x) = 255;
	}

    std::vector<cv::Vec2f> linesa;
    cv::HoughLines(binImage, linesa, 1, CV_PI/30, 100, 0, 0 );
    float rho = linesa[0][0], theta = linesa[0][1];
	cv::Point pt1, pt2;
	double a = cos(theta), b = sin(theta);
	double x0 = a*rho, y0 = b*rho;
	pt1.x = cvRound(x0 + 1000*(-b));
	pt1.y = cvRound(y0 + 1000*(a));
	pt2.x = cvRound(x0 - 1000*(-b));
	pt2.y = cvRound(y0 - 1000*(a));
	cv::line( lineImage, pt1, pt2, cv::Scalar(0,0,255), 1, CV_AA);
	cv::line( outputImage, pt1, pt2, cv::Scalar(0,0,255), 1, CV_AA);
	lineThreshold = cvRound((pt1.y + pt2.y)/2);
	std::cout << "Line Threshold:" << lineThreshold << std::endl;
	cv::imshow("Scene", sceneImage);
	cv::imshow("Gray", grayImage);
	cv::imshow("Bin", binImage);
	cv::imshow("Line", lineImage);
	cv::imshow("Output", outputImage);
	cv::waitKey(0);

	while(1)
	{
		cam >> sceneImage;
		sceneImage.copyTo(outputImage);
		cv::cvtColor(sceneImage, grayImage, cv::COLOR_BGR2HSV);
		cv::split(grayImage, channel);
		//channel[0]=cv::Mat::zeros(grayImage.rows, sceneImage.cols, CV_8UC1);
		//channel[1]=cv::Mat::zeros(grayImage.rows, sceneImage.cols, CV_8UC1);
		//cv::merge(channel, 3, grayImage);

		//cv::cvtColor(channel[2], grayImage, cv::COLOR_BGR2GRAY);
		grayImage = channel[2];
		//cv::equalizeHist(grayImage, grayImage);

		uchar a;
		cv::Point2d aPose(0,0);
		uint16_t i,j;

		binImage.setTo(0);
		lineImage.setTo(0);
		for(i = 0; i < grayImage.cols; i++)
		{
			a = 0;
			for(j = 0; j < grayImage.rows; j++)
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
#if 1
		lines.clear();
		cv::HoughLinesP(binImage, lines, 1, CV_PI/180, 10, 3, 10);
		drawDetectLines(lineImage, lines, cv::Scalar(0));
		drawDetectLines(outputImage, lines, cv::Scalar(0));
#else
	    std::vector<cv::Vec2f> linesa;
	    cv::HoughLines(binImage, linesa, 1, CV_PI/30, 100, 0, 0 );
	    for( size_t i = 0; i < linesa.size(); i++ )
	    {
	        float rho = linesa[i][0], theta = linesa[i][1];
	        cv::Point pt1, pt2;
	        double a = cos(theta), b = sin(theta);
	        double x0 = a*rho, y0 = b*rho;
	        pt1.x = cvRound(x0 + 1000*(-b));
	        pt1.y = cvRound(y0 + 1000*(a));
	        pt2.x = cvRound(x0 - 1000*(-b));
	        pt2.y = cvRound(y0 - 1000*(a));
	        cv::line( lineImage, pt1, pt2, cv::Scalar(0,0,255), 1, CV_AA);
	        cv::line( outputImage, pt1, pt2, cv::Scalar(0,0,255), 1, CV_AA);
	    }
#endif
		cv::imshow("Scene", sceneImage);
		cv::imshow("Gray", grayImage);
		cv::imshow("Bin", binImage);
		cv::imshow("Line", lineImage);
		cv::imshow("Output", outputImage);
		cv::waitKey(1);
	}
	return 0;
}
