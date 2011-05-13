#include "createNewElementDialog.h"
CreateNewElementDialog::CreateNewElementDialog(QWidget *parent)
		: QDialog(parent)
{

	ui.setupUi(this);

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
