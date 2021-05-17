#pragma once
#include <string>

class PreprocessStep
{
public:
	PreprocessStep();

	//Returns name of preprocess step
	std::string getName();

private:
	const std::string name;
};

