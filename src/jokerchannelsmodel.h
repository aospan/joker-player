#ifndef JOKERCHANNELSMODEL_H
#define JOKERCHANNELSMODEL_H

#include "jokerchannel.h"

#include <QPointer>
#include <QSortFilterProxyModel>
#include <QVector>

class JokerSettingsStorage;

// JokerChannelsModel

class JokerChannelsModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(JokerSettingsStorage *settingsStorage READ settingsStorage
               WRITE setSettingsStorage NOTIFY settingsStorageChanged)

public:
    explicit JokerChannelsModel(QObject *parent = nullptr);

    void setSettingsStorage(JokerSettingsStorage *settingsStorage);
    JokerSettingsStorage *settingsStorage() const;

signals:
    void settingsStorageChanged(JokerSettingsStorage *settingsStorage);

private:
    int rowCount(const QModelIndex &parent) const final;
    QVariant data(const QModelIndex &index, int role) const final;
    bool setData(const QModelIndex &index, const QVariant &value, int role) final;
    Qt::ItemFlags flags(const QModelIndex &index) const final;
    QHash<int, QByteArray> roleNames() const final;

    QPointer<JokerSettingsStorage> m_settingsStorage; // not owned!

    friend class JokerChannelsProxyModel;
};

// JokerChannelsProxyModel

class JokerChannelsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(JokerTelevision::Source signalSource READ signalSource
               WRITE setSignalSource NOTIFY signalSourceChanged)

    Q_PROPERTY(JokerTelevision::Standard standardCode READ standardCode
               WRITE setStandardCode NOTIFY standardCodeChanged)

    Q_PROPERTY(QString channelName READ channelName
               WRITE setChannelName NOTIFY channelNameChanged)

public:
    explicit JokerChannelsProxyModel(QObject *parent = nullptr);

    void setSignalSource(JokerTelevision::Source signalSource);
    JokerTelevision::Source signalSource() const;

    void setStandardCode(JokerTelevision::Standard standardCode);
    JokerTelevision::Standard standardCode() const;

    void setChannelName(const QString &channelName);
    QString channelName() const;

    Q_INVOKABLE void selectAllChannels();
    Q_INVOKABLE void unselectAllChannels();

signals:
    void signalSourceChanged(JokerTelevision::Source source);
    void standardCodeChanged(JokerTelevision::Standard standardCode);
    void channelNameChanged(const QString &channelName);

private:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const final;

    JokerTelevision::Source m_signalSource = JokerTelevision::UnknownSource;
    JokerTelevision::Standard m_standardCode = JokerTelevision::UnknownStandard;
    QString m_channelName;
};

#endif // JOKERCHANNELSMODEL_H
