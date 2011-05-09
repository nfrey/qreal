#include "metaElementImpl.h"

#include <QtCore/QDebug>
#include <QtXml/QDomDocument>
#include <QtCore/QProcess>
#include "scalableItem.h"
#include "elementTitleHelpers.h"

using namespace qReal;
using namespace parsers;
using namespace UML;

MetaElementImpl::MetaElementImpl(QDomElement &element, bool isNode, bool isContainer,
								 bool isSortingContainer, int sizeOfForestalling,
								 int sizeOfChildrenForestalling, bool hasMovableChildren,
								 bool minimizesToChildren, bool maximizesChildren,
								 bool isPin, bool isHavePin, bool isbonusContextMenuFieldsEmpty,
								 QString startArrowStyle, QString endArrowStyle):
	mElement(element), mIsNode(isNode), mIsContainer(isContainer),
	mIsSortingContainer(isSortingContainer),
	mSizeOfForestalling(sizeOfForestalling),
	mSizeOfChildrenForestalling(sizeOfChildrenForestalling),
	mHasMovableChildren(hasMovableChildren),
	mMinimizesToChildren(minimizesToChildren),
	mMaximizesChildren(maximizesChildren),
	mIsPin(isPin),
	mIsHavePin(isHavePin),
	mIsBonusContextMenuFieldsEmpty(isbonusContextMenuFieldsEmpty),
	mStartArrowStyle(startArrowStyle),
	mEndArrowStyle(endArrowStyle)
{
}

void MetaElementImpl::init(QRectF &contents, QList<StatPoint> &pointPorts,
						   QList<StatLine> &linePorts, ElementTitleFactoryInterface &factory,
						   QList<ElementTitleInterface*> &titles, SdfRendererInterface *renderer,
						   SdfRendererInterface *portRenderer)
{
	Q_UNUSED(pointPorts);
	Q_UNUSED(linePorts);

	mPictureElement = mElement->firstChildElement("picture");
	mPortsElement = mElement->firstChildElement("ports");
	mLabelElement = mElement->firstChildElement("labels");
	mLineStyleElement = mElement->firstChildElement("lineType");

	int pictureWidth = mPictureElement.attribute("sizex").toInt();
	int pictureHeight = mPictureElement.attribute("sizey").toInt();
	mRenderer = renderer;
	mRenderer->load(mPictureElement);
	portRenderer->load(mPortsElement);
	contents.setWidth(pictureWidth);
	contents.setHeight(pictureHeight);
	foreach(QDomElement label, mLabelElement.childNodes())
	{
		ElementTitleInterface title;
		mTextBinded = label.attribute("textBinded");
		mCenter = label.attribute("center", "false");
		ScalableCoordinate x = ScalableItem.initCoordinate(label.attribute("x"), pictureWidth);
		ScalableCoordinate y = ScalableItem.initCoordinate(label.attribute("y"), pictureHeight);
		QString text = label.attribute("text");
		if (text.isEmpty())
		{
			title = factory.createTitle(
						x,
						y,
						mTextBinded,
						label.attribute("readOnly", false));
		}
		else
		{
			title = factory.createTitle(
						x,
						y,
						text);
		}

		title.setBackground("Qt::" + label.attribute("background",
													 isNode ? "transparent" : "white"););
		title.setScaling(x.isScalable(), y.isScalable());
		title->setFlags(0);
		title->setTextInteractionFlags(Qt::NoTextInteraction);
		titles.append(title);
		mTitles.append(title);
	}
}

void MetaElementImpl::updateData(ElementRepoInterface *repo) const
{
	foreach (ElementTitleInterface title, mTitles)
	{
		QString field;
		if (mTextBinded == "name")
			field = repo->name();
		else
			field = (repo->logicalProperty(mTextBinded));

		if (mCenter == "true")
			title.setHtml(QString("<center>%1</center>").arg(field);
					else
					title.setHtml(field);
	}
}

void MetaElementImpl::paint(QPainter *painter, QRectF &contents)
{
	mRenderer->render(painter, contents);
}

Qt::PenStyle MetaElementImpl::getPenStyle()
{
	if (mIsNode)
		return Qt::SolidLine;
	else
	{
		if (!mLineStyleElement->isNull())
		{
			QString lineType = mLineStyleElement.attribute("type");
			if (lineType.isEmpty())
				qDebug() << "ERROR: no line type";
			else if (lineType == "noPan")
				return Qt::SolidLine;
			switch (lineType.toLower())
			case "nopan":
			case "solidline":
				return Qt::SolidLine;
			case "dashline":
				return Qt::DashLine;
			case "dotline":
				return Qt::DotLine;
			case "dashdotline":
				return Qt::DashDotLine;
			case "dashdotdotline":
				return Qt::DashDotDotLine;
		}
		return Qt::SolidLine;
	}
}

bool MetaElementImpl::isNode()
{
	return mIsNode;
}

bool MetaElementImpl::isResizeable()
{
	return mIsNode && mIsContainer;
}

bool MetaElementImpl::isSortingContainer()
{
	return mIsNode && mIsSortingContainer;
}

int MetaElementImpl::sizeOfForestalling()
{
	return mIsNode ? 0 : mSizeOfForestalling;
}

int MetaElementImpl::sizeOfChildrenForestalling()
{
	return mIsNode ? 0 : mSizeOfChildrenForestalling;
}

bool MetaElementImpl::hasMovableChildren()
{
	return mIsNode && mHasMovableChildren;
}

bool MetaElementImpl::minimizesToChildren()
{
	return mIsNode && mMinimizesToChildren;
}

bool MetaElementImpl::maximizesChildren()
{
	return mIsNode && mMaximizesChildren;
}

bool MetaElementImpl::isPort()
{
	return mIsNode && mIsPin;
}

bool MetaElementImpl::hasPin()
{
	return mIsNode && mIsHavePin;
}

QList<double> MetaElementImpl::border()
{
	QList<double> res;
	if (mIsNode && mIsHavePin)
		res << 30 << 15 << 15 << 25;
	else
		res << 0 << 0 << 0 << 0;
	return res;
}

bool MetaElementImpl::bonusContextMenuFields()
{
	mIsBonusContextMenuFieldsEmpty;
}

bool MetaElementImpl::hasPorts()
{
	return mIsNode && !mPortsElement->isNull();
}

void MetaElementImpl::drawStartArrow(QPainter *painter)
{
	if (!mIsNode)
		generateEdgeStyle(mStartArrowStyle, painter);
}

void MetaElementImpl::drawEndArrow(QPainter *painter)
{
	if (!mIsNode)
		generateEdgeStyle(mEndArrowStyle, painter);
}

void MetaElementImpl::generateEdgeStyle(const QString &styleString, QPainter *painter)
{
	QString style = styleString;
	if (styleString.isEmpty())
		style = "filled_arrow";
	QBrush old = painter->brush();
	QBrush brush;
	brush.setStyle(Qt::SolidPattern);
	switch (style)
	{
	case "":
	case "filled_rhomb":
	case "filled_arrow":
		brush.setColor(Qt::black);
		break;
	case "empty_arrow":
	case "empty_rhomb":
	case "complex_arrow":
		brush.setColor(Qt::white);
		break;
	}
	painter->setBrush(brush);

	switch (style)
	{
	case "empty_arrow":
	case "filled_arrow":
		static const QPointF points1[] = {QPointF(0,0),QPointF(-5,10),QPointF(5,10)};
		painter->drawPolygon(points1, 3);
		break;
	case "empty_rhomb":
	case "filled_rhomb":
		static const QPointF points2[] = {QPointF(0,0),QPointF(-5,10),QPointF(0,20),QPointF(5,10)};
		painter->drawPolygon(points2, 4);
		break;
	case "open_arrow":
		static const QPointF points3[] = {QPointF(-5,10),QPointF(0,0),QPointF(5,10)};
		painter->drawPolyline(points3, 3);
	case "complex_arrow":
		static const QPointF points4[] =
		{
			QPointF(-15,30),QPointF(-10,10),QPointF(0,0),QPointF(10,10),
			QPointF(15,30),QPointF(0,23),QPointF(-15,30)
		};
		painter->drawPolyline(points4, 7);
	}
	painter->setBrush(old);
}
