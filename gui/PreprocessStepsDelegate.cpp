#include "PreprocessStepsDelegate.h"
#include "PreprocessStepsListModel.h"

#include <QCheckBox>

PreprocessStepsDelegate::PreprocessStepsDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
}

PreprocessStepsDelegate::~PreprocessStepsDelegate()
{
}

QWidget *PreprocessStepsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QCheckBox *editor = new QCheckBox(parent);

	return editor;
}

void PreprocessStepsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	bool value = index.model()->data(index, Qt::EditRole).toBool();

	QCheckBox *spinBox = static_cast<QCheckBox *>(editor);
	spinBox->setChecked(value);
}

void PreprocessStepsDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
	QCheckBox *spinBox = static_cast<QCheckBox *>(editor);
	QString value = spinBox->isChecked() ? "Active" : "Inactive";

	model->setData(index, value, Qt::EditRole);
}

void PreprocessStepsDelegate::updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	editor->setGeometry(option.rect);
}
