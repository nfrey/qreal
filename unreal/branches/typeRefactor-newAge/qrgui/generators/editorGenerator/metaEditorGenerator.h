#pragma once

#include <QtCore/QString>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtCore/QHash>

#include "../../kernel/ids.h"
#include "../../mainwindow/errorReporter.h"
#include "../../../qrrepo/logicalRepoApi.h"
#include "../../editorManager/metaPlugin.h"

namespace qReal {

namespace generators {

class MetaEditorGenerator
{
public:
	explicit MetaEditorGenerator(MetaPlugin const *plugin);

	void generateEditor(QString const &pathToFile);
private:
	void serializeObjects(QDomElement &parent, QString const &diag);
	void createNode(QDomElement &parent, QString const &elem);
	void createEdge(QDomElement &parent, QString const &elem);
	void setProperties(QDomElement &parent, QString const &elem);
	void createDiagrams(QDomElement &parent);
	void ensureCorrectness (QDomElement element, QString const &tagName, QString const &value);

	MetaPlugin const *mPlugin;
	QDomDocument mDocument;
	QString mDiagramName;
};
}
}
