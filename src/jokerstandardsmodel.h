#ifndef JOKERSTANDARDSMODEL_H
#define JOKERSTANDARDSMODEL_H

#include "jokerchannel.h"

#include <QPointer>
#include <QSortFilterProxyModel>
#include <QVector>

class JokerSettingsStorage;

// JokerStandardsModel

class JokerStandardsModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(JokerSettingsStorage *settingsStorage READ settingsStorage
               WRITE setSettingsStorage NOTIFY settingsStorageChanged)

public:
    explicit JokerStandardsModel(QObject *parent = nullptr);

    void setSettingsStorage(JokerSettingsStorage *settingsStorage);
    JokerSettingsStorage *settingsStorage() const;

signals:
    void settingsStorageChanged(JokerSettingsStorage *settingsStorage);

private:
    void setupStandards();

    int rowCount(const QModelIndex &parent) const final;
    QVariant data(const QModelIndex &index, int role) const final;
    QHash<int, QByteArray> roleNames() const final;

    QVector<JokerTelevision::Standard> m_standards;
    QPointer<JokerSettingsStorage> m_settingsStorage; // not owned!

    friend class JokerStandardsProxyModel;
};

// JokerStandardsProxyModel

class JokerStandardsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(ChoosingFlag choosingFlags READ choosingFlags
               WRITE setChoosingFlags NOTIFY choosingFlagsChanged)

public:
    enum ChoosingFlag {
        UnspecifiedStandardFlag = 0x0, // None
        DvbtStandardFlag = 0x1, // DVB-T
        DvbsgtStandardFlag = 0x2, // DVB-T2
        AtscStandardFlag = 0x4, // ATSC
        IsdbtStandardFlag = 0x8, // ISDB-T
        DtmbStandardFlag = 0x10, // DTMB
        DvbcStandardFlag = 0x20 // DVB-C
    };
    Q_FLAG(ChoosingFlag)
    Q_DECLARE_FLAGS(ChoosingFlags, ChoosingFlag)

    explicit JokerStandardsProxyModel(QObject *parent = nullptr);
    ~JokerStandardsProxyModel() override;

    void setChoosingFlags(ChoosingFlags choosingFlags);
    ChoosingFlags choosingFlags() const;

signals:
    void choosingFlagsChanged(ChoosingFlags choosingFlags);

private:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const final;

    ChoosingFlags m_choosingFlags = UnspecifiedStandardFlag;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(JokerStandardsProxyModel::ChoosingFlags)

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, JokerStandardsProxyModel::ChoosingFlags choosingFlags);
#endif // QT_NO_DEBUG_STREAM

#endif // JOKERSTANDARDSMODEL_H
