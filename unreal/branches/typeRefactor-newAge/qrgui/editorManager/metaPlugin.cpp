#include "metaPlugin.h"
#include "../parsers/xml/MetaModelParser.cpp"

void MetaPlugin::initPlugin()
{
	MetaModelParser parser = new MetaModelParser();
	parser.parseFile("");
	mElementsNameMap = parser.getElementNameMap();
	mDiagramNameMap = parser.getDiagramNameMap();
	mElementsDescriptionMap = parser.getElementDescriptionMap();
	mPropertyDefault = parser.getPropertyDefaultValues();
	mPropertyTypes = parser.getPropertyTypes();
	mElementContainedTypes = parser.getElementContainedTypes();
	mPropertiesDescriptionMap = parser.getPropertyDescriptionMap();
	mElementConnections = parser.getElementConnections();
	mElementUsages = parser.getElementUsages();
	mGraphicalObjects = parser.getGraphicalObjects();
	mEnums = parser.getEnums();
}

QStringList MetaPlugin::diagrams() const
{
	return mDiagramNameMap.keys();
}

QStringList MetaPlugin::elements(const QString &diagram) const
{
	return mElementsNameMap[diagram].keys();
}

UML::ElementImpl* MetaPlugin::getGraphicalObject(QString const &diagram, QString const &element) const
{
	return mGraphicalObjects[diagram][element];
}

QStringList MetaPlugin::getPropertiesWithDefaultValues(QString const &element) const
{
	return mPropertyDefault[element].keys();
}

QStringList MetaPlugin::getTypesContainedBy(QString const &element) const
{
	return mElementContainedTypes[element];
}

QStringList MetaPlugin::getConnectedTypes(QString const &element) const
{
	return mElementConnections[element];
}

QStringList MetaPlugin::getUsedTypes(QString const &element) const
{
	return mElementUsages[element];
}

QString MetaPlugin::getPropertyType(QString const &element, QString const &property) const
{
	return mPropertyTypes[element][property];
}

QString MetaPlugin::getPropertyDefaultValue(QString const &element, QString const &property) const
{
	return mPropertyDefault[element][property];
}

QStringList MetaPlugin::getPropertyNames(QString const &diagram, QString const &element) const
{
	return mPropertiesDescriptionMap[diagram][element].keys();
}

QStringList MetaPlugin::getEnumValues(QString name) const
{
	return mEnums[name];
}

QIcon MetaPlugin::getIcon(SdfIconEngineV2Interface *engine) const
{
	return QIcon(engine);
}

QString MetaPlugin::editorName() const
{
	return "\"meta Editor\"";
}

QString MetaPlugin::diagramName(QString const &diagram) const
{
	return mDiagramNameMap[diagram];
}

QString MetaPlugin::diagramNodeName(QString const &diagram) const
{
	return mDiagramNodeNameMap[diagram];
}

QString MetaPlugin::elementName(QString const &diagram, QString const &element) const
{
	return mElementsNameMap[diagram][element];
}

QString MetaPlugin::elementDescription(QString const &diagram, QString const &element) const
{
	return mElementsDescriptionMap[diagram][element];
}

QString MetaPlugin::propertyDescription(QString const &diagram, QString const &element, QString const &property) const
{
	return mPropertiesDescriptionMap[diagram][element][property];
}

QString MetaPlugin::elementMouseGesture(QString const &diagram, QString const &element) const
{
	return mElementMouseGesturesMap[diagram][element];
}

