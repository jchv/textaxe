#pragma once

#include <QByteArray>
#include <QString>

class CodingManager
{
public:
    CodingManager();
    ~CodingManager();

    QLatin1String guess(QByteArray text);
};
