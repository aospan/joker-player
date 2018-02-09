#include "jokerprogrammodel.h"
#include "jokeraccessprovider.h"
#include "jokersettingsstorage.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(jkProgramModel)

enum {
    SourceRole = Qt::UserRole + 1,
    IdentifierRole,
    PositionRole,
    DescriptionRole,
    ProgramRole
};

// JokerProgramModel

JokerProgramModel::JokerProgramModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

JokerProgram JokerProgramModel::adjustProgram(JokerProgram program) const
{
    const auto programIt = std::max_element(m_programs.cbegin(), m_programs.cend(),
                                            [](const JokerProgram &a, const JokerProgram &b) {
        return a.m_position < b.m_position;
    });

    if (programIt == m_programs.cend())
        program.m_position = 1;
    else
        program.m_position = (programIt->m_position + 1);

    return program;
}

void JokerProgramModel::addProgram(const JokerProgram &program)
{
    if (m_programs.contains(program)) {
        qCWarning(jkProgramModel) << "Nothing to add, program already is in model:" << program;
        return;
    }

    // Fix program position
    const JokerProgram adjusted = adjustProgram(program);

   qCDebug(jkProgramModel) << "Add adjusted program:" << adjusted;

    const auto rowsCount = m_programs.count();
    beginInsertRows(QModelIndex(), rowsCount, rowsCount);
    m_programs.append(adjusted);
    endInsertRows();

    if (m_settingsStorage)
        m_settingsStorage->saveProgram(adjusted);
}

void JokerProgramModel::removeProgram(const JokerProgram &program)
{
    if (program.m_id >= 0) {
        if (!m_programs.contains(program)) {
            qCWarning(jkProgramModel) << "Nothing to remove, program is not in model:" << program;
            return;
        }

        qCDebug(jkProgramModel) << "Remove program:" << program;

        const auto row = m_programs.indexOf(program);
        beginRemoveRows(QModelIndex(), row, row);
        m_programs.takeAt(row);
        endRemoveRows();
    }
    else {
        qCDebug(jkProgramModel) << "Remove all program";

        const auto rowsCount = m_programs.count();
        beginRemoveRows(QModelIndex(), 0, rowsCount -1);
        m_programs.clear();
        endRemoveRows();

        if (m_settingsStorage)
            m_settingsStorage->deletePrograms();
    }
}

void JokerProgramModel::updateProgram(const JokerProgram &program)
{
    if (!m_programs.contains(program)) {
        qCWarning(jkProgramModel) << "Nothing to update, program is not in model:" << program;
        return;
    }

    const auto row = m_programs.indexOf(program);
    auto &adjusted = m_programs[row];
    // Update only program description and aspect ratio!
    if (adjusted.m_description == program.m_description
            && adjusted.m_aspectRatio == program.m_aspectRatio) {
        qCWarning(jkProgramModel) << "Nothing to update, as description and aspect ration are equals:" << program;
        return;
    }

    adjusted.m_description = program.m_description;
    adjusted.m_aspectRatio = program.m_aspectRatio;

    qCDebug(jkProgramModel) << "Update program:" << adjusted;

    const auto modelIndex = index(row, 0);
    emit dataChanged(modelIndex, modelIndex);

    if (m_settingsStorage)
        m_settingsStorage->saveProgram(adjusted);
}

void JokerProgramModel::cleanup()
{
    qCDebug(jkProgramModel) << "Cleanup";
    if (!m_programs.count())
        return;

    beginResetModel();
    m_programs.clear();
    endResetModel();
}

void JokerProgramModel::setAccessProvider(JokerAccessProvider *accessProvider)
{
    if (m_accessProvider) {
        disconnect(m_accessProvider, &JokerAccessProvider::programDiscovered,
                   this, &JokerProgramModel::addProgram);
        disconnect(m_accessProvider, &JokerAccessProvider::programUpdated,
                   this, &JokerProgramModel::updateProgram);
        disconnect(m_accessProvider, &JokerAccessProvider::programRemoved,
                   this, &JokerProgramModel::removeProgram);
        // Add other disconnections!
    }

    m_accessProvider = accessProvider;
    qCDebug(jkProgramModel) << "Set access provider:" << m_accessProvider;
    if (m_accessProvider) {
        connect(m_accessProvider, &JokerAccessProvider::programDiscovered,
                this, &JokerProgramModel::addProgram);
        connect(m_accessProvider, &JokerAccessProvider::programUpdated,
                this, &JokerProgramModel::updateProgram);
        connect(m_accessProvider, &JokerAccessProvider::programRemoved,
                   this, &JokerProgramModel::removeProgram);
        // Add others connections!
    }

    emit accessProviderChanged(m_accessProvider);
}

JokerAccessProvider *JokerProgramModel::accessProvider() const
{
    return m_accessProvider;
}

void JokerProgramModel::setSettingsStorage(JokerSettingsStorage *settingsStorage)
{
    if (m_settingsStorage == settingsStorage)
        return;
    m_settingsStorage = settingsStorage;
    qCDebug(jkProgramModel) << "Set settings storage:" << m_settingsStorage;
    emit settingsStorageChanged(m_settingsStorage);

    beginResetModel();
    m_programs = m_settingsStorage ? m_settingsStorage->storedPrograms() : QVector<JokerProgram>();
    endResetModel();
}

JokerSettingsStorage *JokerProgramModel::settingsStorage() const
{
    return m_settingsStorage;
}

void JokerProgramModel::notifyUpdate()
{
    emit dataChanged(index(0), index(m_programs.count() - 1));
}

int JokerProgramModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_programs.count();
}

QVariant JokerProgramModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0)
        return QVariant();

    if (index.row() >= m_programs.count())
        return QVariant();

    const auto row = index.row();
    const auto program = m_programs.at(row);

    switch (role) {
    case SourceRole: {
        const auto channel = m_settingsStorage->m_channels.at(program.m_channelIndex);
        return channel.m_source;
    }
    case IdentifierRole:
        return program.m_id;
    case PositionRole:
        return program.m_position;
    case DescriptionRole:
        return program.m_description;
    case ProgramRole:
        return QVariant::fromValue(program);
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> JokerProgramModel::roleNames() const
{
    return {
        { SourceRole, "source" },
        { IdentifierRole, "id" },
        { PositionRole, "position" },
        { DescriptionRole, "description" },
        { ProgramRole, "program" }
    };
}

bool JokerProgramModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    switch (role) {

    case PositionRole: {
        const auto currentProgram = m_programs.at(index.row());
        const auto currentPosition = currentProgram.m_position;
        const auto desiredPosition = value.toInt();

        if (desiredPosition < 1) {
            qCWarning(jkProgramModel) << "Unable to change program position to" << desiredPosition;
            notifyUpdate();
            return false;
        }

        for (auto &programIt : m_programs) {
            const auto position = programIt.m_position;
            if (currentPosition > desiredPosition) {
                if (position == currentPosition)
                    programIt.m_position = desiredPosition;
                else if (position >= desiredPosition && position < currentPosition)
                    programIt.m_position = position + 1;
                else
                    continue; // Required to store only modfied programs.
            } else if (currentPosition < desiredPosition) {
                if (position == currentPosition)
                    programIt.m_position = desiredPosition;
                else if (position <= desiredPosition && position > currentPosition)
                    programIt.m_position = position - 1;
                else
                    continue; // Required to store only modfied programs.
            } else {
                continue; // Required to store only modfied programs.
            }

            // Store only modfied programs!
            if (m_settingsStorage)
                m_settingsStorage->saveProgram(programIt);
        }

        notifyUpdate();
    }
        break;

    default:
        return false;
    }

    return true;
}

Qt::ItemFlags JokerProgramModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}


// JokerProgramProxyModel

JokerProgramProxyModel::JokerProgramProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setSortRole(PositionRole);
    sort(0);
}

void JokerProgramProxyModel::setPermissions(Permissions permissions)
{
    if (m_permissions == permissions)
        return;

    m_permissions = permissions;
    emit permissionsChanged(m_permissions);

    invalidateFilter();
}

JokerProgramProxyModel::Permissions JokerProgramProxyModel::permissions() const
{
    return m_permissions;
}

bool JokerProgramProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);

    if (const auto m = static_cast<JokerProgramModel *>(sourceModel())) {
        const auto sourceIndex = m->index(source_row);
        const auto source = m->data(sourceIndex, SourceRole)
                .value<JokerTelevision::Source>();
        if (source == JokerTelevision::Source::TerrestrialSource
                && (m_permissions & TerrestrialPermission)) {
            return true;
        } else if (source == JokerTelevision::Source::SatelliteSource
                   && (m_permissions & SatellitePermission)) {
            return true;
        }
    }

    return false;
}

bool JokerProgramProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (QSortFilterProxyModel::setData(index, value, role)) {
        invalidate(); // Force call for model re-sorting!
        return true;
    }
    return false;
}
