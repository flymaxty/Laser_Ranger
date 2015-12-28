#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"
#include "sample_config.hpp"

int main(void)
{
	cv::VideoCapture cam(CAMERA_NUM);
	cam.set(cv::CAP_PROP_FRAME_WIDTH, CAM_FRAME_WIDTH);
	cam.set(cv::CAP_PROP_FRAME_HEIGHT, CAM_FRAME_HEIGHT);

	cv::Mat image, grayImage ,equalImage;
	cv::namedWindow("Image");
	double a;
	while(1)
	{
		//a = cam.get();
		std::cout << a << std::endl;
		cam >> image;
		cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
		cv::equalizeHist(grayImage, equalImage);
		cv::imshow("Image", image);
		cv::imshow("Gray Image", grayImage);
		cv::imshow("Equal Image", equalImage);

		if (cv::waitKey(1) != -1)
		{
			std::string imageName;
			std::cout << "Image Name: ";
			std::cin >> imageName;
			imageName ="image/" + imageName;
			std::cout << imageName << std::endl;
			cv::imwrite(imageName, image);
			break;
		}
	}
	return 0;
}
