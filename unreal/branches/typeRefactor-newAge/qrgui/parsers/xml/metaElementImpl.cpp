#include "metaElementImpl.h"

#include <QtCore/QDebug>
#include <QtXml/QDomDocument>
#include <QtCore/QProcess>
#include "../../../qrxc/scalableItem.h"
#include "../pluginInterface/elementTitleHelpers.h"

using namespace qReal;


MetaElementImpl::MetaElementImpl(QDomElement &element, bool isNode, bool isContainer,
								 bool isSortingContainer, int sizeOfForestalling,
								 int sizeOfChildrenForestalling, bool hasMovableChildren,
								 bool minimizesToChildren, bool maximizesChildren,
								 bool isPin, bool isHavePin, QStringList bonusContextMenuFields,
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
	mStartArrowStyle(startArrowStyle),
	mEndArrowStyle(endArrowStyle),
	mBonusContextMenuFields(bonusContextMenuFields)
{
}

MetaElementImpl::MetaElementImpl(MetaElementImpl const * const impl)
{
	mElement = impl->mElement;
	mIsNode = impl->mIsNode;
	mIsContainer = impl->mIsContainer;
	mIsSortingContainer = impl->mIsSortingContainer;
	mSizeOfForestalling = impl->mSizeOfForestalling;
	mSizeOfChildrenForestalling = impl->mSizeOfChildrenForestalling;
	mHasMovableChildren = impl->mHasMovableChildren;
	mMinimizesToChildren = impl->mMinimizesToChildren;
	mMaximizesChildren = impl->mMaximizesChildren;
	mIsPin = impl->mIsPin;
	mIsHavePin = impl->mIsPin;
	mStartArrowStyle = impl->mStartArrowStyle;
	mEndArrowStyle = impl->mEndArrowStyle;
	mBonusContextMenuFields = impl->mBonusContextMenuFields;
}

MetaElementImpl::MetaElementImpl(QDomElement const &element, MetaElementImpl const * const impl)
{
	mElement = element;
	mIsNode = impl->mIsNode;
	mIsContainer = impl->mIsContainer;
	mIsSortingContainer = impl->mIsSortingContainer;
	mSizeOfForestalling = impl->mSizeOfForestalling;
	mSizeOfChildrenForestalling = impl->mSizeOfChildrenForestalling;
	mHasMovableChildren = impl->mHasMovableChildren;
	mMinimizesToChildren = impl->mMinimizesToChildren;
	mMaximizesChildren = impl->mMaximizesChildren;
	mIsPin = impl->mIsPin;
	mIsHavePin = impl->mIsPin;
	mStartArrowStyle = impl->mStartArrowStyle;
	mEndArrowStyle = impl->mEndArrowStyle;
	mBonusContextMenuFields = impl->mBonusContextMenuFields;
}

void MetaElementImpl::init(QRectF &contents, QList<StatPoint> &pointPorts,
						   QList<StatLine> &linePorts, ElementTitleFactoryInterface &factory,
						   QList<ElementTitleInterface*> &titles, SdfRendererInterface *renderer,
						   SdfRendererInterface *portRenderer)
{
	Q_UNUSED(pointPorts);
	Q_UNUSED(linePorts);

	mPictureElement = mElement.firstChildElement("picture");
	mPortsElement = mElement.firstChildElement("ports");
	mLabelElement = mElement.firstChildElement("labels");
	mLineStyleElement = mElement.firstChildElement("lineType");

	mPictureWidth = mPictureElement.attribute("sizex").toInt();
	mPictureHeight = mPictureElement.attribute("sizey").toInt();
	mRenderer = renderer;
	mRenderer->load(mPictureElement);
	portRenderer->load(mPortsElement);
	contents.setWidth(mPictureWidth);
	contents.setHeight(mPictureHeight);

	fillTitles(mLabelElement, titles, factory);
	fillPointPorts(mPortsElement, pointPorts);
	fillLinePorts(mPortsElement, linePorts);
}

void MetaElementImpl::fillTitles(QDomElement &labels, QList<ElementTitleInterface*> &titles,
								 ElementTitleFactoryInterface &factory)
{
	for (QDomElement label = labels.firstChildElement();
		 !label.isNull();
		 label = label.nextSiblingElement())
	{
		ElementTitleInterface *title;
		mTextBinded = label.attribute("textBinded");
		mCenter = label.attribute("center", "false");
		ScalableItem item;
		ScalableCoordinate x = item.initCoordinate(label.attribute("x"), mPictureWidth);
		ScalableCoordinate y = item.initCoordinate(label.attribute("y"), mPictureHeight);
		QString text = label.attribute("text");
		if (text.isEmpty())
		{
			title = factory.createTitle(
						x.value(),
						y.value(),
						mTextBinded,
						label.attribute("readOnly", "false") == "false" ? false : true);
		}
		else
		{
			title = factory.createTitle(
						x.value(),
						y.value(),
						text);
		}

		if (mIsNode)
			title->setBackground(Qt::transparent);
		else
			title->setBackground(Qt::white);
		title->setScaling(x.isScalable(), y.isScalable());
		title->setFlags(0);
		title->setTextInteractionFlags(Qt::NoTextInteraction);
		titles.append(title);
		mTitles.append(title);
	}
}

void MetaElementImpl::fillLinePorts(QDomElement &ports, QList<StatLine> &linePorts)
{
	for (QDomElement linePort = ports.firstChildElement("linePort");
		 !linePort.isNull();
		 linePort = linePort.nextSiblingElement())
	{
		QDomElement portStartElement = linePort.firstChildElement("start");
		QDomElement portEndElement = linePort.firstChildElement("end");

		ScalableItem item;
		ScalableCoordinate startx = item.initCoordinate(portStartElement.attribute("startx"), mPictureWidth);
		ScalableCoordinate starty = item.initCoordinate(portStartElement.attribute("starty"), mPictureHeight);
		ScalableCoordinate endx = item.initCoordinate(portEndElement.attribute("endx"), mPictureWidth);
		ScalableCoordinate endy = item.initCoordinate(portEndElement.attribute("endy"), mPictureHeight);

		StatLine ln;
		ln.line = QLineF(startx.value(), starty.value(), endx.value(), endy.value());
		ln.prop_x1 = startx.isScalable();
		ln.prop_y1 = starty.isScalable();
		ln.prop_x2 = endx.isScalable();
		ln.prop_y2 = endy.isScalable();
		ln.initWidth = mPictureWidth;
		ln.initHeight = mPictureHeight;
		linePorts << ln;
	}
}

void MetaElementImpl::fillPointPorts(QDomElement &ports, QList<StatPoint> &pointPorts)
{
	for (QDomElement pointPort = ports.firstChildElement("pointPort");
		 !pointPort.isNull();
		 pointPort = pointPort.nextSiblingElement())
	{
		ScalableItem item;
		ScalableCoordinate x = item.initCoordinate(pointPort.attribute("x"), mPictureWidth);
		ScalableCoordinate y = item.initCoordinate(pointPort.attribute("y"), mPictureHeight);

		StatPoint pt;
		pt.point = QPointF(x.value(), y.value());
		pt.prop_x = x.isScalable();
		pt.prop_y = y.isScalable();
		pt.initWidth = mPictureWidth;
		pt.initHeight = mPictureHeight;
		pointPorts << pt;
	}
}

void MetaElementImpl::updateData(ElementRepoInterface *repo) const
{
	foreach (ElementTitleInterface* title, mTitles)
	{
		QString field;
		if (mTextBinded == "name")
			field = repo->name();
		else
			field = (repo->logicalProperty(mTextBinded));

		if (mCenter == "true")
			title->setHtml(QString("<center>%1</center>").arg(field));
		else
			title->setHtml(field);
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
		if (!mLineStyleElement.isNull())
		{
			QString lineType = mLineStyleElement.attribute("type");
			if (lineType.isEmpty())
				qDebug() << "ERROR: no line type";
			else if (lineType == "noPan")
				return Qt::SolidLine;
			QString lowerType = lineType.toLower();
			if (lowerType == "nopan" || lowerType == "solidline")
				return Qt::SolidLine;
			else if (lowerType == "dashline")
				return Qt::DashLine;
			else if (lowerType == "dotline")
				return Qt::DotLine;
			else if (lowerType == "dashdotline")
				return Qt::DashDotLine;
			else if (lowerType == "dashdotdotline")
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

bool MetaElementImpl::isContainer()
{
	return mIsNode && mIsContainer;
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

QStringList MetaElementImpl::bonusContextMenuFields()
{
	return mBonusContextMenuFields;
}

bool MetaElementImpl::hasPorts()
{
	return mIsNode && !mPortsElement.isNull();
}

void MetaElementImpl::drawStartArrow(QPainter *painter) const
{
	if (!mIsNode)
		generateEdgeStyle(mStartArrowStyle, painter);
}

void MetaElementImpl::drawEndArrow(QPainter *painter) const
{
	if (!mIsNode)
		generateEdgeStyle(mEndArrowStyle, painter);
}

void MetaElementImpl::generateEdgeStyle(QString styleString, QPainter *painter) const
{
	QString style = styleString;
	if (styleString.isEmpty())
		style = "filled_arrow";
	QBrush old = painter->brush();
	QBrush brush;
	brush.setStyle(Qt::SolidPattern);
	if (style == "" || style == "filled_rhomb" || style == "filled_arrow")
		brush.setColor(Qt::black);
	else
		brush.setColor(Qt::white);

	painter->setBrush(brush);

	if (style == "empty_arrow" || style == "filled_arrow")
	{
		static const QPointF points1[] = {QPointF(0,0),QPointF(-5,10),QPointF(5,10)};
		painter->drawPolygon(points1, 3);
	}
	else if (style == "empty_rhomb" || style == "filled_rhomb")
	{
		static const QPointF points2[] = {QPointF(0,0),QPointF(-5,10),QPointF(0,20),QPointF(5,10)};
		painter->drawPolygon(points2, 4);
	}
	else if (style == "open_arrow")
	{
		static const QPointF points3[] = {QPointF(-5,10),QPointF(0,0),QPointF(5,10)};
		painter->drawPolyline(points3, 3);
	}
	else if (style == "complex_arrow")
	{
		static const QPointF points4[] =
		{
			QPointF(-15,30),QPointF(-10,10),QPointF(0,0),QPointF(10,10),
			QPointF(15,30),QPointF(0,23),QPointF(-15,30)
		};
		painter->drawPolyline(points4, 7);
	}
	painter->setBrush(old);
}
