#include "jokerprogram.h"

#include <QLoggingCategory>

JokerProgram::JokerProgram(int channelIndex, int id)
    : m_channelIndex(channelIndex)
    , m_id(id)
    , m_description(tr("Unknown"))
{
}

bool JokerProgram::operator==(const JokerProgram &other) const
{
    return m_channelIndex == other.m_channelIndex
            && m_id == other.m_id
            ;
}

bool JokerProgram::operator!=(const JokerProgram &other) const
{
    return !(*this == other);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const JokerProgram &program)
{
    debug << "index:" << program.m_channelIndex
          << "id:" << program.m_id
          << "pos:" << program.m_position
          << "name:" << program.m_description;
    return debug;
}
#endif // QT_NO_DEBUG_STREAM
