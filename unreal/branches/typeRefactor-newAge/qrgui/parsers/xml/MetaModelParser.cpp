#include "MetaModelParser.h"

#include <QtCore/QDebug>
#include <QtCore/QUuid>
#include <QtXml/QDomDocument>
#include <QtCore/QProcess>

#include "../../../qrxc/nameNormalizer.h"
#include "../../../utils/xmlUtils.h"

using namespace qReal;
using namespace parsers;

MetaModelParser::MetaModelParser()
	: mIsNode(false), mIsContainer(false), mIsSortingContainer(false),
	  mSizeOfForestalling(0), mSizeOfChildrenForestalling(0),
	  mHasMovableChildren(true), mMinimizesToChildren(false),
	  mMaximizesChildren(false), mIsPin(false), mIsHavePin(false)
{
}

void MetaModelParser::parseFile(const QString &fileName)
{
	mDocument = utils::xmlUtils::loadDocument(fileName);

	fillDiagramNameMap();
}

void MetaModelParser::fillDiagramNameMap()
{
	QDomNodeList const diagrams = mDocument.elementsByTagName("diagram");
	for (unsigned i = 0; i < diagrams.length(); i++)
	{
		QDomElement diagram = diagrams.at(i).toElement();
		mDiagramNameMap.insert(NameNormalizer::normalize(diagram.attribute("name")), diagram.attribute("displayedName"));
		fillDiagramAttributes(diagram);
	}
}

void MetaModelParser::fillDiagramAttributes(QDomElement & diagram)
{
	QDomNodeList diagramAttributes = diagram.childNodes();
	QString diagramNormalizedName = NameNormalizer::normalize(diagram.attribute("name"));
	for (unsigned i = 0; i < diagramAttributes.length(); ++i)
	{
		QDomElement type = diagramAttributes.at(i).toElement();
		if (type.tagName() == "nonGraphicTypes")
			fillEnums(type);
		if (type.tagName() == "graphicTypes")
			fillGraphicElementType(type, diagramNormalizedName);
	}
	mGraphicalObjects.insert(diagramNormalizedName, mGraphicObjectsForElements);
	mIconMap.insert(diagramNormalizedName, mIconMapForElements);
	mPropertiesDescriptionMap.insert(diagramNormalizedName, mPropertyDescriptionsForElements);
	mElementsDescriptionMap.insert(diagramNormalizedName, mElementDescriptionsForElements);
}

void MetaModelParser::fillEnums(QDomElement &nonGraphicType)
{
	for (QDomElement childElement = nonGraphicType.firstChildElement("enum");
		 !childElement.isNull();
		 childElement = childElement.nextSiblingElement())
	{
		QStringList res;
		for (QDomElement value = childElement.firstChildElement("value");
			 !value.isNull();
			 value = value.nextSiblingElement())
		{
			res.append(value.text());
		}
		mEnums.insert(childElement.attribute("name"), res);
	}
}

void MetaModelParser::fillGraphicElementType(QDomElement &graphicType, QString const &diagramName)
{
	QDomNodeList graphicElements = graphicType.childNodes();

	for (unsigned i = 0; i < graphicElements.length(); ++i)
	{
		QDomElement graphicElement = graphicElements.at(i).toElement();
		if (graphicElement.tagName() == "node")
			fillNode(graphicElement, diagramName);
		if (graphicElement.tagName() == "edge")
			fillEdge(graphicElement, diagramName);

	}
	mElementsNameMap.insert(diagramName, mElementNames);
}

void MetaModelParser::fillEdge(QDomElement &edge, QString const &diagramName)
{
	QString normalizedName = NameNormalizer::normalize(getQualifiedName(diagramName, edge.attribute("name")));
	mElementNames.insert(normalizedName, edge.attribute("displayedName"));
	mIsNodeOrEdge.insert(normalizedName, -1);
	setEdgeAttributes(edge, diagramName, normalizedName);
}

void MetaModelParser::setEdgeAttributes(const QDomElement &edge, const QString &diagramName, const QString &elementName)
{
	QDomNodeList edgeList = edge.childNodes();

	for (unsigned i = 0; i < edgeList.length(); ++i)
	{
		QDomElement tag = edgeList.at(i).toElement();
		if (tag.tagName() == "logic")
			setEdgeConfigurations(tag, diagramName, elementName);
	}
	for (unsigned i = 0; i < edgeList.length(); ++i) {
		QDomElement tag = edgeList.at(i).toElement();
		if (tag.tagName() == "graphics")
		{
			MetaElementImpl* impl = new MetaElementImpl(
						tag,
						false,
						false,
						false,
						0,
						0,
						false,
						false,
						false,
						false,
						false,
						mBonusContextMenuFields,
						mStartArrowStyle,
						mEndArrowStyle);
			mGraphicObjectsForElements.insert(elementName, impl);
			mIconMapForElements.insert(elementName, tag.firstChildElement("picture"));
			//add metaElementImpl element
		}
	}
}

void MetaModelParser::setEdgeConfigurations(const QDomElement &tag, const QString &diagramName, const QString &elementName)
{
	QDomNodeList edgeAttributes = tag.childNodes();

	for (unsigned i = 0; i < edgeAttributes.length(); ++i) {
		QDomElement attribute = edgeAttributes.at(i).toElement();
		if (attribute.tagName() == "properties")
			setProperties(attribute, diagramName, elementName);
		else if (attribute.tagName() == "associations")
			setAssociations(attribute);
		//else if (attribute.tagName() == "possibleEdges")
		//setPossibleEdges(attribute, edgeId);
		else if (attribute.tagName() == "bonusContextMenuFields")
		{
			for (QDomElement childElement = attribute.firstChildElement("field");
				 !childElement.isNull();
				 childElement = childElement.nextSiblingElement())
			{
				mBonusContextMenuFields.append(childElement.attribute("name"));
			}
		}
	}
}

void MetaModelParser::setAssociations(const QDomElement &element)
{
	QDomNodeList associations = element.childNodes();
	QDomElement association = associations.at(0).toElement();

	mStartArrowStyle = association.attribute("beginType");
	mEndArrowStyle = association.attribute("endType");
}

void MetaModelParser::setPossibleEdges(const QDomElement &element, QString const &diagramName, QString const &elementName)
{
	QDomNodeList possibleEdges = element.childNodes();

	for (unsigned i = 0; i < possibleEdges.length(); ++i) {
		QDomElement possibleEdge = possibleEdges.at(i).toElement();
		if (possibleEdge.tagName() == "possibleEdge")
		{
		}
	}
}

void MetaModelParser::fillNode(QDomElement &node, QString const &diagramName)
{
	mIsNode = true;

	QString normalizedName = NameNormalizer::normalize(getQualifiedName(diagramName, node.attribute("name")));
	mElementNames.insert(normalizedName, node.attribute("displayedName"));
	mIsNodeOrEdge.insert(normalizedName, 1);

	//fill description map
	QString description = node.attribute("description");
	if (!description.isEmpty())
	{
		mElementDescriptionsForElements.insert(normalizedName, description);
	}

	setNodeAttributes(node, diagramName, normalizedName);
}

void MetaModelParser::setNodeAttributes(const QDomElement &node, const QString &diagramName, const QString &elementName)
{
	QDomNodeList nodeList = node.childNodes();

	for (unsigned i = 0; i < nodeList.length(); ++i)
	{
		QDomElement tag = nodeList.at(i).toElement();
		if (tag.tagName() == "logic")
			setNodeConfigurations(tag, diagramName, elementName);
	}

	for (unsigned i = 0; i < nodeList.length(); ++i)
	{
		QDomElement tag = nodeList.at(i).toElement();
		if (tag.tagName() == "graphics")
		{
			MetaElementImpl* impl = new MetaElementImpl(
						tag,
						true,
						mIsContainer,
						mIsSortingContainer,
						mSizeOfForestalling,
						mSizeOfChildrenForestalling,
						mHasMovableChildren,
						mMinimizesToChildren,
						mMaximizesChildren,
						mIsPin,
						mIsHavePin,
						mBonusContextMenuFields,
						"", "");
			mGraphicObjectsForElements.insert(elementName, impl);
			mIconMapForElements.insert(elementName, tag.firstChildElement("picture"));
		}
	}
}

void MetaModelParser::setNodeConfigurations(const QDomElement &tag, const QString &diagramName, const QString &elementName)
{
	QDomNodeList nodeAttributes = tag.childNodes();

	for (unsigned i = 0; i < nodeAttributes.length(); ++i) {
		QDomElement attribute = nodeAttributes.at(i).toElement();
		QString tagName = attribute.tagName();
		if (tagName == "container")
		{
			setContainers(attribute, elementName);
			setTypedList(attribute, elementName, "contains", mElementContainedTypes);
		}
		else if (tagName == "connections")
			setTypedList(attribute, elementName, "connection", mElementConnections);
		else if (tagName == "usages")
			setTypedList(attribute, elementName, "usage", mElementUsages);
		else if (tagName == "properties")
			setProperties(attribute, diagramName, elementName);
		else if (tagName == "pin")
			mIsPin = true;
		else if (tagName == "action")
			mIsHavePin = true;
		else if (tagName == "bonusContextMenuFields")
		{
			for (QDomElement childElement = attribute.firstChildElement("field");
				 !childElement.isNull();
				 childElement = childElement.nextSiblingElement())
			{
				mBonusContextMenuFields.append(childElement.attribute("name"));
			}
		}
	}
}

void MetaModelParser::setContainers(QDomElement &container,
									const QString &elementName)
{
	if (container.childNodes().count() > 0)
		mIsContainer = true;
	else
		mIsContainer = false;

	QDomElement containerPropertiesElement = container.firstChildElement("properties");
	if (!containerPropertiesElement.isNull())
	{
		for (QDomElement childElement = containerPropertiesElement.firstChildElement();
			 !childElement.isNull();
			 childElement = childElement.nextSiblingElement())
		{
			QString tagName = childElement.tagName();
			if (tagName == "sortContainer")
				mIsSortingContainer = true;
			else if (tagName == "forestalling")
				mSizeOfForestalling = childElement.attribute("size").toInt();
			else if (tagName == "childrenForestalling")
				mSizeOfChildrenForestalling = childElement.attribute("size").toInt();
			else if (tagName == "minimizeToChildren")
				mMinimizesToChildren = true;
			else if (tagName == "banChildrenMove")
				mHasMovableChildren = false;
			else if (tagName == "maximizeChildren")
				mMaximizesChildren = true;
		}
	}
}

void MetaModelParser::setTypedList(QDomElement &node, const QString &elementName,
								   const QString &tagName,
								   QMap<QString, QStringList > map)
{
	QStringList res;
	for (QDomElement childNode= node.firstChildElement(tagName);
		 !childNode.isNull();
		 childNode = childNode.nextSiblingElement())
	{
		QString typeName = NameNormalizer::normalize(childNode.attribute("type"));
		res.append(typeName);
	}
	map.insert(elementName, res);
}

void MetaModelParser::setProperties(QDomElement &properties, const QString &diagramName,
									const QString &elementName)
{
	QMap<QString, QString> defaultValues;
	QMap<QString, QString> types;
	QMap<QString, QString> descriptions;
	for (QDomElement property= properties.firstChildElement("property");
		 !property.isNull();
		 property = property.nextSiblingElement())
	{
		QString description = property.firstChildElement("description").text();
		QString defaultValue = property.firstChildElement("default").text();
		QString name = property.attribute("name");
		QString type = property.attribute("type");
		defaultValues.insert(name, defaultValue);
		types.insert(name, NameNormalizer::normalize(type));
		descriptions.insert(name, description);
	}
	mPropertyDefault.insert(elementName, defaultValues);
	mPropertyTypes.insert(elementName, types);
	mPropertyDescriptionsForElements.insert(elementName, descriptions);
	//mPropertiesDescriptionMap.insert(diagramName,
	//new QMap<QString, QMap<QString, QString>
	//(elementName,
	// descriptions);
	//insert mPropertiesDescriptionMap
}

QString MetaModelParser::getQualifiedName(QString const &context, QString const &name)
{
	return context + "::" + name;
}

QMap<QString, QString> MetaModelParser::getDiagramNameMap()
{
	return mDiagramNameMap;
}

QMap<QString, QMap<QString, QString> > MetaModelParser::getElementNameMap()
{
	return mElementsNameMap;
}

QMap<QString, QMap<QString, QString> > MetaModelParser::getElementDescriptionMap()
{
	return mElementsDescriptionMap;
}

QMap<QString, QMap<QString, QString> > MetaModelParser::getPropertyTypes()
{
	return mPropertyTypes;
}

QMap<QString, QMap<QString, QString> > MetaModelParser::getPropertyDefaultValues()
{
	return mPropertyDefault;
}

QMap<QString, QStringList > MetaModelParser::getElementContainedTypes()
{
	return mElementContainedTypes;
}

QMap<QString, QStringList > MetaModelParser::getElementConnections()
{
	return mElementConnections;
}

QMap<QString, QStringList > MetaModelParser::getElementUsages()
{
	return mElementUsages;
}

QMap<QString, QMap<QString, QMap<QString, QString> > > MetaModelParser::getPropertyDescriptionMap()
{
	return mPropertiesDescriptionMap;
}

QMap<QString, QMap<QString, MetaElementImpl*> > MetaModelParser::getGraphicalObjects()
{
	return mGraphicalObjects;
}

QMap<QString, QStringList> MetaModelParser::getEnums()
{
	return mEnums;
}

QMap<QString, int> MetaModelParser::isNodeOrEdge()
{
	return mIsNodeOrEdge;
}

QMap<QString, QMap<QString, QDomElement> > MetaModelParser::getIconMap()
{
	return mIconMap;
}
