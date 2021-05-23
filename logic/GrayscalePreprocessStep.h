#pragma once
#include "PreprocessStep.h"

//Assumes image is either BGRA, BGR, or GRAY
class GrayscalePreprocessStep : public PreprocessStep
{
public:
	GrayscalePreprocessStep();

	//Returns name of preprocess step
	std::string getName() override;

	//Applies the preprocess step to an image
	void preprocess(const cv::Mat &t_in, cv::Mat &t_out) override;
};

