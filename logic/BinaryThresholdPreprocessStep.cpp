#include "BinaryThresholdPreprocessStep.h"

#include <opencv2/imgproc.hpp>

BinaryThresholdPreprocessStep::BinaryThresholdPreprocessStep()
	: PreprocessStep{}, m_threshold{ 127 }
{

}

//Returns name of preprocess step
std::string BinaryThresholdPreprocessStep::getName()
{
	return "Binary Threshold";
}

//Applies the preprocess step to an image
void BinaryThresholdPreprocessStep::preprocess(const cv::Mat &t_in, cv::Mat &t_out)
{
	cv::threshold(t_in, t_out, m_threshold, 255, cv::THRESH_BINARY);
}

int BinaryThresholdPreprocessStep::getThreshold()
{
	return m_threshold;
}

void BinaryThresholdPreprocessStep::setThreshold(const int t_threshold)
{
	m_threshold = t_threshold;
}
