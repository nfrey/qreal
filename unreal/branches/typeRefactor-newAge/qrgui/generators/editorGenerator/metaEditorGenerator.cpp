#include "metaEditorGenerator.h"

#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QMessageBox>

#include "../../kernel/roles.h"

#include "../../../utils/outFile.h"

using namespace qReal;
using namespace generators;
using namespace utils;

MetaEditorGenerator::MetaEditorGenerator(MetaPlugin const *plugin)
	: mPlugin(plugin)
{
}

void MetaEditorGenerator::generateEditor(const QString &pathToFile)
{
	QFileInfo fileName(pathToFile);

	QDomElement metamodel = mDocument.createElement("metamodel");
	metamodel.setAttribute("xmlns", "http://schema.real.com/schema/");
	mDocument.appendChild(metamodel);

	QDomElement nameSpace = mDocument.createElement("namespace");
	QDomText nameSpaceName = mDocument.createTextNode("UML 2.0");
	nameSpace.appendChild(nameSpaceName);
	metamodel.appendChild(nameSpace);

	createDiagrams(metamodel);

	OutFile outxml(pathToFile);
	outxml() << "<?xml version='1.0' encoding='utf-8'?>\n";
	outxml() << mDocument.toString(4);
	mDocument.clear();
}

void MetaEditorGenerator::createDiagrams(QDomElement &parent)
{
	QStringList rootElements = mPlugin->diagrams();
	foreach (QString const diag, rootElements) {
		QDomElement diagram = mDocument.createElement("diagram");
		ensureCorrectness(diagram, "name", diag);
		ensureCorrectness(diagram, "displayedName", mPlugin->diagramName(diag));
		ensureCorrectness(diagram, "nodeName", mPlugin->diagramNodeName(diag));
		parent.appendChild(diagram);

		serializeObjects(diagram, diag);
	}
}

void MetaEditorGenerator::serializeObjects(QDomElement &parent, QString const &diag)
{
	QStringList elements = mPlugin->elements(diag);
	mDiagramName = diag;

	QDomElement tagGraphic = mDocument.createElement("graphicTypes");
	parent.appendChild(tagGraphic);

	foreach (QString const elem, elements) {
		if (mPlugin->isNodeOrEdge(elem) == 1)
			createNode (tagGraphic, elem);
		else
			createEdge (tagGraphic, elem);
	}
}

void MetaEditorGenerator::createNode(QDomElement &parent, QString const &elem)
{
	QDomElement node = mDocument.createElement("node");
	ensureCorrectness(node, "name", elem);
	ensureCorrectness(node, "displayedName", mPlugin->elementName(mDiagramName, elem));
	parent.appendChild(node);

	QDomElement graphics = mDocument.createElement("graphics");
	graphics.appendChild(mPlugin->getDomElementForIcon(mDiagramName, elem));
	node.appendChild(graphics);

	QDomElement logic = mDocument.createElement("logic");
	node.appendChild(logic);
	setProperties(logic, elem);
}

void MetaEditorGenerator::createEdge(QDomElement &parent, QString const &elem)
{
	QDomElement edge = mDocument.createElement("edge");
	ensureCorrectness(edge, "name", elem);
	ensureCorrectness(edge, "displayedName", mPlugin->elementName(mDiagramName, elem));
	parent.appendChild(edge);
	QDomElement graphics = mDocument.createElement("graphics");
	edge.appendChild(graphics);
	QDomElement lineType = mDocument.createElement("lineType");
	ensureCorrectness(lineType, "type", "solidLine");
	graphics.appendChild(lineType);


	QDomElement logic = mDocument.createElement("logic");
	edge.appendChild(logic);

	setProperties(logic, elem);
}

void MetaEditorGenerator::setProperties(QDomElement &parent,QString const &elem)
{
	QDomElement tagProperties = mDocument.createElement("properties");
	QStringList properties = mPlugin->getPropertyNames(mDiagramName, elem);

	foreach (QString const prop, properties)
	{
		QDomElement property = mDocument.createElement("property");
		ensureCorrectness(property, "type", "string");
		ensureCorrectness(property, "name", prop);
		QString defaultValue = mPlugin->getPropertyDefaultValue(elem, prop);
		if (!defaultValue.isEmpty())
		{
			QDomElement defaultTag = mDocument.createElement("default");
			QDomText value = mDocument.createTextNode(defaultValue);
			defaultTag.appendChild(value);
			property.appendChild(defaultTag);
		}
		tagProperties.appendChild(property);
	}


	if (!tagProperties.childNodes().isEmpty())
		parent.appendChild(tagProperties);
}


void MetaEditorGenerator::ensureCorrectness(QDomElement element, const QString &tagName, const QString &value)
{
	QString tag = tagName;
	if ((value == "") && ((tag == "displayedName")))
		return;
	if (value == "") {
		element.setAttribute(tagName, "");
	}
	else if (tag == "name") {
		QRegExp patten;
		patten.setPattern("[A-Za-z_]+([A-Za-z_0-9 :]*)");
		if (patten.exactMatch(value))
			element.setAttribute(tagName, value);
		else {
			element.setAttribute(tagName, value);
		}
	}
	else element.setAttribute(tagName, value);
}

