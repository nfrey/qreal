#include <QDialog>
#include "ui_changePropertyListDialog.h"
#include "createNewElementDialog.h"

class ChangePropertyListDialog : public QDialog {
	Q_OBJECT
public:
	ChangePropertyListDialog(QWidget *parent = 0);
	ChangePropertyListDialog(QWidget *parent = 0, QMap<QString,QString> propertyMap = QMap<QString, QString>());
	QMap<QString, QString> getPropertyNamesWithDefaultValue();
private:
	Ui::ChangePropertyListDialog ui;
	CreateNewElementDialog* mDialog;
private slots:
	void AddNewProperty();
	void OpenNewElementDialog();
};
