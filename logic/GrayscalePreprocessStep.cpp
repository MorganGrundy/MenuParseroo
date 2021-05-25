#include "GrayscalePreprocessStep.h"

#include <opencv2/imgproc.hpp>

GrayscalePreprocessStep::GrayscalePreprocessStep()
	: PreprocessStep{}
{

}

//Returns name of preprocess step
std::string GrayscalePreprocessStep::getName()
{
	return "Convert to Grayscale";
}

//Applies the preprocess step to an image
void GrayscalePreprocessStep::preprocess(const cv::Mat &t_in, cv::Mat &t_out)
{
	switch (t_in.channels())
	{
	case 1: t_out = t_in.clone(); break;
	case 3: cv::cvtColor(t_in, t_out, cv::COLOR_BGR2GRAY); break;
	case 4: cv::cvtColor(t_in, t_out, cv::COLOR_BGRA2GRAY); break;
	default: throw std::invalid_argument("Unsupported number of channels");
	}
}
