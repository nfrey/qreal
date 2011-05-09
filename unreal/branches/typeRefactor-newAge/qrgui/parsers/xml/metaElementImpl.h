#pragma once

#include <QtCore/QString>
#include <QtXml/QDomElement>
#include "../../../qrgui/pluginInterface/editorInterface.h\"
#include "../../../qrgui/pluginInterface/elementRepoInterface.h"
#include "../../../qrgui/pluginInterface/elementTitleHelpers.h"

using namespace UML;
namespace parsers {

class MetaElementImpl: public UML::ElementImpl
{
public:
	MetaElementImpl(QDomElement &graphicElement,
					bool isNode,
					bool isContainer,
					bool isSortingContainer,
					int sizeOfForestalling,
					int sizeOfChildrenForestalling,
					bool hasMovableChildren,
					bool minimizesToChildren,
					bool maximizesChildren,
					bool isPin,
					bool isHavePin,
					bool isbonusContextMenuFieldsEmpty,
					QString startArrowStyle,
					QString endArrowStyle);
	void init(ElementTitleFactoryInterface &, QList<ElementTitleInterface*> &) {}

	void init(QRectF &contents, QList<StatPoint> &pointPorts,
			  QList<StatLine> &linePorts, ElementTitleFactoryInterface &factory,
			  QList<ElementTitleInterface*> &titles, SdfRendererInterface *renderer,
			  SdfRendererInterface *portRenderer);
	void paint(QPainter *painter, QRectF &contents);
	Qt::PenStyle getPenStyle();
	void drawStartArrow(QPainter *) const;
	void drawEndArrow(QPainter *) const;
	bool hasPorts();
	void updateData(ElementRepoInterface *repo) const;
	bool isNode();
	bool isResizeable();
	bool isContainer();
	bool isSortingContainer();
	int sizeOfForestalling();
	int sizeOfChildrenForestalling();
	bool hasMovableChildren();
	bool minimizesToChildren();
	bool maximizesChildren();
	bool isPort();
	bool hasPin();
	QList<double> border();
	QStringList bonusContextMenuFields();

private:
	void generateEdgeStyle(QString const &styleString, QPainter *painter);

	bool mIsNode;
	bool mIsContainer;
	bool mIsSortingContainer;
	int mSizeOfForestalling;
	int mSizeOfChildrenForestalling;
	bool mHasMovableChildren;
	bool mMinimizesToChildren;
	bool mMaximizesChildren;
	bool mIsPin;
	bool mIsHavePin;
	bool mIsBonusContextMenuFieldsEmpty;
	QString mStartArrowStyle;
	QString mEndArrowStyle;

	SdfRendererInterface *mRenderer;
	QDomElement *mElement;
	QDomElement *mPictureElement;
	QDomElement *mPortsElement;
	QDomElement *mLabelElement;
	QDomElement *mLineStyleElement;
	QList<ElementTitleInterface> mTitles;
	QString mTextBinded;
	QString mCenter;
};
}
