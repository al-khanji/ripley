#ifndef DESKTOPMODEL_H
#define DESKTOPMODEL_H

#include <QAbstractListModel>
#include <QProcess>

struct DesktopEntry;
class DesktopProcessPrivate;
class DesktopProcess : public QProcess
{
    Q_OBJECT
    Q_PROPERTY(int state READ state NOTIFY stateChanged)
public:
    DesktopProcess(const DesktopEntry &entry);
    ~DesktopProcess();

    Q_INVOKABLE void start();

signals:
    void stateChanged();

private:
    QScopedPointer<DesktopProcessPrivate> d;
};

class DesktopModelPrivate;
class DesktopModel : public QAbstractListModel
{
    Q_OBJECT
    enum Role {
        Type = Qt::UserRole + 1,
        Version,
        Name,
        GenericName,
        NoDisplay,
        Comment,
        Icon,
        Hidden,
        OnlyShowIn,
        NotShownIn,
        DBusActivatable,
        TryExec,
        Exec,
        Path,
        Terminal,
        Actions,
        MimeType,
        Categories,
        Keywords,
        StartupNotify,
        StartupWMClass,
        URL,
        Extensions,
        Process
    };

public:
    DesktopModel();
    ~DesktopModel();

    // QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
    bool insertRows(int row, int count, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);

private:
    void parseEntries();

    friend class DesktopModelPrivate;
    QScopedPointer<DesktopModelPrivate> d;
};


#endif // DESKTOPMODEL_H
