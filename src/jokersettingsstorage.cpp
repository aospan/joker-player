#include "jokersettingsstorage.h"
#include "jokerprogrammodel.h"

#include <QTimer>
#include <QUuid>
#include <QDir>
#include <QXmlStreamReader>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(jkSettingsStorage)

const int kStorageUpdateInterval = 2000; // 2 sec
const char kProgramsTable[] = "Programs";
const char kChannelsTable[] = "Channels";
const char kSettingsTable[] = "Settings";

enum ProgramPropertyIndex {
    ProgramChannelIndex,
    ProgramIdIndex,
    ProgramPositionIndex,
    ProgramDescriptionIndex,
    ProgramAspectRatio
};

enum ChannelPropertyIndex {
    ChannelCaptionIndex,
    ChannelSourceIndex,
    ChannelStandardIndex,
    ChannelModulationIndex,
    ChannelVoltageIndex,
    ChannelFrequencyIndex,
    ChannelBandwidthIndex,
    ChannelSymbolRateIndex
};

enum SettingPropertyIndex {
    SettingIdIndex,
    SettingPresetLowFrequencyIndex,
    SettingPresetHighFrequencyIndex,
    SettingPresetSwitchFrequencyIndex,
    SettingPresetUsedIndex,
    SettingToneIndex
};

// JokerLnbPreset

JokerLnbPreset::JokerLnbPreset(int lowFrequency,
                               int highFrequency,
                               int switchFrequency)
    : m_lowFrequency(lowFrequency)
    , m_highFrequency(highFrequency)
    , m_switchFrequency(switchFrequency)
{
}

bool JokerLnbPreset::operator==(const JokerLnbPreset &other) const
{
    return m_lowFrequency == other.m_lowFrequency
            && m_highFrequency == other.m_highFrequency
            && m_switchFrequency == other.m_switchFrequency
            ;
}

bool JokerLnbPreset::operator!=(const JokerLnbPreset &other) const
{
    return !(*this == other);
}

// JokerSettingsStorage

JokerSettingsStorage::JokerSettingsStorage(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_connection(QUuid::createUuid().toString())
{
    // Initialize database
    openDatabase();

    restoreChannels();
    restoreSettings();

    // Initialize timer
    m_timer->setSingleShot(true);
    m_timer->setInterval(kStorageUpdateInterval);
    connect(m_timer, &QTimer::timeout,
            this, &JokerSettingsStorage::processPendings);
}

JokerSettingsStorage::~JokerSettingsStorage()
{
    processPendings();

    closeDatabase();
}

JokerTelevision::Tone JokerSettingsStorage::tone() const
{
    return m_tone;
}

void JokerSettingsStorage::setTone(JokerTelevision::Tone tone)
{
    if (m_tone == tone)
        return;

    auto db = QSqlDatabase::database(m_connection);
    QSqlQuery query(db);
    const QString updateQuery = QLatin1String("update ")
            + QLatin1String(kSettingsTable)
            + QLatin1String(" set tone = ")
            + QString::number(tone)
            + QLatin1String(" where id = 1");
    if (!query.exec(updateQuery)) {
        qCCritical(jkSettingsStorage) << "Unable to save tone to database:" << db.lastError();
    }

    m_tone = tone;
    qCDebug(jkSettingsStorage) << "Set tone:" << m_tone;
    emit toneChanged(m_tone);
}

int JokerSettingsStorage::presetIndex() const
{
    return m_presetIndex;
}

void JokerSettingsStorage::setPresetIndex(int presetIndex)
{
    if (m_presetIndex == presetIndex)
        return;

    auto db = QSqlDatabase::database(m_connection);
    QSqlQuery query(db);
    const QString updateQuery = QLatin1String("update ")
            + QLatin1String(kSettingsTable)
            + QLatin1String(" set currentpresetindex = ")
            + QString::number(presetIndex)
            + QLatin1String(" where id = 1");
    if (!query.exec(updateQuery)) {
        qCCritical(jkSettingsStorage) << "Unable to save preset index to database:" << db.lastError();
    }

    m_presetIndex = presetIndex;
    qCDebug(jkSettingsStorage) << "Set preset index:" << m_presetIndex;
    emit presetIndexChanged(m_presetIndex);

    const auto m_preset = preset();
    emit presetChanged(m_preset);
}

JokerLnbPreset JokerSettingsStorage::preset() const
{
    return m_presets.at(m_presetIndex);
}

void JokerSettingsStorage::setPreset(const JokerLnbPreset &preset)
{
    auto &m_preset = m_presets[m_presetIndex];
    if (m_preset == preset)
        return;

    if (m_presetIndex == ManualPresetIndex) {
        auto db = QSqlDatabase::database(m_connection);
        QSqlQuery query(db);
        const QString updateQuery = QLatin1String("update ")
                + QLatin1String(kSettingsTable)
                + QLatin1String(" set userpresetlowfrequency = ")
                + QString::number(preset.m_lowFrequency)
                + QLatin1String(", userpresethighfrequency = ")
                + QString::number(preset.m_highFrequency)
                + QLatin1String(", userpresetswitchfrequency = ")
                + QString::number(preset.m_switchFrequency)
                + QLatin1String(" where id = 1");
        if (!query.exec(updateQuery)) {
            qCCritical(jkSettingsStorage) << "Unable to save manual preset to database:" << db.lastError();
        }
    }

    m_preset = preset;
    qCDebug(jkSettingsStorage) << "Set preset:" << m_preset.m_lowFrequency
                               << m_preset.m_highFrequency  << m_preset.m_switchFrequency;
    emit presetChanged(m_preset);
}

void JokerSettingsStorage::openDatabase()
{
    const QString database = QDir::homePath() + QLatin1String("/joker-player.db");
    auto db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), m_connection);
    db.setDatabaseName(database);
    if (!db.open()) {
        qCCritical(jkSettingsStorage) << "Unable to open database:" << db.lastError();
        return;
    }

    // Create programs table
    const QString createProgramsQuery = QLatin1String("create table if not exists ")
            + QLatin1String(kProgramsTable)
            + QLatin1String("(channel integer, "
                            "id integer, "
                            "position integer, "
                            "description text, "
                            "aspectratio integer, "
                            "primary key (channel, id) on conflict replace)");

    QSqlQuery programsQuery(db);
    if (!programsQuery.prepare(createProgramsQuery) || !programsQuery.exec()) {
        qCCritical(jkSettingsStorage) << "Unable to create programs table:" << db.lastError();
    }

    // Create channels table
    const QString createChannelsQuery = QLatin1String("create table if not exists ")
            + QLatin1String(kChannelsTable)
            + QLatin1String("(caption text, "
                            "source integer, "
                            "standard integer, "
                            "modulation integer, "
                            "voltage integer, "
                            "frequency integer, "
                            "bandwidth integer, "
                            "symbolrate integer, "
                            "primary key (caption, source, standard, modulation, voltage, "
                            "frequency, bandwidth, symbolrate) on conflict replace)");

    QSqlQuery channelsQuery(db);
    if (!channelsQuery.prepare(createChannelsQuery) || !channelsQuery.exec()) {
        qCCritical(jkSettingsStorage) << "Unable to create channels table:" << db.lastError();
    }

    // Create settings table
    const QString createSettingsQuery = QLatin1String("create table if not exists ")
            + QLatin1String(kSettingsTable)
            + QLatin1String("(id integer primary key autoincrement not null, "
                            "userpresetlowfrequency integer, "
                            "userpresethighfrequency integer, "
                            "userpresetswitchfrequency integer, "
                            "currentpresetindex integer,"
                            "tone integer"
                            ")");

    QSqlQuery settingsQuery(db);
    if (!settingsQuery.prepare(createSettingsQuery) || !settingsQuery.exec()) {
        qCCritical(jkSettingsStorage) << "Unable to create settings table:" << db.lastError();
    } else {
        const QString selectQuery = QLatin1String("select count(*) from ")
                + QLatin1String(kSettingsTable);
        QSqlQuery query(db);
        if (query.exec(selectQuery)) {
            if (query.next()) {
                const auto recordsCount = query.value(0).toInt();
                if (recordsCount == 0) {
                    QSqlQuery query(db);
                    const QString insertQuery = QLatin1String("insert into ")
                            + QLatin1String(kSettingsTable)
                            + QLatin1String("(userpresetlowfrequency, userpresethighfrequency, "
                                            "userpresetswitchfrequency, currentpresetindex, tone) "
                                            "values (?, ?, ?, ?, ?)");

                    if (query.prepare(insertQuery)) {
                        query.addBindValue(0); // default preset low frequency
                        query.addBindValue(0); // default preset high frequency
                        query.addBindValue(0); // default preset switch frequency
                        query.addBindValue(m_presetIndex); // default preset index
                        query.addBindValue(m_tone); // default tone
                        if (!query.exec()) {
                            qCCritical(jkSettingsStorage) << "Unable to exec settings insert query:" << db.lastError();
                        }
                    } else {
                        qCCritical(jkSettingsStorage) << "Unable to prepare settings insert query:" << db.lastError();
                    }
                }
            } else {
                qCCritical(jkSettingsStorage) << "Unable to query settings record:" << db.lastError();
            }
        } else {
            qCCritical(jkSettingsStorage) << "Unable to query settings count:" << db.lastError();
        }
    }

    // Syncing can be slow, especially on Linux and Windows
    db.exec(QLatin1String("pragma synchronous = off;"));
}

void JokerSettingsStorage::closeDatabase()
{
    {
        auto db = QSqlDatabase::database(m_connection);
        db.close();
    }

    QSqlDatabase::removeDatabase(m_connection);
}

void JokerSettingsStorage::restoreChannels()
{
    {
        // Restore from database
        const auto db = QSqlDatabase::database(m_connection);
        const QString selectQuery = QLatin1String("select * from ")
                + QLatin1String(kChannelsTable);

        QSqlQuery query(db);
        if (query.exec(selectQuery)) {
            while (query.next()) {
                const auto caption = query.value(ChannelCaptionIndex).toString();
                const auto source = query.value(ChannelSourceIndex).toInt();
                const auto standard = query.value(ChannelStandardIndex).toInt();
                const auto modulation = query.value(ChannelModulationIndex).toInt();
                const auto voltage = query.value(ChannelVoltageIndex).toInt();
                const auto frequency = query.value(ChannelFrequencyIndex).toInt();
                const auto bandwidth = query.value(ChannelBandwidthIndex).toInt();
                const auto symbolRate = query.value(ChannelSymbolRateIndex).toInt();

                if (source < JokerTelevision::UnknownSource || source > JokerTelevision::SatelliteSource) {
                    qCCritical(jkSettingsStorage) << "Unknown source are met:" << source;
                    continue;
                } else if (standard < JokerTelevision::UnknownStandard || standard > JokerTelevision::DvbcAnnexCStandard) {
                    qCCritical(jkSettingsStorage) << "Unknown standard are met:" << standard;
                    continue;
                } else if (modulation < JokerTelevision::QpskModulation || modulation > JokerTelevision::Qam4NrModulation) {
                    qCCritical(jkSettingsStorage) << "Unknown modulation are met:" << modulation;
                    continue;
                } else if (voltage < JokerTelevision::Voltage13 || voltage > JokerTelevision::VoltageOff) {
                    qCCritical(jkSettingsStorage) << "Unknown voltage are met:" << voltage;
                    continue;
                } else {
                    JokerChannel channel;
                    channel.m_caption = caption;
                    channel.m_source = static_cast<JokerTelevision::Source>(source);
                    channel.m_standard = static_cast<JokerTelevision::Standard>(standard);
                    channel.m_modulation = static_cast<JokerTelevision::Modulation>(modulation);
                    channel.m_voltage = static_cast<JokerTelevision::Voltage>(voltage);
                    channel.m_frequencyMhz = frequency;
                    channel.m_bandwidth = bandwidth;
                    channel.m_symbolRate = symbolRate;

                    m_channels.append(channel);
                }
            }
        } else {
            qCCritical(jkSettingsStorage) << "Unable to query channels:" << db.lastError();
        }
    }

    // Read XML channels and try to append new channels to cache
    const auto channels = channelsFromConfigs();
    for (const auto &channel : channels) {
        if (m_channels.contains(channel))
            continue;
        m_channels.append(channel);

        // Save new channel to database
        saveChannel(channel);
    }
}

void JokerSettingsStorage::restoreSettings()
{
    // Default standard LNB presets
    m_presets << JokerLnbPreset(9750, 10600, 11700); // Universal
    m_presets << JokerLnbPreset(10750, 10750, 11700); // L10750
    m_presets << JokerLnbPreset(10700, 10700, 11750); // L10700

    // Restore settings from database
    // We have only one record!
    const auto db = QSqlDatabase::database(m_connection);
    const QString selectQuery = QLatin1String("select * from ")
            + QLatin1String(kSettingsTable)
            + QLatin1String(" where id = 1");

    QSqlQuery query(db);
    if (query.exec(selectQuery)) {
        // We have only one record!
        if (query.next()) {
            // Manual LNB preset
            const auto userPresetLowFrequency = query.value(SettingPresetLowFrequencyIndex).toInt();
            const auto userPresetHighFrequency = query.value(SettingPresetHighFrequencyIndex).toInt();
            const auto userPresetSwitchFrequency = query.value(SettingPresetSwitchFrequencyIndex).toInt();
            m_presets << JokerLnbPreset(userPresetLowFrequency, userPresetHighFrequency, userPresetSwitchFrequency);
            // Current preset index
            const auto usedPresetIndex = query.value(SettingPresetUsedIndex).toInt();
            m_presetIndex = usedPresetIndex;
            // Current tone
            const auto tone = query.value(SettingToneIndex).toInt();
            m_tone = static_cast<JokerTelevision::Tone>(tone);
        } else {
            qCCritical(jkSettingsStorage) << "Unable to query settings record:" << db.lastError();
        }
    } else {
        qCCritical(jkSettingsStorage) << "Unable to query settings:" << db.lastError();
    }
}

QVector<JokerChannel> JokerSettingsStorage::channelsFromConfigs() const
{
    const QString directoryPath = qApp->applicationDirPath() + QLatin1String("/xml");
    QDir configDirectory(directoryPath);
    configDirectory.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    configDirectory.setNameFilters({ QLatin1String("*.xml") });
    const auto configs = configDirectory.entryInfoList();

    QVector<JokerChannel> channels;

    if (configs.isEmpty()) {
        qCCritical(jkSettingsStorage) << "No channel configurations found in:" << directoryPath;
    } else {
        for (const auto &config : configs) {
            const auto chs = channelsFromConfig(config.absoluteFilePath());
            channels << chs;
        }
    }

    return channels;
}

QVector<JokerChannel> JokerSettingsStorage::channelsFromConfig(const QString &configPath) const
{
    qCDebug(jkSettingsStorage) << "Parse channels config:" << configPath;

    QVector<JokerChannel> channels;

    QFile config(configPath);
    if (!config.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(jkSettingsStorage) << "Unable to open config:" << configPath;
        return channels;
    }

    QXmlStreamReader stream(&config);

    while (!stream.atEnd()) {

        const auto token = stream.readNext();
        if (token != QXmlStreamReader::StartElement)
            continue;

        const auto elementName = stream.name();
        if (elementName == QLatin1String("delivery_system")) {
            // Read terrestrial standards
            const auto attributes = stream.attributes();
            const auto standard = attributes.value(QLatin1String("standard")).toInt();
            const auto source = JokerTelevision::TerrestrialSource; // We know that it is terrestrial channels
            if (standard < JokerTelevision::UnknownStandard
                    && standard > JokerTelevision::DvbcAnnexCStandard) {
                qCWarning(jkSettingsStorage) << "Unknown standard are met:" << standard;
                stream.skipCurrentElement();
            } else {
                // Read frequencies
                for (;;) {
                    const auto token = stream.readNext();
                    if (token == QXmlStreamReader::StartElement) {
                        const auto elementName = stream.name();
                        if (elementName == QLatin1String("freq")) {
                            const auto attributes = stream.attributes();
                            const auto bandwidth = attributes.value(QLatin1String("bandwidth")).toInt() * 1000000;
                            const auto frequencyMhz = attributes.value(QLatin1String("frequency_mhz")).toInt();
                            const auto modulation = attributes.value(QLatin1String("modulation")).toInt();
                            if (modulation < JokerTelevision::QpskModulation || modulation > JokerTelevision::Qam4NrModulation) {
                                qCWarning(jkSettingsStorage) << "Unknown modulation are met:" << modulation;
                                stream.skipCurrentElement();
                            } else {
                                //qCWarning(jkSettingsStorage) << "Parsed terrestrial channel:" << standard << "f:" << frequencyMhz
                                //                             << "b:" << bandwidth << "m:" << modulation;

                                JokerChannel channel;
                                channel.m_source = source;
                                channel.m_standard = static_cast<JokerTelevision::Standard>(standard);
                                channel.m_modulation = static_cast<JokerTelevision::Modulation>(modulation);
                                channel.m_frequencyMhz = frequencyMhz;
                                channel.m_bandwidth = bandwidth;

                                channels.append(channel);
                            }
                        } else {
                            stream.skipCurrentElement();
                        }
                    } else if (token == QXmlStreamReader::EndDocument) {
                        break;
                    }
                }
            }
        } else if (elementName == QLatin1String("satellites")) {
            const auto source = JokerTelevision::SatelliteSource; // We know that it is satellite channels
            for (;;) {
                const auto token = stream.readNext();
                if (token == QXmlStreamReader::StartElement) {
                    const auto elementName = stream.name();
                    if (elementName == QLatin1String("sat")) {
                        const auto attributes = stream.attributes();
                        const auto satelliteName = attributes.value(QLatin1String("name")).toString();
                        for (;;) {
                            const auto token = stream.readNext();
                            if (token == QXmlStreamReader::StartElement) {
                                const auto elementName = stream.name();
                                if (elementName == QLatin1String("transponder")) {
                                    const auto attributes = stream.attributes();
                                    const auto frequencyMhz = attributes.value(QLatin1String("frequency")).toInt() / 1000;
                                    const auto symbolRate = attributes.value(QLatin1String("symbol_rate")).toInt();
                                    const auto modulation = attributes.value(QLatin1String("modulation")).toInt();
                                    const auto polarization = attributes.value(QLatin1String("polarization")).toInt();
                                    const auto system = attributes.value(QLatin1String("system")).toInt();
                                    if (modulation < 1 || modulation > 2) {
                                        qCWarning(jkSettingsStorage) << "Unknown modulation are met:" << modulation;
                                        stream.skipCurrentElement();
                                    } else if (polarization < 0 || polarization > 1) {
                                        qCWarning(jkSettingsStorage) << "Unknown polarization are met:" << polarization;
                                        stream.skipCurrentElement();
                                    } else if (system < 0 || system > 1) {
                                        qCWarning(jkSettingsStorage) << "Unknown system are met:" << system;
                                        stream.skipCurrentElement();
                                    } else {
                                        //qCWarning(jkSettingsStorage) << "Parsed satellite channel:" << standard << "f:" << frequencyMhz
                                        //                             << "sr:" << symbolRate << "m:" << modulation << "n:" << satelliteName;

                                        JokerChannel channel;
                                        channel.m_caption = satelliteName;
                                        channel.m_source = source;
                                        channel.m_standard = (system == 0)
                                                ? JokerTelevision::DvbsStandard
                                                : ((system == 1)
                                                   ? JokerTelevision::DvbssgStandard
                                                   : JokerTelevision::UnknownStandard);
                                        channel.m_modulation = (modulation == 1)
                                                ? JokerTelevision::QpskModulation
                                                : ((modulation == 2)
                                                   ? JokerTelevision::Psk8Modulation
                                                   : JokerTelevision::QpskModulation);
                                        channel.m_voltage = (polarization == 1)
                                                ? JokerTelevision::Voltage13
                                                : ((polarization == 0)
                                                   ? JokerTelevision::Voltage18
                                                   : JokerTelevision::VoltageOff);
                                        channel.m_frequencyMhz = frequencyMhz;
                                        channel.m_symbolRate = symbolRate;

                                        channels.append(channel);
                                    }
                                }
                            } else if (token == QXmlStreamReader::EndElement) {
                                const auto elementName = stream.name();
                                if (elementName != QLatin1String("transponder"))
                                    break;
                            }
                        }
                    } else {
                        stream.skipCurrentElement();
                    }
                } else if (token == QXmlStreamReader::EndDocument) {
                    break;
                }
            }
        }
    }

    if (stream.hasError())
        qCWarning(jkSettingsStorage) << "Parsing error:" << stream.errorString();

    return channels;
}

void JokerSettingsStorage::saveProgram(const JokerProgram &program)
{
    qCDebug(jkSettingsStorage) << "Store program:" << program.m_channelIndex << program.m_id;

    auto programIt = std::find(m_pendingPrograms.begin(), m_pendingPrograms.end(), program);
    if (programIt == m_pendingPrograms.end()) {
        m_pendingPrograms.append(program);
    } else {
        *programIt = program;
    }

    m_timer->start();
}

void JokerSettingsStorage::deletePrograms() {
    auto db = QSqlDatabase::database(m_connection);

    if (!db.transaction()) {
        qCCritical(jkSettingsStorage) << "Unable to start transaction:" << db.lastError();
        return;
    }

    QSqlQuery query(db);
    const QString truncateQuery = QLatin1String("delete from ")
            + QLatin1String(kProgramsTable);

    if (!query.exec(truncateQuery)) {

        qCCritical(jkSettingsStorage) << "Unable to exec programs truncate query:" << db.lastError();
    }
    if (!db.commit()) {
        qCCritical(jkSettingsStorage) << "Unable to finish transaction:" << db.lastError();
        return;
    }

}

QVector<JokerProgram> JokerSettingsStorage::storedPrograms() const
{
    QVector<JokerProgram> programs;

    const auto db = QSqlDatabase::database(m_connection);
    const QString selectQuery = QLatin1String("select * from ")
            + QLatin1String(kProgramsTable);

    QSqlQuery query(db);
    if (query.exec(selectQuery)) {
        while (query.next()) {
            const auto channelIndex = query.value(ProgramChannelIndex).toInt();
            const auto id = query.value(ProgramIdIndex).toInt();
            const auto position = query.value(ProgramPositionIndex).toInt();
            const auto description = query.value(ProgramDescriptionIndex).toString();
            const auto aspectRatio = query.value(ProgramAspectRatio).toInt();

            JokerProgram program(channelIndex, id);
            program.m_position = position;
            program.m_description = description;
            program.m_aspectRatio = aspectRatio;

            programs.append(program);
        }
    } else {
        qCCritical(jkSettingsStorage) << "Unable to query programs:" << db.lastError();
    }

    return programs;
}

void JokerSettingsStorage::saveChannel(const JokerChannel &channel)
{
    qCDebug(jkSettingsStorage) << "Store channel:" << channel.m_source
                               << channel.m_standard << channel.m_frequencyMhz;

    auto channelIt = std::find(m_pendingChannels.begin(), m_pendingChannels.end(), channel);
    if (channelIt == m_pendingChannels.end()) {
        m_pendingChannels.append(channel);
    } else {
        *channelIt = channel;
    }

    m_timer->start();
}

void JokerSettingsStorage::processPendings()
{
    auto db = QSqlDatabase::database(m_connection);

    if (!db.transaction()) {
        qCCritical(jkSettingsStorage) << "Unable to start transaction:" << db.lastError();
        return;
    }

    // Append programs
    while (m_pendingPrograms.count() > 0) {
        const auto program = m_pendingPrograms.takeFirst();

        QSqlQuery query(db);
        const QString insertQuery = QLatin1String("insert into ")
                + QLatin1String(kProgramsTable)
                + QLatin1String("(channel, id, position, description, aspectratio) values (?, ?, ?, ?, ?)");

        if (query.prepare(insertQuery)) {
            query.addBindValue(program.m_channelIndex);
            query.addBindValue(program.m_id);
            query.addBindValue(program.m_position);
            query.addBindValue(program.m_description);
            query.addBindValue(program.m_aspectRatio);
            if (!query.exec()) {
                qCCritical(jkSettingsStorage) << "Unable to exec programs insert query:" << db.lastError();
            }
        } else {
            qCCritical(jkSettingsStorage) << "Unable to prepare programs insert query:" << db.lastError();
        }
    }

    // Append channels
    while (m_pendingChannels.count() > 0) {
        const auto channel = m_pendingChannels.takeFirst();

        QSqlQuery query(db);
        const QString insertQuery = QLatin1String("insert into ")
                + QLatin1String(kChannelsTable)
                + QLatin1String("(caption, source, standard, modulation, "
                                "voltage, frequency, bandwidth, symbolrate) "
                                "values (?, ?, ?, ?, ?, ?, ?, ?)");

        if (query.prepare(insertQuery)) {
            query.addBindValue(channel.m_caption);
            query.addBindValue(channel.m_source);
            query.addBindValue(channel.m_standard);
            query.addBindValue(channel.m_modulation);
            query.addBindValue(channel.m_voltage);
            query.addBindValue(channel.m_frequencyMhz);
            query.addBindValue(channel.m_bandwidth);
            query.addBindValue(channel.m_symbolRate);
            if (!query.exec()) {
                qCCritical(jkSettingsStorage) << "Unable to exec channels insert query:" << db.lastError();
            }
        } else {
            qCCritical(jkSettingsStorage) << "Unable to prepare channels insert query:" << db.lastError();
        }
    }

    if (!db.commit()) {
        qCCritical(jkSettingsStorage) << "Unable to finish transaction:" << db.lastError();
        return;
    }
}
