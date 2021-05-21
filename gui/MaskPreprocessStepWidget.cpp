#include "MaskPreprocessStepWidget.h"
#include "MaskPreprocessStep.h"
#include "MaskPainterDialog.h"
#include "ImageUtility.h"
#include "asmOpenCV.h"

#include <QMessageBox>

#include <opencv2/imgproc.hpp>

MaskPreprocessStepWidget::MaskPreprocessStepWidget(QWidget *parent)
	: PreprocessStepWidget(parent)
{
	//Create step
	m_step = new MaskPreprocessStep();

	//Set name label
	m_nameLabel->setText(QString::fromStdString(m_step->getName()));

	//Create button
	m_button = new QPushButton("Edit Mask", this);
	m_layout->insertWidget(3, m_button);

	//When button clicked open mask painter dialog
	connect(m_button, &QPushButton::released,
		this, [=]() {
			//Mask creation requires an image
			if (m_inputImageCache.empty())
			{
				QMessageBox msgBox;
				msgBox.setText("Error: No image loaded");
				msgBox.exec();
				return;
			}

			//Combine cached input image with mask
			cv::Mat imageAndMask;
			ImageUtility::matAddAlpha(m_inputImageCache, static_cast<MaskPreprocessStep *>(m_step)->getMask(), imageAndMask);

			//Convert mat to QImage
			QImage image = ASM::cvMatToQImage(imageAndMask);

			//Open mask painter dialog
			MaskPainterDialog maskPainterDialog(image, this);
			if (maskPainterDialog.exec() == QDialog::Accepted)
			{
				imageAndMask = ASM::QImageToCvMat(maskPainterDialog.getImage());

				cv::Mat mask;
				cv::extractChannel(imageAndMask, mask, 3);
				static_cast<MaskPreprocessStep *>(m_step)->setMask(mask);

				emit stepChanged();
			}
		});
}

MaskPreprocessStepWidget::~MaskPreprocessStepWidget()
{}

//Applies the preprocess step to an image
void MaskPreprocessStepWidget::preprocess(const cv::Mat & t_in, cv::Mat & t_out)
{
	//Cache input image
	m_inputImageCache = t_in.clone();

	//Resize mask
	cv::Mat mask = static_cast<MaskPreprocessStep *>(m_step)->getMask();
	if (t_in.size() != mask.size())
	{
		cv::Mat newMask = cv::Mat(t_in.size(), CV_8UC1, cv::Scalar::all(255));
		if (!t_in.empty() && !mask.empty())
		{
			mask(cv::Range(0, std::min(mask.rows, newMask.rows)), cv::Range(0, std::min(mask.cols, newMask.cols))).copyTo(newMask);
		}
		static_cast<MaskPreprocessStep *>(m_step)->setMask(newMask);
	}

	m_step->preprocess(t_in, t_out);
}
