#pragma once

#include <QtCore/QHash>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtXml/QDomElement>
#include "../../kernel/ids.h"
#include "../../../qrrepo/logicalRepoApi.h"
#include "../../../qrgui/pluginInterface/editorInterface.h"
#include "../../../qrgui/pluginInterface/elementRepoInterface.h"
#include "../../../qrgui/pluginInterface/elementTitleHelpers.h"
#include "metaElementImpl.h"

namespace qrRepo {
class RepoApi;
}

namespace qReal {

class EditorManager;

namespace parsers {

class MetaModelParser
{
public:
	MetaModelParser();

	void parseFile(QString const &fileName);
	void loadIncludeList(QString const &fileName);
	QMap<QString, QString> getDiagramNameMap();
	QMap<QString, QMap<QString, QString> > getElementNameMap();
	QMap<QString, QMap<QString, QString> > getElementDescriptionMap();
	QMap<QString, QMap<QString, QString> > getPropertyTypes();
	QMap<QString, QMap<QString, QString> > getPropertyDefaultValues();
	QMap<QString, QMap<QString, UML::ElementImpl*> > getGraphicalObjects();
	QMap<QString, QStringList > getElementContainedTypes();
	QMap<QString, QStringList > getElementConnections();
	QMap<QString, QStringList > getElementUsages();
	QMap<QString, QMap<QString, QMap<QString, QString> > > getPropertyDescriptionMap();
	QMap<QString, QStringList > getEnums();
	QMap<QString, int> isNodeOrEdge();

private:
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
	QString mStartArrowStyle;
	QString mEndArrowStyle;
	QStringList mBonusContextMenuFields;

	QDomDocument mDocument;
	QMap<QString, QString> mDiagramNameMap;
	QMap<QString, QMap<QString, QString> > mElementsNameMap;
	QMap<QString, QMap<QString, QString> > mElementsDescriptionMap;
	QMap<QString, QMap<QString, QString> > mPropertyTypes;
	QMap<QString, QMap<QString, QString> > mPropertyDefault;
	QMap<QString, QMap<QString, QMap<QString, QString> > > mPropertiesDescriptionMap;
	QMap<QString, QMap<QString, QString> > mElementMouseGesturesMap;
	QMap<QString, QMap<QString, UML::ElementImpl*> > mGraphicalObjects;
	QMap<QString, QStringList > mElementContainedTypes;
	QMap<QString, QStringList > mElementConnections;
	QMap<QString, QStringList > mElementUsages;
	QMap<QString, QStringList > mEnums;
	QMap<QString, int> mIsNodeOrEdge;
	QMap<QString, QString> mElementNames;
	QMap<QString, UML::ElementImpl*> mGraphicObjectsForElements;
	QMap<QString, QMap<QString, QString> > mPropertyDescriptionsForElements;
	QMap<QString, QString> mElementDescriptionsForElements;


	void fillDiagramNameMap();
	void fillDiagramAttributes(QDomElement &diagram);
	void fillGraphicElementType(QDomElement &graphicType, QString const &diagramName);
	void fillNode(QDomElement &node, QString  const &diagramName);
	void fillEdge(QDomElement &edge, QString const &diagramName);
	QString getQualifiedName(QString const &context, QString const &name);
	void setContainers(QDomElement &container, QString const &elementName);
	void setProperties(QDomElement &properties, QString const &diagramName, QString const &elementName);
	void fillEnums(QDomElement &nonGraphicType);
	void setNodeConfigurations(const QDomElement &tag, const QString &diagramName, const QString &elementName);
	void setNodeAttributes(const QDomElement &node, const QString &diagramName, const QString &elementName);
	void setEdgeConfigurations(const QDomElement &tag, const QString &diagramName, const QString &elementName);
	void setEdgeAttributes(const QDomElement &node, const QString &diagramName, const QString &elementName);
	void setPossibleEdges(const QDomElement &element, QString const &diagramName, QString const &elementName);
	void setAssociations(const QDomElement &element);
	void setTypedList(QDomElement &node, const QString &elementName,
									   const QString &tagName,
									   QMap<QString, QStringList > map);
};
}
}
