#pragma once
#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtGui/QIcon>
#include <QPair>

#include "../../pluginInterface/editorInterface.h"
#include "../../parsers/xml/metaElementImpl.h"
#include "../../umllib/sdfRenderer.h"

class MetaPlugin : public QObject, public qReal::EditorInterface
{
	Q_OBJECT
	Q_INTERFACES(qReal::EditorInterface)

public:

	MetaPlugin(QString const &fileName);

	virtual void initPlugin();
//	virtual void initMouseGestureMap();
//	virtual void initNameMap();
//	virtual void initPropertyMap();
//	virtual void initPropertyDefaultsMap();
//	virtual void initDescriptionMap();

	virtual QString id() const { return "meta"; }
	virtual bool isQuickMetaModelingMode() const { return true; }
	virtual QStringList diagrams() const;
	virtual QStringList elements(QString const &diagram) const;
	virtual QStringList getPropertiesWithDefaultValues(QString const &element) const;

	virtual QStringList getTypesContainedBy(QString const &element) const;
	virtual QStringList getConnectedTypes(QString const &element) const;
	virtual QStringList getUsedTypes(QString const &element) const;
	virtual QList<QPair<QPair<QString,QString>,QPair<bool,QString> > > getPossibleEdges(QString const &element) const;//

	virtual int isNodeOrEdge(QString const &element) const;

	virtual QIcon getIcon(SdfIconEngineV2Interface *engine) const;
	virtual QIcon getIcon(SdfIconEngineV2Interface *engine, QString const &diagramName, QString const &elementName) const;
	virtual UML::ElementImpl* getGraphicalObject(QString const &diagram, QString const &element) const;
	virtual QString getPropertyType(QString const &element, QString const &property) const;
	virtual QString getPropertyDefaultValue(QString const &element, QString const &property) const;
	virtual QStringList getPropertyNames(QString const &diagram, QString const &element) const;
	virtual QStringList getEnumValues(QString name) const;

	virtual QString editorName() const;
	virtual QString diagramName(QString const &diagram) const;
	virtual QString diagramNodeName(QString const &diagram) const;
	virtual QString elementName(QString const &diagram, QString const &element) const;
	virtual QString elementDescription(QString const &diagram, QString const &element) const;
	virtual QString propertyDescription(QString const &diagram, QString const &element, QString const &property) const;
	virtual QString elementMouseGesture(QString const &digram, QString const &element) const;

	virtual QList<qReal::ListenerInterface*> listeners() const;

	void addElement(QString const &diagramName, QString const &element, QString const &elementDisplayName);
private:
	QString const &mFileName;
	QMap<QString, QIcon> iconMap;
	QMap<QString, QString> mDiagramNameMap;
	QMap<QString, QString> mDiagramNodeNameMap;//
	QMap<QString, QMap<QString, QString> > mPropertyTypes;
	QMap<QString, QMap<QString, QString> > mPropertyDefault;
	QMap<QString, QMap<QString, QString> > mElementsNameMap;
	QMap<QString, QMap<QString, MetaElementImpl*> > mGraphicalObjects;
	QMap<QString, QStringList> mElementContainedTypes;
	QMap<QString, QStringList> mElementConnections;
	QMap<QString, QStringList> mElementUsages;
	QMap<QString, QMap<QString, QString> > mElementsDescriptionMap;
	QMap<QString, QMap<QString, QMap<QString, QString> > > mPropertiesDescriptionMap;
	QMap<QString, QMap<QString, QString> > mElementMouseGesturesMap;//
	QMap<QString, QStringList> mEnums;
	QMap<QString, int> mIsNodeOrEdge;
	QMap<QString, QMap<QString, QDomElement> > mIconMap;

	QString getQualifiedName(QString const &context, QString const &name) const;
};
