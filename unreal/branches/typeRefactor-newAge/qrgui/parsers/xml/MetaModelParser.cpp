#include "MetaModelParser.h"

#include <QtCore/QDebug>
#include <QtCore/QUuid>
#include <QtXml/QDomDocument>
#include <QtCore/QProcess>

#include "nameNormalizer.h"


using namespace qReal;
using namespace parsers;

MetaModelParser::MetaModelParser()
{
}

void MetaModelParser::parseFile(const QString &fileName)
{
	QFileInfo directoryName(fileName);
	QString fileBaseName = directoryName.baseName();

	if (containsName(fileBaseName))
		return;
	mDocument = utils::xmlUtils::loadDocument(fileName);

	Id const packageId = getPackageId();
	initMetamodel(doc, fileName, packageId);

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

	for (unsigned i = 0; i < diagramAttributes.length(); ++i)
	{
		QDomElement type = diagramAttributes.at(i).toElement();
		if (type.tagName() == "nonGraphicTypes")
			fillEnums(type);
		if (type.tagName() == "graphicTypes")
			fillGraphicElementType(type, NameNormalizer::normalize(diagram.attribute("name")));
	}
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

void MetaModelParser::fillGraphicElementType(QDomElement &graphicType, QString &diagramName)
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
}

void MetaModelParser::fillEdge(QDomElement &edge, QString diagramName)
{
	mIsNode = false;
	mIsContainer = false;
}

void MetaModelParser::fillNode(QDomElement &node, QString diagramName)
{
	mIsNode = true;

	QString normalizedName = NameNormalizer::normalize(qualifiedName(diagramName, node.attribute("name")));
	mElementsNameMap.insert(diagramName, new QMap<QString, QString>(normalizedName, node.attribute("displayedName")));

	//fill description map
	QString description = node.attribute("description");
	if (!description.isEmpty())
	{
		mElementsDescriptionMap.insert(diagramName, new QMap<QString, QString>(normalizedName, description));
	}

	setNodeAttributes(node, nodeId);
}

void MetaModelParser::setNodeAttributes(const QDomElement &node, const QString &elementName)
{
	QDomNodeList nodeList = node.childNodes();

	for (unsigned i = 0; i < nodeList.length(); ++i)
	{
		QDomElement tag = nodeList.at(i).toElement();
		if (tag.tagName() == "logic")
			setNodeConfigurations(tag, elementName);
		if (tag.tagName() == "graphics")
		{
			//add MetaElementImpl to the Map
		}
	}
}

void MetaModelParser::setNodeConfigurations(const QDomElement &tag, const QString &elementName)
{
	QDomNodeList nodeAttributes = tag.childNodes();

	for (unsigned i = 0; i < nodeAttributes.length(); ++i) {
		QDomElement attribute = nodeAttributes.at(i).toElement();
		switch (attribute.tagName())
		{
		case "container":
			setContainers(attribute, elementName);
			setTypedList(attribute, elementName, "contains", mElementContainedTypes);
		case "connections":
			setTypedList(attribute, elementName, "connection", mElementConnections);
		case "usages":
			setTypedList(attribute, elementName, "usage", mElementUsages);
		case "properties":
			setProperties(attribute, elementName);
		}

		if (attribute.tagName() == "generalizations")
			setGeneralization(attribute, nodeId);
		else if (attribute.tagName() == "pin")
			setPin(nodeId);
		else if (attribute.tagName() == "action")
			setAction(nodeId);
		else if (attribute.tagName() == "bonusContextMenuFields")
			setFields(attribute, nodeId);
	}
}

void MetaModelParser::setContainers(const QDomElement &container,
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
			switch (childElement.tagName())
			{
			case "sortContainer":
				mIsSortingContainer = true;
			case "forestalling":
				mSizeOfForestalling = childElement.attribute("size").toInt();
			case "childrenForestalling":
				mSizeOfChildrenForestalling = childElement.attribute("size").toInt();
			case "minimizeToChildren":
				mMinimizesToChildren = true;
			case "banChildrenMove":
				mHasMovableChildren = false;
			case "maximizeChildren":
				mMaximizesChildren = true;

			}
		}
	}
}

void MetaModelParser::setTypedList(QDomElement &node, const QString &elementName,
								   const QString &tagName,
								   QMap<QString, QMap<QString, QString> > map)
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

void MetaModelParser::setProperties(QDomElement &properties, const QString diagramName,
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
	mPropertiesDescriptionMap.insert(diagramName,
									 new QMap<QString, QMap<QString, QString>
									 (elementName,
									  descriptions);
}

void MetaModelParser::setEdgeAttributes(const QDomElement &edge, const Id &edgeId)
{
	QDomNodeList edgeList = edge.childNodes();

	for (unsigned i = 0; i < edgeList.length(); ++i) {
		QDomElement tag = edgeList.at(i).toElement();
		if (tag.tagName() == "logic")
			setEdgeConfigurations(tag, edgeId);
		if (tag.tagName() == "graphics")
		{
			//add metaElementImpl element
		}
	}
}

QString MetaModelParser::getQualifiedName(QString &context, QString &name)
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

QMap<QString, QMap<QString, UML::ElementImpl> > MetaModelParser::getGraphicalObjects()
{
	return mGraphicalObjects;
}

QMap<QString, QStringList> MetaModelParser::getEnums()
{
	return mEnums;
}
