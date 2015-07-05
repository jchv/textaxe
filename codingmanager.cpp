#include "codingmanager.h"
#include "libcharguess/charguess.h"

CodingManager::CodingManager()
{
    CharGuessInit();
}

CodingManager::~CodingManager()
{
    CharGuessDone();
}

QLatin1String CodingManager::guess(QByteArray text)
{
    if (text.startsWith("\xEF\xBB\xBF"))
        return QLatin1String("UTF-8+BOM");

    if (text.startsWith("\xFF\xFE"))
        return QLatin1String("UTF-16LE");

    if (text.startsWith("\xFE\xFF"))
        return QLatin1String("UTF-16BE");

    const char *g = GuessChardet(text.data());
    if (g)
        return QLatin1String(g);
    else
        return QLatin1String("UTF-8");
}
