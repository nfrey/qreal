#include "metaPlugin.h"
#include "../parsers/xml/MetaModelParser.cpp"

MetaPlugin::MetaPlugin(QString const &fileName):
	mFileName(fileName)
{}

void MetaPlugin::initPlugin()
{
	MetaModelParser parser = MetaModelParser();
	parser.parseFile(mFileName);
	mElementsNameMap = parser.getElementNameMap();
	mDiagramNameMap = parser.getDiagramNameMap();
	mDiagramNodeNameMap = parser.getDiagramNodeNameMap();
	mElementsDescriptionMap = parser.getElementDescriptionMap();
	mPropertyDefault = parser.getPropertyDefaultValues();
	mPropertyTypes = parser.getPropertyTypes();
	mElementContainedTypes = parser.getElementContainedTypes();
	mPropertiesDescriptionMap = parser.getPropertyDescriptionMap();
	mElementConnections = parser.getElementConnections();
	mElementUsages = parser.getElementUsages();
	mGraphicalObjects = parser.getGraphicalObjects();
	mEnums = parser.getEnums();
	mIsNodeOrEdge = parser.isNodeOrEdge();
	mIconMap = parser.getIconMap();
}

QList<QPair<QPair<QString,QString>,QPair<bool,QString> > > MetaPlugin::getPossibleEdges(const QString &element) const
{
	return 	QList<QPair<QPair<QString,QString>,QPair<bool,QString> > >();
}

QList<qReal::ListenerInterface*> MetaPlugin::listeners() const
{
	return QList<qReal::ListenerInterface*>();
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
	return new MetaElementImpl(mGraphicalObjects[diagram][element]);
}

QDomElement MetaPlugin::getDomElementForIcon(QString const &diagram, QString const &element) const
{
	return mIconMap[diagram][element];
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

int MetaPlugin::isNodeOrEdge(QString const &element) const
{
	return mIsNodeOrEdge[element];
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

QIcon MetaPlugin::getIcon(SdfIconEngineV2Interface *engine, QString const &diagramName, QString const &elementName) const
{
	if (diagramName.isEmpty() || elementName.isEmpty())
		return QIcon(engine);
	SdfIconEngineV2* eng = new SdfIconEngineV2(mIconMap[diagramName][elementName]);
	return QIcon(eng);
}

QString MetaPlugin::editorName() const
{
	return "\"ololo Editor\"";
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

void MetaPlugin::addElement(const QString &diagramName, const QString &element, const QString &elementDisplayName,
							ElementImpl* impl, QDomElement elementForIcon)
{
	QString normalizedName = NameNormalizer::normalize(getQualifiedName(diagramName, element));
	if (mElementsNameMap[diagramName].contains(normalizedName))
		mElementsNameMap[diagramName].insert(normalizedName + "Copy", elementDisplayName + "Copy");
	else
		mElementsNameMap[diagramName].insert(normalizedName, elementDisplayName);

	mGraphicalObjects[diagramName].insert(normalizedName, dynamic_cast<MetaElementImpl*>(impl));
	mIconMap[diagramName].insert(normalizedName, elementForIcon);
}

void MetaPlugin::changeElement(const QString &diagramName, const QString &element, const QString &elementDisplayName,
							   ElementImpl* impl, QDomElement elementForIcon)
{
	QString normalizedName = NameNormalizer::normalize(getQualifiedName(diagramName, element));
	mElementsNameMap[diagramName][normalizedName] = elementDisplayName;
	mGraphicalObjects[diagramName][normalizedName] = dynamic_cast<MetaElementImpl*>(impl);
	mIconMap[diagramName][normalizedName] = elementForIcon;
}

void MetaPlugin::addProperty(const QString &diagramName, const QString &element, const QString &property, const QString &defaultValue)
{
	QString normalizedName = NameNormalizer::normalize(getQualifiedName(diagramName, element));
	if (mPropertiesDescriptionMap[diagramName].contains(normalizedName))
	{
		mPropertiesDescriptionMap[diagramName][normalizedName].insert(property, defaultValue);
		mPropertyDefault[normalizedName].insert(property, defaultValue);
	}
	else
	{
		QMap<QString, QString> map;
		map.insert(property, defaultValue);
		mPropertiesDescriptionMap[diagramName].insert(normalizedName, map);
		mPropertyDefault.insert(normalizedName, map);
	}
}

QString MetaPlugin::getQualifiedName(QString const &context, QString const &name) const
{
	return context + "::" + name;
}
