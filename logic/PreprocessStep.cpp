#include "PreprocessStep.h"

PreprocessStep::PreprocessStep()
{}

std::string PreprocessStep::getName()
{
    return name;
}

//Applies the preprocess step to an image
void PreprocessStep::preprocess(const cv::Mat &t_in, cv::Mat &t_out)
{
    t_out = t_in.clone();
}
