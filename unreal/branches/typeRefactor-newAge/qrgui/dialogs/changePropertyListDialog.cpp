#include "changePropertyListDialog.h"
ChangePropertyListDialog::ChangePropertyListDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

ChangePropertyListDialog::ChangePropertyListDialog(QWidget *parent, QMap<QString, QString> propertyMap)
	: QDialog(parent)
{
	ui.setupUi(this);
	QStringList propertyNames = propertyMap.keys();
	ui.propertyTable->setRowCount(propertyNames.count());
	for(int i = 0; i < propertyNames.count(); i++)
	{
		QString key = propertyNames[i];
		QTableWidgetItem *nameItem = new QTableWidgetItem(key);
		QTableWidgetItem *valueItem = new QTableWidgetItem(propertyMap[key]);
		ui.propertyTable->setItem(i, 0, nameItem);
		ui.propertyTable->setItem(i, 1, valueItem);
	}
}
