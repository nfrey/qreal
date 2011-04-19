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
}

QStringList MetaPlugin::diagrams() const
{
	return mDiagramNameMap.keys();
}

QStringList MetaPlugin::elements(const QString &diagram) const
{
	return mElementsNameMap[diagram].keys();
}

QStringList MetaPlugin::getPropertiesWithDefaultValues(QString const &element) const
{
	return mPropertyDefault[element].keys();
}

QStringList MetaPlugin::getTypesContainedBy(QString const &element) const
{
	QStringList result;

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
	return diagramNameMap[diagram];
}

QString MetaPlugin::diagramNodeName(QString const &diagram) const
{
	return diagramNodeNameMap[diagram];
}

QString MetaPlugin::elementName(QString const &diagram, QString const &element) const
{
	return elementsNameMap[diagram][element];
}

QString MetaPlugin::elementDescription(QString const &diagram, QString const &element) const
{
	return elementsDescriptionMap[diagram][element];
}

QString MetaPlugin::propertyDescription(QString const &diagram, QString const &element, QString const &property) const
{
	return propertiesDescriptionMap[diagram][element][property];
}

QString MetaPlugin::elementMouseGesture(QString const &diagram, QString const &element) const
{
	return elementMouseGesturesMap[diagram][element];
}

