#include "metamodelChangeManager.h"
using namespace qReal;
ItemForAdd::ItemForAdd(const NewType &type, const QString &name, const QString &description, const QIcon &icon, const int index)
	:mType(type), mName(name), mDescription(description), mIcon(icon), mIndex(index)
{
}

NewType ItemForAdd::getType() const
{
	return mType;
}

QString ItemForAdd::getName() const
{
	return mName;
}

QString ItemForAdd::getDescription() const
{
	return mDescription;
}

QIcon ItemForAdd::getIcon() const
{
	return mIcon;
}

int ItemForAdd::getIndex() const
{
	return mIndex;
}

MetamodelChangeManager::MetamodelChangeManager(NewType childElementType, EditorManager * editorManager)
	:mChildElementType(childElementType), mEditorManager(editorManager)
{
	mMetaPlugin = dynamic_cast<MetaPlugin *>(mEditorManager->getQuickMetamodelingPlugin());
}

MetamodelChangeManager::MetamodelChangeManager(EditorManager * editorManager)
	:mEditorManager(editorManager)
{
	mMetaPlugin = dynamic_cast<MetaPlugin *>(mEditorManager->getQuickMetamodelingPlugin());
}

ItemForAdd* MetamodelChangeManager::createCopyElement()
{
	ElementImpl* impl = mMetaPlugin->getGraphicalObject(mChildElementType.diagram(), mChildElementType.element());
	QDomElement elemForIcon = mMetaPlugin->getDomElementForIcon(mChildElementType.diagram(), mChildElementType.element());
	return createElement(mChildElementType.element() + "Copy", impl, elemForIcon);
}

ItemForAdd* MetamodelChangeManager::createNewElement(QString const &elementName, bool isNode)
{
	QDomElement graphicTag;
	MetaElementImpl* impl;
	if (!isNode)
	{
		graphicTag = this->createGraphicalObjectForNewConnection();
		impl = new MetaElementImpl(graphicTag,
								   false, false, false, 0, 0, false,
								   false, false, false, false,
								   QStringList(), "filled_arrow", "filled_arrow");
	}
	else
	{
		graphicTag = this->createGraphicalObjectForNewElement();
		impl = new MetaElementImpl(graphicTag,
								   true, true, false, 0, 0, true,
								   true, true, false, false,
								   QStringList(), "", "");
	}
	SdfRenderer* portRenderer = new SdfRenderer();
	SdfRenderer* renderer = new SdfRenderer();
	ElementTitleFactory factory;
	QList<ElementTitleInterface*> titles;
	QRectF rect = QRectF(0, 0, 100, 100);
	QList<StatPoint> points;
	QList<StatLine> lines;
	impl->init(rect, points, lines, factory, titles, renderer, portRenderer);
	return createElement(elementName, impl, graphicTag.firstChildElement("picture"));//, mDialog->getElementDisplayedName());
}

int MetamodelChangeManager::changeExistedElement(const QDomElement &element)
{
	MetaElementImpl* oldimpl = dynamic_cast<MetaElementImpl*>
			(mMetaPlugin->getGraphicalObject(mChildElementType.diagram(), mChildElementType.element()));
	QDomElement graphicTag = element;
	MetaElementImpl* impl = new MetaElementImpl(graphicTag, oldimpl);
	SdfRenderer* portRenderer = new SdfRenderer();
	SdfRenderer* renderer = new SdfRenderer();
	ElementTitleFactory factory;
	QList<ElementTitleInterface*> titles;
	QRectF rect = QRectF(0, 0, 100, 100);
	QList<StatPoint> points;
	QList<StatLine> lines;
	impl->init(rect, points, lines, factory, titles, renderer, portRenderer);
	return changeElement(mChildElementType.element(), impl, graphicTag.firstChildElement("picture"));
}

int MetamodelChangeManager::changeElement(const QString &elementName, ElementImpl *impl, QDomElement elementForIcon)
{
	mMetaPlugin->changeElement(mChildElementType.diagram(), elementName, elementName, impl, elementForIcon);

	int i = 0;
	for (i = 0; i < mEditorManager->editors().count(); i++)
	{
		if (mEditorManager->editors().at(i).editor() == mMetaPlugin->id())
			break;
	}
	return i;
}

ItemForAdd* MetamodelChangeManager::createElement(const QString &elementName, ElementImpl *impl, QDomElement elementForIcon)
{
	ItemForAdd* item;
	QString normalizedName = NameNormalizer::normalize(elementName);
	QString diagName = mMetaPlugin->diagrams().first();
	mMetaPlugin->addElement(diagName, elementName, elementName, impl, elementForIcon);

	int i = 0;
	for (i = 0; i < mEditorManager->editors().count(); i++)
	{
		if (mEditorManager->editors().at(i).editor() == mMetaPlugin->id())
			break;
	}

	foreach (NewType const editor, mEditorManager->editors())
	{
		if (editor.editor() == mMetaPlugin->id())
		{
			foreach (NewType const diagram, mEditorManager->diagrams(editor))
			{
				foreach (NewType const element, mEditorManager->elements(diagram))
				{
					if (element.element() == normalizedName)
						item = new ItemForAdd(element,
											  mEditorManager->friendlyName(element),
											  mEditorManager->description(element),
											  mEditorManager->icon(element), i);
				}
			}
		}
	}
	return item;
}

QDomElement MetamodelChangeManager::createGraphicalObjectForNewElement()
{
	QDomDocument doc;
	QDomElement graphicTag = doc.createElement("graphics");
	QDomElement pictureTag = doc.createElement("picture");
	pictureTag.setAttribute("sizex", 100);
	pictureTag.setAttribute("sizey", 100);
	QDomElement rectangleTag = doc.createElement("rectangle");
	rectangleTag.setTagName("rectangle");
	rectangleTag.setAttribute("fill", "#ffffff");
	rectangleTag.setAttribute("stroke-style", "solid");
	rectangleTag.setAttribute("stroke", "#000000");
	rectangleTag.setAttribute("y1", 0);
	rectangleTag.setAttribute("x1", 0);
	rectangleTag.setAttribute("y2", 100);
	rectangleTag.setAttribute("x2", 100);
	rectangleTag.setAttribute("stroke-width", 1);
	pictureTag.appendChild(rectangleTag);
	graphicTag.appendChild(pictureTag);

	return graphicTag;
}

QDomElement MetamodelChangeManager::createGraphicalObjectForNewConnection()
{
	QDomDocument doc;
	QDomElement graphicTag = doc.createElement("graphics");
	QDomElement lineTag = doc.createElement("lineType");

	lineTag.setAttribute("type", "solidLine");
	graphicTag.appendChild(lineTag);

	return graphicTag;
}

void MetamodelChangeManager::addNewProperty(const QString &element, const QString &property, const QString &defaultValue)
{
	QString diagName = mMetaPlugin->diagrams().first();
	mMetaPlugin->addProperty(diagName, element, property, defaultValue);
}
