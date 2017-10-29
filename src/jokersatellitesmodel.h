#ifndef JOKERSATELLITESMODEL_H
#define JOKERSATELLITESMODEL_H

#include "jokerchannel.h"

#include <QPointer>
#include <QSortFilterProxyModel>
#include <QVector>

class JokerSettingsStorage;

// JokerSatellitesModel

class JokerSatellitesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(JokerSettingsStorage *settingsStorage READ settingsStorage
               WRITE setSettingsStorage NOTIFY settingsStorageChanged)

public:
    explicit JokerSatellitesModel(QObject *parent = nullptr);

    void setSettingsStorage(JokerSettingsStorage *settingsStorage);
    JokerSettingsStorage *settingsStorage() const;

signals:
    void settingsStorageChanged(JokerSettingsStorage *settingsStorage);

private:
    void setupSatellites();

    int rowCount(const QModelIndex &parent) const final;
    QVariant data(const QModelIndex &index, int role) const final;
    QHash<int, QByteArray> roleNames() const final;

    QStringList m_satellites;
    QPointer<JokerSettingsStorage> m_settingsStorage; // not owned!

    friend class JokerStandardsProxyModel;
};

#endif // JOKERSATELLITESMODEL_H
