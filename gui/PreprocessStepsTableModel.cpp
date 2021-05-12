#include "PreprocessStepsTableModel.h"

PreprocessStepsTableModel::PreprocessStepsTableModel(QObject *parent)
	: QAbstractTableModel(parent)
{
	m_steps.append("Data 1");
	m_steps.append("Data 2");
	m_steps.append("Data 3");
	m_steps.append("Data 4");
	m_steps.append("Data 5");
}

PreprocessStepsTableModel::~PreprocessStepsTableModel()
{
}

Qt::ItemFlags PreprocessStepsTableModel::flags(const QModelIndex &index) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren | Qt::ItemIsEditable;
}

QVariant PreprocessStepsTableModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= m_steps.size() || index.row() < 0)
		return QVariant();

	if (role == Qt::DisplayRole)
	{
		switch (index.column())
		{
		case 0:
			return "0";
		case 1:
			return m_steps[index.row()];
		case 2:
			return "2";
		default:
			break;
		}
	}

	return QVariant();
}

QVariant PreprocessStepsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	if (orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case 0:
			return tr("First");
		case 1:
			return tr("Data");
		case 2:
			return tr("Last");
		default:
			break;
		}
	}
	return QVariant();
}

int PreprocessStepsTableModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return m_steps.size();
}

int PreprocessStepsTableModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;

	return 3;
}

bool PreprocessStepsTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (index.isValid() && role == Qt::EditRole)
	{
		const int row = index.row();
		auto step = m_steps.value(row);

		switch (index.column())
		{
		case 1:
			step = value.toString();
			break;
		default:
			return false;
		}
		m_steps.replace(row, step);
		emit dataChanged(index, index, { Qt::DisplayRole, Qt::EditRole });

		return true;
	}

	return false;
}
