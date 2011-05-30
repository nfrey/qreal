#include "createNewElementDialog.h"
CreateNewElementDialog::CreateNewElementDialog(QWidget *parent)
		: QDialog(parent)
{

	ui.setupUi(this);

}
void CreateNewElementDialog::setLabelsName(QString firstLabel, QString secondLable)
{
	ui.label->setText(firstLabel);
	ui.label_2->setText(secondLable);
}

QString CreateNewElementDialog::getElementName() const
{
	return ui.elementNameEdit->text();
}

QString CreateNewElementDialog::getElementDisplayedName() const
{
	return ui.elementDisplayedNameEdit->text();
}

QString CreateNewElementDialog::getElementDescription() const
{
	return ui.elementDescriptionEdit->text();

}
