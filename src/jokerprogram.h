#ifndef JOKERPROGRAM_H
#define JOKERPROGRAM_H

#include "jokertelevision.h"

#include <QCoreApplication> // for Q_DECLARE_TR_FUNCTIONS

class JokerProgram
{
    Q_DECLARE_TR_FUNCTIONS(JokerProgram)
    Q_GADGET

    Q_PROPERTY(int id MEMBER m_id)
    Q_PROPERTY(QString description MEMBER m_description)

public:
    explicit JokerProgram(int channelIndex = -1, int id = -1);

    bool operator==(const JokerProgram &other) const;
    bool operator!=(const JokerProgram &other) const;

    int m_channelIndex = -1;
    int m_id = -1;
    int m_position = 0;
    QString m_description;
};

Q_DECLARE_TYPEINFO(JokerProgram, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(JokerProgram)

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const JokerProgram &program);
#endif // QT_NO_DEBUG_STREAM

#endif // JOKERPROGRAM_H
