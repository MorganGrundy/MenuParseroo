#pragma once
#include "PreprocessStep.h"

class BinaryThresholdPreprocessStep : public PreprocessStep
{
public:
	BinaryThresholdPreprocessStep();

	//Returns name of preprocess step
	std::string getName() override;

	//Applies the preprocess step to an image
	void preprocess(const cv::Mat &t_in, cv::Mat &t_out) override;

	int getThreshold();
	void setThreshold(const int t_threshold);

private:
	int m_threshold;
};

