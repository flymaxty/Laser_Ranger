#include "LaserRanger.hpp"

LaserRanger::LaserRanger(uint16_t in_frameHeight, uint16_t in_frameWidth)
{
	m_frameHeight = in_frameHeight;
	m_frameWidth = in_frameWidth;

	m_binImage = cv::Mat(m_frameHeight, m_frameWidth, CV_8UC1);
	m_lineImage = cv::Mat(m_frameHeight, m_frameWidth, CV_8UC3);
	m_outputImage = cv::Mat(m_frameHeight, m_frameWidth, CV_8UC3);
}

LaserRanger::~LaserRanger()
{

}

LaserRanger::getBaseLine(cv::Mat in_image)
{
	cv::Mat hsvImage, valueImage, binImage, lineImage, outputImage;
	cv::Mat channel[3];

	binImage = cv::Mat(m_frameHeight, m_frameWidth, CV_8UC1);
	lineImage = cv::Mat(m_frameHeight, m_frameWidth, CV_8UC3);

	in_image.copyTo(outputImage);
	cv::cvtColor(in_image, hsvImage, cv::COLOR_BGR2HSV);
	cv::split(hsvImage, channel);
	channel[2].copyTo(valueImage);

	uint8_t maxValue;
	cv::Point2d aPose(0,0);

	for(i = 0; i < valueImage.cols; i++)
	{
		maxValue = 0;
		for(j = 0; j < valueImage.rows; j++)
		{
			if(valueImage.at<uchar>(j, i) > maxValue)
			{
				maxValue = valueImage.at<uchar>(j, i);
				aPose.x = i;
				aPose.y = j;
			}
		}
		binImage.at<uchar>(aPose.y, aPose.x) = 255;
	}

    std::vector<cv::Vec2f> lines;
    cv::HoughLines(binImage, lines, 1, CV_PI/30, 100, 0, 0 );
    if(!lines.empty())
    {
		float rho = lines[0][0], theta = lines[0][1];
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		
		cv::Point pt1, pt2;
		pt1.x = cvRound(x0 + 1000*(-b));
		pt1.y = cvRound(y0 + 1000*(a));
		pt2.x = cvRound(x0 - 1000*(-b));
		pt2.y = cvRound(y0 - 1000*(a));
		cv::line(lineImage, pt1, pt2, cv::Scalar(0,0,255), 1, cv::LINE_AA);
		cv::line(outputImage, pt1, pt2, cv::Scalar(0,0,255), 1, cv::LINE_AA);
		m_baseLine = cvRound((pt1.y + pt2.y)/2);
		std::cout << "m_baseLine: " << m_baseLine << endl;

		#ifdef LASER_RANGER_DEBUG
		cv::imshow("Laser_Ranger in_image", in_image);
		cv::imshow("Laser_Ranger valueImage", valueImage);
		cv::imshow("Laser_Ranger binImage", binImage);
		cv::imshow("Laser_Ranger lineImage", lineImage);
		cv::imshow("Laser_Ranger outputImage", outputImage);
		cv::waitKet(0);
		#endif

		return true;
	}
	else
		return false;
}

LaserRanger::getObstacle
