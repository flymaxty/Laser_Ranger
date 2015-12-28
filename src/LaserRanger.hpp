
#pragma once

#define LASER_RANGER_DEBUG

#include <iosteam>
#include "opencv2/opencv.hpp"

class LaserRanger
{
public:
	uint16_t m_baseLine;
	uint16_t m_frameHeight, m_frameWidth;
private:

public:
	LaserRanger(uint16_t in_frameHeight, uint16_t in_frameWidth);
	~LaserRanger();

	getBaseLine(cv::Mat in_image);
}