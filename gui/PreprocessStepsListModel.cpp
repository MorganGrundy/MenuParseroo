#include "PreprocessStepsListModel.h"

PreprocessStepsListModel::PreprocessStepsListModel(QObject *parent)
	: QAbstractListModel(parent)
{
}

PreprocessStepsListModel::~PreprocessStepsListModel()
{
}

Qt::ItemFlags PreprocessStepsListModel::flags(const QModelIndex &index) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren | Qt::ItemIsEditable;
}

QVariant PreprocessStepsListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= m_steps.size() || index.row() < 0)
		return QVariant();

	if (role == Qt::DisplayRole)
	{
		return m_steps[index.row()];
	}

	return QVariant();
}

QVariant PreprocessStepsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
	{
		return tr("Step");
	}
	return QVariant();
}

int PreprocessStepsListModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return m_steps.size();
}

int PreprocessStepsListModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return 3;
}

bool PreprocessStepsListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (index.isValid() && role == Qt::EditRole)
	{
		const int row = index.row();
		auto step = m_steps.value(row);

		step = value.toString();

		m_steps.replace(row, step);
		emit dataChanged(index, index, { Qt::DisplayRole, Qt::EditRole });

		return true;
	}

	return false;
}

bool PreprocessStepsListModel::insertStep(int row, const QString &step)
{
	beginInsertRows(QModelIndex(), row, row);

	m_steps.insert(row, step);

	endInsertRows();
	return true;
}

bool PreprocessStepsListModel::removeStep(int row)
{
	beginRemoveRows(QModelIndex(), row, row);

	m_steps.removeAt(row);

	endRemoveRows();
	return true;
}
