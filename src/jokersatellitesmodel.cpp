#include "jokersatellitesmodel.h"
#include "jokersettingsstorage.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(jkSatellitesModel)

enum {
    SatelliteNameRole = Qt::UserRole + 1,
};

// JokerSatellitesModel

JokerSatellitesModel::JokerSatellitesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void JokerSatellitesModel::setSettingsStorage(JokerSettingsStorage *settingsStorage)
{
    if (m_settingsStorage == settingsStorage)
        return;

    m_settingsStorage = settingsStorage;
    qCDebug(jkSatellitesModel) << "Set settings storage:" << m_settingsStorage;
    emit settingsStorageChanged(m_settingsStorage);

    beginResetModel();
    setupSatellites();
    endResetModel();
}

JokerSettingsStorage *JokerSatellitesModel::settingsStorage() const
{
    return m_settingsStorage;
}

void JokerSatellitesModel::setupSatellites()
{
    m_satellites.clear();
    if (!m_settingsStorage)
        return;

    for (const auto &channel : qAsConst(m_settingsStorage->m_channels)) {
        if (channel.m_source != JokerTelevision::SatelliteSource)
            continue;
        if (m_satellites.contains(channel.m_caption))
            continue;
        m_satellites.append(channel.m_caption);
    }
}

int JokerSatellitesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_satellites.count();
}

QVariant JokerSatellitesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0)
        return QVariant();

    if (index.row() >= m_satellites.count())
        return QVariant();

    const auto row = index.row();

    switch (role) {
    case SatelliteNameRole:
        return m_satellites.at(row);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> JokerSatellitesModel::roleNames() const
{
    return { { SatelliteNameRole, "satelliteName" } };
}
