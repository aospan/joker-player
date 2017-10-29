#include "jokerchannel.h"

#include <QLoggingCategory>

bool JokerChannel::operator==(
        const JokerChannel &other) const
{
    return m_caption == other.m_caption
            && m_source == other.m_source
            && m_standard == other.m_standard
            && m_modulation == other.m_modulation
            && m_voltage == other.m_voltage
            && m_frequencyMhz == other.m_frequencyMhz
            && m_bandwidth == other.m_bandwidth
            && m_symbolRate == other.m_symbolRate
            // Do not add selection membe here!
            ;
}

bool JokerChannel::operator!=(
        const JokerChannel &other) const
{
    return !(*this == other);
}
