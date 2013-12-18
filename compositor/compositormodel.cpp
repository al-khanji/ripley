#include "compositormodel.h"

#include <QWaylandSurface>

class CompositorModelPrivate
{
friend class CompositorModel;
    QHash<int, QByteArray> roleNames;
    int rowCount;
    QList<CompositorItem> items;
};

CompositorModel::CompositorModel()
    : d(new CompositorModelPrivate)
{
    d->rowCount = 0;
    d->roleNames.insert(Surface, "surface");
}

bool CompositorModel::append(const CompositorItem &item)
{
    d->items.append(item);
    return insertRow(d->rowCount, QModelIndex());
}

bool CompositorModel::remove(const CompositorItem &item)
{
    const int row = d->items.indexOf(item);
    if (row >= 0) {
        d->items.removeAt(row);
        return removeRow(row);
    }
    return false;
}

int CompositorModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return d->rowCount;
}

QHash<int, QByteArray> CompositorModel::roleNames() const
{
    return d->roleNames;
}

QVariant CompositorModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (static_cast<Role>(role)) {
    case Surface:
        return QVariant::fromValue(d->items.at(index.row()).surface);
    default:
        break;
    }
    return QVariant();
}

bool CompositorModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (row < 0 || row > d->rowCount || count < 1)
        return false;

    beginInsertRows(QModelIndex(), row, row + count - 1);
    d->rowCount = d->items.size();
    endInsertRows();
    return true;
}

bool CompositorModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (row < 0 || row >= d->rowCount || count < 1)
        return false;

    const int endRow = qMin(d->rowCount - 1, row + count - 1);
    beginRemoveRows(QModelIndex(), row, endRow);
    d->rowCount = d->items.size();
    endRemoveRows();
    return true;
}
