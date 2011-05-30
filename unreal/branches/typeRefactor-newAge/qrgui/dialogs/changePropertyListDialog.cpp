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
	connect(ui.addPropertyButton, SIGNAL(pressed()), SLOT(OpenNewElementDialog()));
	connect(ui.saveButton, SIGNAL(pressed()), SLOT(accept()));
}

void ChangePropertyListDialog::OpenNewElementDialog()
{
	mDialog = new CreateNewElementDialog(this);
	//mDialog->setLabelsName("property name", "default value");
	connect(mDialog, SIGNAL(accepted()), SLOT(AddNewProperty()));
	mDialog->show();
}

void ChangePropertyListDialog::AddNewProperty()
{

	ui.propertyTable->setRowCount(ui.propertyTable->rowCount() + 1);
	QTableWidgetItem *nameItem = new QTableWidgetItem(mDialog->getElementName());
	QTableWidgetItem *valueItem = new QTableWidgetItem(mDialog->getElementDisplayedName());
	ui.propertyTable->setItem(ui.propertyTable->rowCount()-1, 0, nameItem);
	ui.propertyTable->setItem(ui.propertyTable->rowCount()-1, 1, valueItem);
}

QMap<QString, QString> ChangePropertyListDialog::getPropertyNamesWithDefaultValue()
{
	QMap<QString, QString> res;
	for (int i = 0; i < ui.propertyTable->rowCount(); i++)
	{
		res.insert(ui.propertyTable->item(i,0)->text(), ui.propertyTable->item(i,1)->text());
	}
	return res;
}

