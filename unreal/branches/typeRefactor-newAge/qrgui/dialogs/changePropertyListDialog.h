#include <QDialog>
#include "ui_changePropertyListDialog.h"

class ChangePropertyListDialog : public QDialog {
	Q_OBJECT
public:
	ChangePropertyListDialog(QWidget *parent = 0);
	ChangePropertyListDialog(QWidget *parent = 0, QMap<QString,QString> propertyMap = QMap<QString, QString>());
private:
	Ui::ChangePropertyListDialog ui;
};
