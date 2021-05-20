#include "MaskPreprocessStep.h"

MaskPreprocessStep::MaskPreprocessStep()
	: PreprocessStep{}
{

}

//Returns name of preprocess step
std::string MaskPreprocessStep::getName()
{
	return "Apply Mask";
}

//Applies the preprocess step to an image
void MaskPreprocessStep::preprocess(const cv::Mat &t_in, cv::Mat &t_out)
{
	if (m_mask.empty())
	{
		t_out = t_in.clone();
	}
	else
	{
		t_out = cv::Mat::zeros(t_in.size(), t_in.type());
		cv::copyTo(t_in, t_out, m_mask);
	}
}

cv::Mat MaskPreprocessStep::getMask()
{
	return m_mask;
}

void MaskPreprocessStep::setMask(const cv::Mat &t_mask)
{
	m_mask = t_mask;
}
