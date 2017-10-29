#include "jokertelevision.h"

#include <QLoggingCategory>

QString JokerTelevision::getStandardName(Standard standard)
{
    switch (standard) {
    case DvbcAnnexAStandard:
        return tr("DVBC-ANNEX-A");
    case DvbcAnnexBStandard:
        return tr("DVBC-ANNEX-B");
    case DvbtStandard:
        return tr("DVB-T");
    case DssStandard:
        return tr("DSS");
    case DvbsStandard:
        return tr("DVBS");
    case DvbssgStandard:
        return tr("DVBS2");
    case DvbhStandard:
        return tr("DVBH");
    case IdsbtStandard:
        return tr("ISDB-T");
    case IdsbsStandard:
        return tr("ISDB-S");
    case IdsbcStandard:
        return tr("ISDB-C");
    case AtscStandard:
        return tr("ATSC");
    case AtscmhStandard:
        return tr("ATSCMH");
    case DtmbStandard:
        return tr("DTMB");
    case CmmbStandard:
        return tr("CMMB");
    case DabStandard:
        return tr("DAB");
    case DvbsgtStandard:
        return tr("DVB-T2");
    case TurboStandard:
        return tr("TURBO");
    case DvbcAnnexCStandard:
        return tr("DVBC-ANNEX-C");
    default:
        return tr("Unknown");
    }
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, JokerTelevision::Source source)
{
    switch (source) {
    case JokerTelevision::UnknownSource:
        debug << "Source::Invalid";
        break;
    case JokerTelevision::TerrestrialSource:
        debug << "Source::Terrestrial";
        break;
    case JokerTelevision::SatelliteSource:
        debug << "Source::Satellite";
        break;
    default:
        debug << "Source::Unknown";
        break;
    }
    return debug;
}

QDebug operator<<(QDebug debug, JokerTelevision::Standard standard)
{
    switch (standard) {
    case JokerTelevision::DvbcAnnexAStandard:
        debug << "Standard::DVBC-ANNEX-A";
        break;
    case JokerTelevision::DvbcAnnexBStandard:
        debug << "Standard::DVBC-ANNEX-B";
        break;
    case JokerTelevision::DvbtStandard:
        debug << "Standard::DVB-T";
        break;
    case JokerTelevision::DssStandard:
        debug << "Standard::DSS";
        break;
    case JokerTelevision::DvbsStandard:
        debug << "Standard::DVBS";
        break;
    case JokerTelevision::DvbssgStandard:
        debug << "Standard::DVBS2";
        break;
    case JokerTelevision::DvbhStandard:
        debug << "Standard::DVBH";
        break;
    case JokerTelevision::IdsbtStandard:
        debug << "Standard::ISDB-T";
        break;
    case JokerTelevision::IdsbsStandard:
        debug << "Standard::ISDB-S";
        break;
    case JokerTelevision::IdsbcStandard:
        debug << "Standard::ISDB-C";
        break;
    case JokerTelevision::AtscStandard:
        debug << "Standard::ATSC";
        break;
    case JokerTelevision::AtscmhStandard:
        debug << "Standard::ATSCMH";
        break;
    case JokerTelevision::DtmbStandard:
        debug << "Standard::DTMB";
        break;
    case JokerTelevision::CmmbStandard:
        debug << "Standard::CMMB";
        break;
    case JokerTelevision::DabStandard:
        debug << "Standard::DAB";
        break;
    case JokerTelevision::DvbsgtStandard:
        debug << "Standard::DVB-T2";
        break;
    case JokerTelevision::TurboStandard:
        debug << "Standard::TURBO";
        break;
    case JokerTelevision::DvbcAnnexCStandard:
        debug << "Standard::DVBC-ANNEX-C";
        break;
    default:
        debug << "Standard::Unknown";
        break;
    }
    return debug;
}

QDebug operator<<(QDebug debug, JokerTelevision::Modulation modulation)
{
    switch (modulation) {
    case JokerTelevision::QpskModulation:
        debug << "Modulation::QPSK";
        break;
    case JokerTelevision::Qam16Modulation:
        debug << "Modulation::QAM16";
        break;
    case JokerTelevision::Qam32Modulation:
        debug << "Modulation::QAM32";
        break;
    case JokerTelevision::Qam64Modulation:
        debug << "Modulation::QAM64";
        break;
    case JokerTelevision::Qam128Modulation:
        debug << "Modulation::QAM128";
        break;
    case JokerTelevision::Qam256Modulation:
        debug << "Modulation::QAM256";
        break;
    case JokerTelevision::QamAutoModulation:
        debug << "Modulation::QAMAUTO";
        break;
    case JokerTelevision::Vsb8Modulation:
        debug << "Modulation::VSB8";
        break;
    case JokerTelevision::Vsb16Modulation:
        debug << "Modulation::VSB16";
        break;
    case JokerTelevision::Psk8Modulation:
        debug << "Modulation::PSK8";
        break;
    case JokerTelevision::Apsk16Modulation:
        debug << "Modulation::APSK16";
        break;
    case JokerTelevision::Apsk32Modulation:
        debug << "Modulation::APSK32";
        break;
    case JokerTelevision::DqpskModulation:
        debug << "Modulation::DQPSK";
        break;
    case JokerTelevision::Qam4NrModulation:
        debug << "Modulation::QAM4NR";
        break;
    default:
        debug << "Modulation::Unknown";
        break;
    }
    return debug;
}

QDebug operator<<(QDebug debug, JokerTelevision::Voltage voltage)
{
    switch (voltage) {
    case JokerTelevision::Voltage13:
        debug << "Voltage::13";
        break;
    case JokerTelevision::Voltage18:
        debug << "Voltage::18";
        break;
    case JokerTelevision::VoltageOff:
        debug << "Voltage::Off";
        break;
    default:
        debug << "Voltage::Unknown";
        break;
    }
    return debug;
}

QDebug operator<<(QDebug debug, JokerTelevision::Tone tone)
{
    switch (tone) {
    case JokerTelevision::ToneOn:
        debug << "Tone::On";
        break;
    case JokerTelevision::ToneOff:
        debug << "Tone::Off";
        break;
    default:
        debug << "Tone::Unknown";
        break;
    }
    return debug;
}
#endif // QT_NO_DEBUG_STREAM
