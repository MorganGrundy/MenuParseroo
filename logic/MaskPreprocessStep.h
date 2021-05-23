#pragma once
#include "PreprocessStep.h"
class MaskPreprocessStep : public PreprocessStep
{
public:
	MaskPreprocessStep();

	//Returns name of preprocess step
	std::string getName() override;

	//Applies the preprocess step to an image
	void preprocess(const cv::Mat &t_in, cv::Mat &t_out) override;

	cv::Mat getMask();
	void setMask(const cv::Mat &t_mask);

private:
	cv::Mat m_mask;
};

