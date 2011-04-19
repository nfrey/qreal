#pragma once

#include <QtCore/QHash>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtXml/QDomElement>

#include "../../kernel/ids.h"
#include "../../../qrrepo/logicalRepoApi.h"

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

private:
	QDomDocument mDocument;
	QMap<QString, QString> mDiagramNameMap;
	QMap<QString, QMap<QString, QString> > mElementsNameMap;
	QMap<QString, QMap<QString, QString> > mElementsDescriptionMap;
	QMap<QString, QMap<QString, QString> > mPropertyTypes;
	QMap<QString, QMap<QString, QString> > mPropertyDefault;
	QMap<QString, QMap<QString, QMap<QString, QString> > > mPropertiesDescriptionMap;
	QMap<QString, QMap<QString, QString> > mElementMouseGesturesMap;

	void fillDiagramNameMap();
	void fillDiagramAttributes(QDomElement &diagram);
	void fillGraphicElementType(QDomElement &graphicType, QString &diagramName);
	void fillGraphicElement(QDomElement &node, QString &diagramName);
	void fillProperyMap(QDomElement &property, QString &elementName);
	QString getQualifiedName(QString &context, QString &name);

};
}
}
