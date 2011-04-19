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
		//if (type.tagName() == "nonGraphicTypes")
		//	createNonGraphicElements(type, diagramId);
		if (type.tagName() == "graphicTypes")
			fillGraphicElementType(type, NameNormalizer::normalize(diagram.attribute("name")));
	}
}

void MetaModelParser::fillGraphicElementType(QDomElement &graphicType, QString &diagramName)
{
	QDomNodeList graphicElements = graphicType.childNodes();

	for (unsigned i = 0; i < graphicElements.length(); ++i)
	{
		QDomElement graphicElement = graphicElements.at(i).toElement();
		fillGraphicElement(graphicElement, diagramName);

	}
}

void MetaModelParser::fillGraphicElement(QDomElement &node, QString diagramName)
{
	QString normalizedName = NameNormalizer::normalize(qualifiedName(diagramName, node.attribute("name")));
	mElementsNameMap.insert(diagramName, new QMap<QString, QString>(normalizedName, node.attribute("displayedName")));

	//fill description map
	QString description = node.attribute("description");
	if (!description.isEmpty())
	{
		mElementsDescriptionMap.insert(diagramName, new QMap<QString, QString>(normalizedName, description));
	}

	QDomElement logic = node.firstChildElement("logic");
	if (!logic.isNull())
	{
		QDomElement properties = logic.firstChildElement("properties");
		if (!properties.isNull())
		{
			for (QDomElement property = properties.firstChildElement("property");
				!property.isNull();
				property = property.nextSiblingElement("property"))
			{
				fillPropertyMap(property, normalizedName);
			}
		}
	}
}

void MetaModelParser::fillProperyMap(QDomElement &property, QString &elementName)
{
	QString name = property.attribute("name");
	QString value = property.attribute("default");
	QString type = property.attribute("type");
	mPropertyDefault.insert(elementName, new QMap<QString, QString>(name, value));
	mPropertyTypes.insert(elementName, new QMap<QString, QString>(name, NameNormalizer::normalize(type)));
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

