#pragma once

#include <QtCore/QString>
#include "scalableCoordinate.h"

class ScalableItem
{
public:
	static ScalableCoordinate initCoordinate(QString const &coordinate, int maxValue);
	virtual ~ScalableItem() {};
protected:	
};
