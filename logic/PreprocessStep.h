#pragma once
#include <string>

#include <opencv2/core.hpp>

class PreprocessStep
{
public:
	PreprocessStep();

	//Returns name of preprocess step
	virtual std::string getName();

	//Applies the preprocess step to an image
	virtual void preprocess(const cv::Mat &t_in, cv::Mat &t_out);

private:
};

