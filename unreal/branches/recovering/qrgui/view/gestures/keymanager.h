#pragma once
#include "IKeyManager.h"
#include <QPointF>
#include <QList>

class KeyManager : public IKeyManager
{
public:
    KeyManager();
    QString getKey(QList<QPoint> const & path);

private:
    void analysePoints(QList<QPoint> const & path);
    void clear();
    int mLeftBound;
    int mRightBound;
    int mUpperBound;
    int mLowerBound;
    static const int inMovement = 20;
    static const int iSize = 7;
    static const int unidimensionalLimit = 4;
};
