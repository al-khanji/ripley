#include "desktopmodel.h"

#include <QDir>
#include <QFile>
#include <QStringList>
#include <QUrl>
#include <QSharedPointer>

struct DesktopEntry
{
    enum Type { Unknown = 0, Application = 1, Link = 2, Directory = 3 };
    Type type;
    QString version;
    QString name;
    QString genericName;
    bool noDisplay;
    QString comment;
    QUrl icon;
    bool hidden;
    QStringList onlyShowIn;
    QStringList notShowIn;
    bool dbusActivatable;
    QString tryExec;
    QString exec;
    QString path;
    bool terminal;
    QVariantMap actions;
    QStringList mimeType;
    QStringList categories;
    QStringList keywords;
    bool startupNotify;
    QString startupWMClass;
    QUrl url;
    QVariantMap extensions;
    mutable QSharedPointer<DesktopProcess> process;

    static const QHash<QString, Type> &types()
    {
        static QHash<QString, Type> types;
        if (types.isEmpty()) {
            types.insert("Application", Application);
            types.insert("Link", Link);
            types.insert("Directory", Directory);
        }
        return types;
    }
};

class DesktopProcessPrivate
{
    friend class DesktopProcess;
    DesktopProcessPrivate(const DesktopEntry &entry) : entry(entry) { }
    const DesktopEntry entry;
};

DesktopProcess::DesktopProcess(const DesktopEntry &entry)
    : d(new DesktopProcessPrivate(entry))
{
    connect(this, &QProcess::stateChanged, this, &DesktopProcess::stateChanged);
}

DesktopProcess::~DesktopProcess()
{
}

void DesktopProcess::start()
{
    QProcess::start(d->entry.exec);
}

class DesktopModelPrivate
{
    friend class DesktopModel;
    QList<DesktopEntry> entries;
    QList<DesktopProcess *> processes;
    int rowCount;
    QHash<int, QByteArray> roleNames;
    QHash<QString, DesktopModel::Role> roles;
};

DesktopModel::DesktopModel()
    : d(new DesktopModelPrivate)
{
    d->rowCount = 0;
    d->roleNames.insert(Version, "version");
    d->roles.insert("Version", Version);
    d->roleNames.insert(Name, "name");
    d->roles.insert("Name", Name);
    d->roleNames.insert(GenericName, "genericName");
    d->roles.insert("GenericName", GenericName);
    d->roleNames.insert(NoDisplay, "noDisplay");
    d->roles.insert("NoDisplay", NoDisplay);
    d->roleNames.insert(Comment, "comment");
    d->roles.insert("Comment", Comment);
    d->roleNames.insert(Icon, "icon");
    d->roles.insert("Icon", Icon);
    d->roleNames.insert(Hidden, "hidden");
    d->roles.insert("Hidden", Hidden);
    d->roleNames.insert(OnlyShowIn, "onlyShowIn");
    d->roles.insert("OnlyShowIn", OnlyShowIn);
    d->roleNames.insert(DBusActivatable, "dbusActivatable");
    d->roles.insert("DBusActivatable", DBusActivatable);
    d->roleNames.insert(TryExec, "tryExec");
    d->roles.insert("TryExec", TryExec);
    d->roleNames.insert(Exec, "exec");
    d->roles.insert("Exec", Exec);
    d->roleNames.insert(Path, "path");
    d->roles.insert("Path", Path);
    d->roleNames.insert(Terminal, "terminal");
    d->roles.insert("Terminal", Terminal);
    d->roleNames.insert(MimeType, "mimeType");
    d->roles.insert("mimeType", MimeType);
    d->roleNames.insert(Categories, "categories");
    d->roles.insert("Categories", Categories);
    d->roleNames.insert(Keywords, "keywords");
    d->roles.insert("Keywords", Keywords);
    d->roleNames.insert(StartupNotify, "startupNotify");
    d->roles.insert("StartupNotify", StartupNotify);
    d->roleNames.insert(StartupWMClass, "startupWMClass");
    d->roles.insert("StartupWMClass", StartupWMClass);
    d->roleNames.insert(URL, "url");
    d->roles.insert("URL", URL);
    d->roleNames.insert(Extensions, "extensions");
    d->roleNames.insert(Process, "process");

    parseEntries();
}

DesktopModel::~DesktopModel()
{
}

int DesktopModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return d->rowCount;
}

QHash<int, QByteArray> DesktopModel::roleNames() const
{
    return d->roleNames;
}

QVariant DesktopModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const DesktopEntry &entry = d->entries[index.row()];
    switch (static_cast<Role>(role)) {
    case Type:
        return entry.type;
    case Version:
        return entry.version;
    case Name:
        return entry.name;
    case GenericName:
        return entry.genericName;
    case NoDisplay:
        return entry.noDisplay;
    case Comment:
        return entry.comment;
    case Icon:
        return entry.icon;
    case Hidden:
        return entry.hidden;
    case OnlyShowIn:
        return entry.onlyShowIn;
    case NotShownIn:
        return entry.notShowIn;
    case DBusActivatable:
        return entry.dbusActivatable;
    case TryExec:
        return entry.tryExec;
    case Exec:
        return entry.exec;
    case Path:
        return entry.path;
    case Terminal:
        return entry.terminal;
    case Actions:
        return entry.actions;
    case MimeType:
        return entry.mimeType;
    case Categories:
        return entry.categories;
    case Keywords:
        return entry.keywords;
    case StartupNotify:
        return entry.startupNotify;
    case StartupWMClass:
        return entry.startupWMClass;
    case URL:
        return entry.url;
    case Extensions:
        return entry.extensions;
    case Process:
        if (!entry.process) {
            entry.process.reset(new DesktopProcess(entry));
            d->processes.append(entry.process.data());
        }
        return QVariant::fromValue(entry.process.data());
    }
    return QVariant();
}

bool DesktopModel::insertRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (row < 0 || row > d->rowCount || count < 1)
        return false;

    beginInsertRows(QModelIndex(), row, row + count - 1);
    d->rowCount = d->entries.size();
    endInsertRows();
    return true;
}

bool DesktopModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    if (row < 0 || row >= d->rowCount || count < 1)
        return false;

    const int endRow = qMin(d->rowCount - 1, row + count - 1);
    beginRemoveRows(QModelIndex(), row, endRow);
    d->rowCount = d->entries.size();
    endRemoveRows();
    return true;
}

void DesktopModel::parseEntries()
{
    QDir desktopDir("desktop");
    foreach (const QString &fileEntry, desktopDir.entryList(QStringList("*.desktop"), QDir::Files|QDir::NoDotAndDotDot, QDir::Name)) {
        QFile file(desktopDir.absoluteFilePath(fileEntry));
        QString section;
        if (file.open(QFile::ReadOnly)) {
            DesktopEntry entry;
            while (!file.atEnd()) {
                QString line = file.readLine().trimmed();
                if (line.startsWith('[') && line.endsWith(']')) {
                    section = line.mid(1, line.length() - 2);
                    continue;
                }

                if (section == QStringLiteral("Desktop Entry")) {
                    const QStringList parts = line.split('=');
                    if (parts.length() != 2)
                        continue;

                    const QString &key = parts[0];
                    const QString &val = parts[1];

                    if (key.startsWith("X-")) {
                        entry.extensions.insert(key.mid(2), val);
                        continue;
                    }

                    switch (d->roles.value(key)) {
                    case Type:
                        entry.type = DesktopEntry::types().value(val);
                        break;
                    case Version:
                        entry.version = val;
                        break;
                    case Name:
                        entry.name = val;
                        break;
                    case GenericName:
                        entry.genericName = val;
                        break;
                    case NoDisplay:
                        entry.noDisplay = val.toLower() == "true";
                        break;
                    case Comment:
                        entry.comment = val;
                        break;
                    case Icon:
                        entry.icon = QUrl::fromLocalFile(desktopDir.absoluteFilePath(val));
                        break;
                    case Hidden:
                        entry.hidden = val.toLower() == "true";
                        break;
                    case OnlyShowIn:
                        entry.onlyShowIn = val.split(';');
                        break;
                    case NotShownIn:
                        entry.notShowIn = val.split(';');
                        break;
                    case DBusActivatable:
                        entry.dbusActivatable = val.toLower() == "true";
                        break;
                    case TryExec:
                        entry.tryExec = val;
                        break;
                    case Exec:
                        entry.exec = desktopDir.absoluteFilePath(val);
                        break;
                    case Path:
                        entry.path = val;
                        break;
                    case Terminal:
                        entry.terminal = val.toLower() == "true";;
                        break;
                    case Actions:
                        foreach (const QString &action, val.split(';'))
                            entry.actions[action] = QVariantMap();
                        break;
                    case MimeType:
                        entry.mimeType = val.split(';');
                        break;
                    case Categories:
                        entry.categories = val.split(';');
                        break;
                    case Keywords:
                        entry.keywords = val.split(';');
                        break;
                    case StartupNotify:
                        entry.startupNotify = val.toLower() == "true";;
                        break;
                    case StartupWMClass:
                        entry.startupWMClass = val;
                        break;
                    case URL:
                        entry.url = val;
                        break;
                    default:
                        break;
                    }
                    continue;
                }

                if (entry.extensions.contains(section)) {
                    qWarning("Implement me!");
                    continue;
                }
            }
            d->entries.append(entry);
            insertRow(d->rowCount, QModelIndex());
        }
    }
}

/*static inline remove(DesktopModel *model, DesktopModelPrivate *d, const DesktopEntry &item)
{
    const int row = d->entries.indexOf(item);
    if (row >= 0) {
        d->entries.removeAt(row);
        model->removeRow(row);
    }
}*/
