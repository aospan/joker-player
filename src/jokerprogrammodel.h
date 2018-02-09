#ifndef JOKERPROGRAMMODEL_H
#define JOKERPROGRAMMODEL_H

#include "jokerprogram.h"

#include <QPointer>
#include <QSortFilterProxyModel>

class JokerAccessProvider;
class JokerSettingsStorage;

// JokerProgramModel

class JokerProgramModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(JokerAccessProvider *accessProvider READ accessProvider
               WRITE setAccessProvider NOTIFY accessProviderChanged)

    Q_PROPERTY(JokerSettingsStorage *settingsStorage READ settingsStorage
               WRITE setSettingsStorage NOTIFY settingsStorageChanged)

public:
    explicit JokerProgramModel(QObject *parent = nullptr);

    void cleanup();

    void setAccessProvider(JokerAccessProvider *accessProvider);
    JokerAccessProvider *accessProvider() const;

    void setSettingsStorage(JokerSettingsStorage *settingsStorage);
    JokerSettingsStorage *settingsStorage() const;

signals:
    void accessProviderChanged(JokerAccessProvider *accessProvider);
    void settingsStorageChanged(JokerSettingsStorage *settingsStorage);

private:
    JokerProgram adjustProgram(JokerProgram program) const;

    void addProgram(const JokerProgram &program);
    void removeProgram(const JokerProgram &program);
    void updateProgram(const JokerProgram &program);

    void notifyUpdate();

    int rowCount(const QModelIndex &parent) const final;
    QVariant data(const QModelIndex &index, int role) const final;
    QHash<int, QByteArray> roleNames() const final;
    Qt::ItemFlags flags(const QModelIndex &index) const final;
    bool setData(const QModelIndex &index, const QVariant &value, int role) final;

    QVector<JokerProgram> m_programs;
    QPointer<JokerAccessProvider> m_accessProvider; // not owned!
    QPointer<JokerSettingsStorage> m_settingsStorage; // not owned

    friend class JokerProgramProxyModel;
    friend class JokerPlayer;
};


// JokerProgramProxyModel

class JokerProgramProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(Permission permissions READ permissions WRITE setPermissions NOTIFY permissionsChanged)

public:
    // Note: Do not use as 'enum class' as it
    // interprets as flags!
    enum Permission {
        NoPermission = 0x0,
        TerrestrialPermission = 0x1,
        SatellitePermission = 0x2,
        AllPermissions = TerrestrialPermission | SatellitePermission
    };
    Q_FLAG(Permission)
    Q_DECLARE_FLAGS(Permissions, Permission)

    explicit JokerProgramProxyModel(QObject *parent = nullptr);

    void setPermissions(Permissions permissions);
    Permissions permissions() const;

signals:
    void permissionsChanged(Permissions permissions);

private:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const final;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) final;

    Permissions m_permissions = Permission::AllPermissions;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(JokerProgramProxyModel::Permissions)

#endif // JOKERPROGRAMMODEL_H
