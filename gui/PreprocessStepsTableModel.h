#pragma once

#include <QAbstractTableModel>

class PreprocessStepsTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	PreprocessStepsTableModel(QObject *parent = nullptr);
	~PreprocessStepsTableModel();

	Qt::ItemFlags flags(const QModelIndex &index) const override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	bool insertStep(int row, const QString &step);
	bool removeStep(int row);

private:
	QList<QString /*PreprocessStep*/> m_steps;
};
