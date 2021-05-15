#include "PreprocessStepsDelegate.h"
#include "PreprocessStepsListModel.h"
#include "PreprocessStepWidget.h"

#include <QPainter>

PreprocessStepsDelegate::PreprocessStepsDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
}

PreprocessStepsDelegate::~PreprocessStepsDelegate()
{
}

void PreprocessStepsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (!index.isValid())
		return;

	QStyleOptionViewItemV4 opt = option;
	QStyledItemDelegate::initStyleOption(&opt, index);

	if (option.state & QStyle::State_Selected)
	{
		painter->fillRect(option.rect, option.palette.highlight());
	}

	//This doesn't work :P
	if (option.state & QStyle::StateFlag::State_Editing)
	{
		return;
	}

	painter->save();
	PreprocessStepWidget *widget = new PreprocessStepWidget();
	painter->translate(option.rect.topLeft());
	widget->render(painter, QPoint(), QRegion(), QWidget::DrawChildren);
	painter->restore();
}

QWidget *PreprocessStepsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return new PreprocessStepWidget(parent);
}

QSize PreprocessStepsDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	return PreprocessStepWidget().sizeHint();
}

void PreprocessStepsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	static_cast<PreprocessStepWidget *>(editor)->setName(index.data().toString());
}

void PreprocessStepsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	model->setData(index, static_cast<PreprocessStepWidget *>(editor)->getName());
}

void PreprocessStepsDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	static_cast<PreprocessStepWidget *>(editor)->setGeometry(option.rect);
}
