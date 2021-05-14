#include "PreprocessStepsDelegate.h"
#include "PreprocessStepsListModel.h"
#include "PreprocessStepWidget.h"

PreprocessStepsDelegate::PreprocessStepsDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
}

PreprocessStepsDelegate::~PreprocessStepsDelegate()
{
}

QWidget *PreprocessStepsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	PreprocessStepWidget *editor = new PreprocessStepWidget(parent);

	return editor;
}

void PreprocessStepsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QString value = index.model()->data(index, Qt::EditRole).toString();

	PreprocessStepWidget *widget = static_cast<PreprocessStepWidget *>(editor);
	widget->setName(value);
}

void PreprocessStepsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	PreprocessStepWidget *widget = static_cast<PreprocessStepWidget *>(editor);

	model->setData(index, widget->getName(), Qt::EditRole);
}

void PreprocessStepsDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	editor->setGeometry(option.rect);
}
