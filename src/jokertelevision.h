#ifndef JOKERTELEVISION_H
#define JOKERTELEVISION_H

#include <QObject>

class JokerTelevision : public QObject
{
    Q_OBJECT

public:
    enum Source {
        UnknownSource,
        TerrestrialSource,
        SatelliteSource
    };
    Q_ENUM(Source)

    // Copied from jokertv headers, do not change order!
    enum Standard {
        UnknownStandard, // None
        DvbcAnnexAStandard, // DVBC-ANNEX-A
        DvbcAnnexBStandard, // DVBC-ANNEX-B
        DvbtStandard, // DVB-T
        DssStandard, // DSS
        DvbsStandard, // DVBS
        DvbssgStandard, // DVBS2
        DvbhStandard, // DVBH
        IdsbtStandard, // ISDB-T
        IdsbsStandard, // ISDB-S
        IdsbcStandard, // ISDB-C
        AtscStandard, // ATSC
        AtscmhStandard, // ATSCMH
        DtmbStandard, // DTMB
        CmmbStandard, // CMMB
        DabStandard, // DAB
        DvbsgtStandard, // DVB-T2
        TurboStandard, // TURBO
        DvbcAnnexCStandard // DVBC-ANNEX-C
    };
    Q_ENUM(Standard)

    // Copied from jokertv headers, do not change order!
    enum Modulation {
        QpskModulation,
        Qam16Modulation,
        Qam32Modulation,
        Qam64Modulation,
        Qam128Modulation,
        Qam256Modulation,
        QamAutoModulation,
        Vsb8Modulation,
        Vsb16Modulation,
        Psk8Modulation,
        Apsk16Modulation,
        Apsk32Modulation,
        DqpskModulation,
        Qam4NrModulation,
    };
    Q_ENUM(Modulation)

    // Copied from jokertv headers, do not change order!
    enum Voltage {
        Voltage13,
        Voltage18,
        VoltageOff
    };
    Q_ENUM(Voltage)

    // Copied from jokertv headers, do not change order!
    enum Tone {
        ToneOn,
        ToneOff
    };
    Q_ENUM(Tone)

    static QString getStandardName(Standard standard);
};

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, JokerTelevision::Source source);
QDebug operator<<(QDebug debug, JokerTelevision::Standard standard);
QDebug operator<<(QDebug debug, JokerTelevision::Modulation modulation);
QDebug operator<<(QDebug debug, JokerTelevision::Voltage voltage);
QDebug operator<<(QDebug debug, JokerTelevision::Tone tone);
#endif // QT_NO_DEBUG_STREAM

#endif // JOKERTELEVISION_H
