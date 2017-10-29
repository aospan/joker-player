#ifndef JOKERCHANNEL_H
#define JOKERCHANNEL_H

#include "jokertelevision.h"

class JokerChannel
{
public:
    explicit JokerChannel() = default;

    bool operator==(const JokerChannel &other) const;
    bool operator!=(const JokerChannel &other) const;

    QString m_caption;
    JokerTelevision::Source m_source = JokerTelevision::UnknownSource;
    JokerTelevision::Standard m_standard = JokerTelevision::UnknownStandard;
    JokerTelevision::Modulation m_modulation = JokerTelevision::QpskModulation;
    JokerTelevision::Voltage m_voltage = JokerTelevision::VoltageOff;
    int m_frequencyMhz = 0; // MHz
    int m_bandwidth = 0;
    int m_symbolRate = 0;
    bool m_selected = false;
};

Q_DECLARE_TYPEINFO(JokerChannel, Q_MOVABLE_TYPE);

#endif // JOKERCHANNEL_H
