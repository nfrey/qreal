#include <QDialog>
#include "ui_CreateNewElementDialog.h"

class CreateNewElementDialog : public QDialog {
	Q_OBJECT
public:
	CreateNewElementDialog(QWidget *parent = 0);
	QString getElementName() const;
	QString getElementDisplayedName() const;
	QString getElementDescription() const;
private:
	 Ui::CreateNewElementDialog ui;
};
