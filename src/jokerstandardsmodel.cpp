#include "jokerstandardsmodel.h"
#include "jokersettingsstorage.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(jkStandardsModel)

enum {
    StandardCodeRole = Qt::UserRole + 1,
    StandardNameRole
};

// JokerStandardsModel

JokerStandardsModel::JokerStandardsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void JokerStandardsModel::setSettingsStorage(JokerSettingsStorage *settingsStorage)
{
    if (m_settingsStorage == settingsStorage)
        return;

    m_settingsStorage = settingsStorage;
    qCDebug(jkStandardsModel) << "Set settings storage:" << m_settingsStorage;
    emit settingsStorageChanged(m_settingsStorage);

    beginResetModel();
    setupStandards();
    endResetModel();
}

JokerSettingsStorage *JokerStandardsModel::settingsStorage() const
{
    return m_settingsStorage;
}

void JokerStandardsModel::setupStandards()
{
    m_standards.clear();
    if (!m_settingsStorage)
        return;

    for (const auto &channel : qAsConst(m_settingsStorage->m_channels)) {
        if (channel.m_source != JokerTelevision::TerrestrialSource)
            continue;
        if (m_standards.contains(channel.m_standard))
            continue;
        m_standards.append(channel.m_standard);
    }
}

int JokerStandardsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_standards.count();
}

QVariant JokerStandardsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0)
        return QVariant();

    if (index.row() >= m_standards.count())
        return QVariant();

    const auto row = index.row();

    switch (role) {
    case StandardCodeRole:
        return m_standards.at(row);
    case StandardNameRole:
        return JokerTelevision::getStandardName(m_standards.at(row));
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> JokerStandardsModel::roleNames() const
{
    return { { StandardCodeRole, "standardCode" },
        { StandardNameRole, "standardName" },
    };
}

// JokerStandardsProxyModel

JokerStandardsProxyModel::JokerStandardsProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

JokerStandardsProxyModel::~JokerStandardsProxyModel()
{
    // Reset selections from all terrestrial channels
    setChoosingFlags(UnspecifiedStandardFlag);
}

void JokerStandardsProxyModel::setChoosingFlags(ChoosingFlags choosingFlags)
{
    if (m_choosingFlags == choosingFlags)
        return;

    const auto changedFlags = m_choosingFlags ^ choosingFlags;

    // Setup or cleanup channels selections in storage
    // in depends on selected checkboxes.
    if (const auto m = static_cast<JokerStandardsModel *>(sourceModel())) {
        if (auto storage = m->settingsStorage()) {
            for (auto &channel : storage->m_channels) {

                if (channel.m_source != JokerTelevision::TerrestrialSource)
                    continue;

                // DVB-T
                if ((changedFlags & DvbtStandardFlag)
                        && (channel.m_standard == JokerTelevision::DvbtStandard)) {
                    channel.m_selected = choosingFlags & DvbtStandardFlag;
                }

                // DVB-T2
                if ((changedFlags & DvbsgtStandardFlag)
                        && (channel.m_standard == JokerTelevision::DvbsgtStandard)) {
                    channel.m_selected = choosingFlags & DvbsgtStandardFlag;
                }

                // ATSC
                if ((changedFlags & AtscStandardFlag)
                        && (channel.m_standard == JokerTelevision::AtscStandard)) {
                    channel.m_selected = choosingFlags & AtscStandardFlag;
                }

                // ISDB-T
                if ((changedFlags & IsdbtStandardFlag)
                        && (channel.m_standard == JokerTelevision::IdsbtStandard)) {
                    channel.m_selected = choosingFlags & IsdbtStandardFlag;
                }

                // DTMB
                if ((changedFlags & DtmbStandardFlag)
                        && (channel.m_standard == JokerTelevision::DtmbStandard)) {
                    channel.m_selected = choosingFlags & DtmbStandardFlag;
                }

                // DVB-C
                if ((changedFlags & DvbcStandardFlag)
                        && (channel.m_standard == JokerTelevision::DvbcAnnexCStandard)) {
                    channel.m_selected = choosingFlags & DvbcStandardFlag;
                }
            }
        }
    }

    m_choosingFlags = choosingFlags;
    qCDebug(jkStandardsModel) << "Choose:" << m_choosingFlags;
    emit choosingFlagsChanged(m_choosingFlags);

    invalidateFilter();
}

JokerStandardsProxyModel::ChoosingFlags JokerStandardsProxyModel::choosingFlags() const
{
    return m_choosingFlags;
}

bool JokerStandardsProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);

    if (const auto m = static_cast<JokerStandardsModel *>(sourceModel())) {
        const auto sourceIndex = m->index(source_row);
        const auto standardCode = m->data(sourceIndex, StandardCodeRole).value<JokerTelevision::Standard>();
        if (standardCode == JokerTelevision::DvbtStandard
                && (m_choosingFlags & DvbtStandardFlag)) {
            return true;
        } else if (standardCode == JokerTelevision::DvbsgtStandard
                   && (m_choosingFlags & DvbsgtStandardFlag)) {
            return true;
        } else if (standardCode == JokerTelevision::AtscStandard
                   && (m_choosingFlags & AtscStandardFlag)) {
            return true;
        } else if (standardCode == JokerTelevision::IdsbtStandard
                   && (m_choosingFlags & IsdbtStandardFlag)) {
            return true;
        } else if (standardCode == JokerTelevision::DtmbStandard
                   && (m_choosingFlags & DtmbStandardFlag)) {
            return true;
        } else if (standardCode == JokerTelevision::DvbcAnnexCStandard
                   && (m_choosingFlags & DvbcStandardFlag)) {
            return true;
        }
    }

    return false;
}

QDebug operator<<(QDebug debug, JokerStandardsProxyModel::ChoosingFlags choosingFlags)
{
    debug << "Standard(";
    QStringList standardList;
    if (choosingFlags == JokerStandardsProxyModel::UnspecifiedStandardFlag) {
        standardList << QLatin1String("None");
    } else {
        if (choosingFlags & JokerStandardsProxyModel::DvbtStandardFlag)
            standardList << QLatin1String("DVB-T");
        if (choosingFlags & JokerStandardsProxyModel::DvbsgtStandardFlag)
            standardList << QLatin1String("DVB-T2");
        if (choosingFlags & JokerStandardsProxyModel::AtscStandardFlag)
            standardList << QLatin1String("ATSC");
        if (choosingFlags & JokerStandardsProxyModel::IsdbtStandardFlag)
            standardList << QLatin1String("ISDB-T");
        if (choosingFlags & JokerStandardsProxyModel::DtmbStandardFlag)
            standardList << QLatin1String("DTMB");
        if (choosingFlags & JokerStandardsProxyModel::DvbcStandardFlag)
            standardList << QLatin1String("DVB-C");
    }
    debug << standardList.join(QLatin1Char('|'));
    debug << ')';
    return debug;
}
