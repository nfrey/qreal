#include <QDialog>
#include "ui_CreateNewElementDialog.h"

class CreateNewElementDialog : public QDialog {
	Q_OBJECT
public:
	CreateNewElementDialog(QWidget *parent = 0);
	void setLabelsName(QString first, QString second);
	QString getElementName() const;
	QString getElementDisplayedName() const;
	QString getElementDescription() const;
private:
	 Ui::CreateNewElementDialog ui;
};
