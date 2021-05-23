#pragma once
#include "PreprocessStepWidget.h"

#include <QWidget>
#include <QVBoxLayout>

#include <opencv2/core.hpp>

class PreprocessStepListWidget : public QWidget
{
	Q_OBJECT

public:
	enum class Direction { Up = -1, Down = 1 };

	PreprocessStepListWidget(QWidget *parent);
	~PreprocessStepListWidget();

public slots:
	//Adds a preprocessing step to the list
	void addStep(PreprocessStepWidget *step);
	//Moves step in direction
	void moveStep(PreprocessStepWidget *step, const Direction dir);
	//Deletes step
	void deleteStep(PreprocessStepWidget *step);

	//Sets the image to apply the preprocessing steps to
	void setImage(const cv::Mat &t_in);
	//Performs all preprocess steps on current image
	void preprocess();
	//Returns cached image result
	cv::Mat getResult();

signals:
	//Emits preprocessed image whenever it changes
	void imageUpdated(const cv::Mat &t_out);

private:
	QVBoxLayout *m_layout;
	QList<PreprocessStepWidget *> m_steps;

	cv::Mat m_image;
	cv::Mat m_cachedImageResult;
};