#include "jokerchannelsmodel.h"
#include "jokersettingsstorage.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(jkChannelsModel)

enum {
    ChannelStandardRole = Qt::UserRole + 1,
    ChannelFrequenyRole,
    ChannelSelectedRole,
    ChannelSignalSourceRole,
    ChannelNameRole,
    ChannelModulationRole,
    ChannelVoltageRole
};

// JokerChannelsModel

JokerChannelsModel::JokerChannelsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void JokerChannelsModel::setSettingsStorage(
        JokerSettingsStorage *settingsStorage)
{
    if (m_settingsStorage == settingsStorage)
        return;

    m_settingsStorage = settingsStorage;
    qCDebug(jkChannelsModel) << "Set settings storage:" << m_settingsStorage;
    emit settingsStorageChanged(m_settingsStorage);

    beginResetModel();
    // Do nothing
    endResetModel();
}

JokerSettingsStorage *JokerChannelsModel::settingsStorage() const
{
    return m_settingsStorage;
}

int JokerChannelsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_settingsStorage ? m_settingsStorage->m_channels.count() : 0;
}

QVariant JokerChannelsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0)
        return QVariant();

    if (!m_settingsStorage)
        return QVariant();

    if (index.row() >= m_settingsStorage->m_channels.count())
        return QVariant();

    const auto row = index.row();
    const auto channel = m_settingsStorage->m_channels.at(row);

    switch (role) {
    case ChannelStandardRole:
        return channel.m_standard;
    case ChannelFrequenyRole:
        return channel.m_frequencyMhz;
    case ChannelSelectedRole:
        return channel.m_selected;
    case ChannelSignalSourceRole:
        return channel.m_source;
    case ChannelNameRole:
        return channel.m_caption;
    case ChannelModulationRole:
        return channel.m_modulation;
    case ChannelVoltageRole:
        return channel.m_voltage;
    default:
        return QVariant();
    }
}

bool JokerChannelsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!m_settingsStorage)
        return false;

    if (!index.isValid())
        return false;

    const auto row = index.row();
    if (row < 0 || row > m_settingsStorage->m_channels.count())
        return false;

    switch (role) {

    case ChannelSelectedRole: {
        auto &channel = m_settingsStorage->m_channels[row];
        const auto checked = value.toBool();
        channel.m_selected = checked;
    }
        break;

    default:
        return false;
    }

    emit dataChanged(index, index);

    return true;
}

Qt::ItemFlags JokerChannelsModel::flags(const QModelIndex &index) const
{
    return (index.isValid()) ? (QAbstractListModel::flags(index) | Qt::ItemIsEditable)
                             : Qt::ItemIsEnabled;
}

QHash<int, QByteArray> JokerChannelsModel::roleNames() const
{
    return {
        { ChannelStandardRole, "standard" },
        { ChannelFrequenyRole, "frequency" },
        { ChannelSelectedRole, "selected" } ,
        { ChannelSignalSourceRole, "signalSource" },
        { ChannelNameRole, "channelName" },
        { ChannelModulationRole, "modulation" },
        { ChannelVoltageRole, "voltage" }
    };
}

// JokerChannelsProxyModel

JokerChannelsProxyModel::JokerChannelsProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void JokerChannelsProxyModel::setSignalSource(JokerTelevision::Source signalSource)
{
    if (m_signalSource == signalSource)
        return;

    m_signalSource = signalSource;
    qCDebug(jkChannelsModel) << "Choose signal source:"
                             << m_signalSource;
    emit signalSourceChanged(m_signalSource);

    invalidateFilter();
}

JokerTelevision::Source JokerChannelsProxyModel::signalSource() const
{
    return m_signalSource;
}


void JokerChannelsProxyModel::setStandardCode(
        JokerTelevision::Standard standardCode)
{
    if (m_standardCode == standardCode)
        return;

    m_standardCode = standardCode;
    qCDebug(jkChannelsModel) << "Choose standard:"
                             << m_standardCode;
    emit standardCodeChanged(m_standardCode);

    invalidateFilter();
}

JokerTelevision::Standard JokerChannelsProxyModel::standardCode() const
{
    return m_standardCode;
}

void JokerChannelsProxyModel::setChannelName(const QString &channelName)
{
    if (m_channelName == channelName)
        return;

    // Setup or cleanup channels selections in storage
    // in depends on selected satellite.
    if (const auto m = static_cast<JokerChannelsModel *>(sourceModel())) {
        if (auto storage = m->settingsStorage()) {
            for (auto &channel : storage->m_channels) {
                if (channel.m_source != JokerTelevision::SatelliteSource)
                    continue;
                channel.m_selected = (channel.m_caption == channelName);
            }
        }
    }

    m_channelName = channelName;
    qCDebug(jkChannelsModel) << "Choose channel:"
                             << m_channelName;
    emit channelNameChanged(m_channelName);

    invalidateFilter();
}

QString JokerChannelsProxyModel::channelName() const
{
    return m_channelName;
}

void JokerChannelsProxyModel::selectAllChannels()
{
    const auto rows = rowCount();
    for (auto row = 0; row < rows; ++row) {
        const auto modelIndex = index(row, 0);
        setData(modelIndex, true, ChannelSelectedRole);
    }
}

void JokerChannelsProxyModel::unselectAllChannels()
{
    const auto rows = rowCount();
    for (auto row = 0; row < rows; ++row) {
        const auto modelIndex = index(row, 0);
        setData(modelIndex, false, ChannelSelectedRole);
    }
}

bool JokerChannelsProxyModel::filterAcceptsRow(
        int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);

    if (const auto m = static_cast<JokerChannelsModel *>(sourceModel())) {
        const auto sourceIndex = m->index(source_row);

        // Filter for signal source
        const auto signalSource = m->data(sourceIndex, ChannelSignalSourceRole)
                .value<JokerTelevision::Source>();
        if (signalSource != m_signalSource)
            return false;

        if (signalSource == JokerTelevision::TerrestrialSource) {
            // Filter for terrestrial standard
            const auto standard = m->data(sourceIndex, ChannelStandardRole)
                    .value<JokerTelevision::Standard>();
            if (standard != m_standardCode)
                return false;
        } else if (signalSource == JokerTelevision::SatelliteSource) {
            // Filter for channel name (it is satellite name)
            const auto channelName = m->data(sourceIndex, ChannelNameRole).toString();
            if (channelName != m_channelName)
                return false;
        }

        return true;
    }

    return false;
}
