#include <QtCore/QHash>
#include <QtGui/QWidget>
#include <QtGui/QIcon>

#include "../kernel/NewType.h"
#include "../editorManager/metaPlugin.h"
#include "../editorManager/editorManager.h"
#include "../../qrxc/nameNormalizer.h"
#include "../umllib/elementTitle.h"

namespace qReal
{
class ItemForAdd
{
public:
	ItemForAdd(NewType const &type, QString const &name, QString const &description,  QIcon const &icon, int const index);
	NewType getType() const;
	QString getName() const;
	QString getDescription() const;
	QIcon getIcon() const;
	int getIndex() const;
private:
	NewType mType;
	QString mName;
	QString mDescription;
	QIcon mIcon;
	int mIndex;
};

class MetamodelChangeManager : public QObject
{
public:
	MetamodelChangeManager(NewType childElementType, EditorManager * editorManager);
	MetamodelChangeManager(EditorManager * editorManager);
	ItemForAdd* createNewElement(QString const &elementName, bool isNode);
	ItemForAdd* createCopyElement();
	int changeExistedElement(QDomElement const &element);
	void addNewProperty(QString const &element, QString const &property, QString const &defaultValue);
private:
	ItemForAdd* createElement(QString const &elementName, ElementImpl* impl, QDomElement elementForIcon);
	QDomElement createGraphicalObjectForNewElement();
	QDomElement createGraphicalObjectForNewConnection();
	int changeElement(QString const &elementName, ElementImpl* impl, QDomElement elementForIcon);

	NewType mChildElementType;
	MetaPlugin* mMetaPlugin;
	EditorManager * mEditorManager;
};
}
