#include <QtCore/QUuid>
#include <QtGui/QLabel>
#include <QtGui/QMouseEvent>
#include <QtGui/QVBoxLayout>
#include <QtGui/QComboBox>
#include <QtGui/QScrollArea>
#include <QMenu>

#include "paletteToolbox.h"
#include "../kernel/definitions.h"

using namespace qReal;
using namespace qReal::gui;

PaletteToolbox::DraggableElement::DraggableElement(NewType const &type, QString const &name, QString const &description,
												   QIcon const &icon, QWidget *parent)
	: QWidget(parent), mType(type), mIcon(icon), mText(name)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setContentsMargins(4, 4, 4, 4);

	QLabel *pic = new QLabel(this);
	pic->setFixedSize(24, 24); // the frame
	pic->setPixmap(mIcon.pixmap(22, 22));
	layout->addWidget(pic);

	QLabel *text = new QLabel(this);
	text->setText(mText);
	layout->addWidget(text);

	setLayout(layout);

	QString modifiedDescription = description;
	if (!modifiedDescription.isEmpty()){
		modifiedDescription.insert(0, "<body>");//turns alignment on
		setToolTip(modifiedDescription);
	}

}

PaletteToolbox::PaletteToolbox(QWidget *parent)
	: QWidget(parent)
{
	mLayout = new QVBoxLayout;
	mLayout->setSpacing(6);
	mLayout->setMargin(0);

	mComboBox = new QComboBox;
	mLayout->addWidget(mComboBox);

	mScrollArea = new QScrollArea;
	mLayout->addWidget(mScrollArea);

	setLayout(mLayout);
}

PaletteToolbox::~PaletteToolbox()
{
	mScrollArea->takeWidget();
	delete mScrollArea;
	delete mComboBox;
	delete mLayout;

	for (int i = 0; i < mTabs.count(); i++)
		delete mTabs[i];
}

void PaletteToolbox::setEditorManager(EditorManager *manager)
{
	mEditorManager = manager;
}

void PaletteToolbox::setActiveEditor(int const comboIndex)
{
	if (mTabs.size() > 0) {
		mScrollArea->takeWidget(); // Save current editor from extermination.
		mScrollArea->setWidget(mTabs[comboIndex]);
	}
}

void PaletteToolbox::addDiagramType(NewType const &type, QString const &name)
{
	QWidget *tab = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout(tab);

	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);

	tab->setLayout(layout);

	mTabs.append(tab);
	mTabNames.append(name);

	Q_ASSERT(type.typeSize() == 2); // it should be diagram
	mCategories[type] = mTabs.size() - 1;

	mComboBox->addItem(name);

	Q_ASSERT(mTabNames.size() == mTabs.size());
}

void PaletteToolbox::addItemType(NewType const &type, QString const &name, QString const &description,  QIcon const &icon)
{
	NewType category(type.editor(), type.diagram());
	QWidget *tab = mTabs[mCategories[category]];
	Q_ASSERT(tab);

	DraggableElement *element = new DraggableElement(type, name, description, icon, this);
	tab->layout()->addWidget(element);
}

void PaletteToolbox::deleteDiagramType(const NewType &type)
{
	if (mCategories.contains(type)) {
		mComboBox->removeItem(mCategories[type]);
		mTabNames.remove(mCategories[type]);
		mTabs.remove(mCategories[type]);
		mCategories.remove(type);
	}
}

void PaletteToolbox::initDone()
{
	connect(mComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setActiveEditor(int)));
	setActiveEditor(0);
}

QComboBox* PaletteToolbox::getComboBox()
{
	return mComboBox;
}

QVector<QString> PaletteToolbox::getTabNames()
{
	return mTabNames;
}

void PaletteToolbox::dragEnterEvent(QDragEnterEvent * /*event*/)
{
}

void PaletteToolbox::dropEvent(QDropEvent * /*event*/)
{
}

void PaletteToolbox::mousePressEvent(QMouseEvent *event)
{
	QWidget *atMouse = childAt(event->pos());
	if (!atMouse || atMouse == this)
		return;

	DraggableElement *child = dynamic_cast<DraggableElement *>(atMouse->parent());
	if (!child)
		child = dynamic_cast<DraggableElement *>(atMouse);
	if (!child)
		return;

	Q_ASSERT(child->type().typeSize() == 3); // it should be element type

	if (event->button() == Qt::RightButton)
	{
		QMenu *menu = new QMenu(atMouse);
		QAction *newElementAction = menu->addAction("Create new element");
		QAction *copyElementAction = menu->addAction("Create element copy");
		connect(newElementAction, SIGNAL(triggered()), SLOT(createNewElement()));
		connect(copyElementAction, SIGNAL(triggered()), SLOT(createElementCopy()));

		QPoint cursorPos = QCursor::pos();
		menu->exec(cursorPos);
	}
	// new element's ID is being generated here
	// may this epic event should take place in some more appropriate place

	Id elementId(QUuid::createUuid().toString());
	NewType elementType = child->type();
	QByteArray itemData;
	bool isFromLogicalModel = false;

	QDataStream stream(&itemData, QIODevice::WriteOnly);
	stream << elementId.toString();  // uuid
	stream << elementType.toString(); //type
	stream << ROOT_ID.toString();  // pathToItem
	stream << QString("(" + child->text() + ")");
	stream << QPointF(0, 0);
	stream << isFromLogicalModel;

	QMimeData *mimeData = new QMimeData;
	mimeData->setData("application/x-real-uml-data", itemData);

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);

	QPixmap p = child->icon().pixmap(96, 96);

	if (!p.isNull())
		drag->setPixmap(child->icon().pixmap(96, 96));

	if (drag->start(Qt::CopyAction | Qt::MoveAction) == Qt::MoveAction)
		child->close();
	else
		child->show();
}

void PaletteToolbox::createNewElement()
{
	MetaPlugin * metaPlugin = dynamic_cast<MetaPlugin *>(mEditorManager->getQuickMetamodelingPlugin());

	metaPlugin->addElement("TestDiagram", "bla", "bla");
	foreach (NewType const editor, mEditorManager->editors())
	{
		if (editor.editor() == metaPlugin->id())
		{
			foreach (NewType const diagram, mEditorManager->diagrams(editor))
			{
				foreach (NewType const element, mEditorManager->elements(diagram))
				{
					if (element.element() == "Bla")
						this->addItemType(element, mEditorManager->friendlyName(element), mEditorManager->description(element), mEditorManager->icon(element));
				}
			}

		}
	}
	connect(mComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setActiveEditor(int)));
	int i = 0;
	for (i = 0; i < mEditorManager->editors().count(); i++)
	{
		if (mEditorManager->editors().at(i).editor() == metaPlugin->id())
			break;
	}
	setActiveEditor(i);
}

void PaletteToolbox::createElementCopy()
{

}
