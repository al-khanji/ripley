#ifndef COMPOSITORMODEL_H
#define COMPOSITORMODEL_H

#include <QAbstractListModel>

class QWaylandSurface;

struct CompositorItem
{
    CompositorItem(QWaylandSurface *surface) : surface(surface) { }
    QWaylandSurface *surface;
    bool operator ==(const CompositorItem &other) const
    {
        return surface == other.surface;
    }
};

class CompositorModelPrivate;

class CompositorModel : public QAbstractListModel
{
    Q_OBJECT
    enum Role { Surface = Qt::UserRole + 1 };
public:
    CompositorModel();

    bool append(const CompositorItem &item);
    bool remove(const CompositorItem &item);

    // QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);

private:
    CompositorModelPrivate *d;
};


#endif // COMPOSITORMODEL_H
