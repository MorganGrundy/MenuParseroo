#pragma once

#include <QStyledItemDelegate>

class PreprocessStepsDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	PreprocessStepsDelegate(QObject *parent = nullptr);
	~PreprocessStepsDelegate();

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const override;

	void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const override;

	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
		const QModelIndex &index) const override;
};
