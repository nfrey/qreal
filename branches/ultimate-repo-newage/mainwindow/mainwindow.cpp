/** @file mainwindow.cpp
 * 	@brief Главное окно приложения
 * */
#include <QtGui>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSvg/QSvgGenerator>
#include <QMessageBox>
#include <QTcpSocket>

#include "mainwindow.h"

#include "propertyeditorproxymodel.h"
#include "realrepomodel.h"
#include "editorview.h"
#include "optionsDialog.h"

using namespace qReal;

MainWindow::MainWindow() : model(0)
{
	QPixmap korkodil(":/icons/kroki2.PNG");
	QSplashScreen splash(QPixmap(":/icons/kroki2.PNG"), Qt::SplashScreen | Qt::WindowStaysOnTopHint);
	splash.show();
	QApplication::processEvents();

	ui.setupUi(this);

	ui.minimapView->setScene(ui.view->scene());
	ui.minimapView->setRenderHint(QPainter::Antialiasing, true);

	connect(ui.diagramExplorer, SIGNAL( activated( const QModelIndex & ) ),
			ui.view->mvIface(), SLOT( setRootIndex( const QModelIndex & ) ) );
	connect(ui.view->scene(), SIGNAL(selectionChanged()), SLOT(sceneSelectionChanged()));
	connect(ui.diagramExplorer, SIGNAL( clicked( const QModelIndex & ) ),
			this, SLOT( activateItemOrDiagram( const QModelIndex & ) ) );
	connect(ui.actionConnect, SIGNAL( triggered() ), this, SLOT( connectRepo() ) );
	connect(ui.actionDisconnect, SIGNAL( triggered() ), this, SLOT( closeRepo() ) );
	connect(ui.actionQuit, SIGNAL( triggered() ), this, SLOT( close() ) );

	connect(ui.actionZoom_In, SIGNAL( triggered() ), ui.view, SLOT( zoomIn() ) );
	connect(ui.actionZoom_Out, SIGNAL( triggered() ), ui.view, SLOT( zoomOut() ) );

	connect(ui.actionAntialiasing, SIGNAL( toggled(bool) ), ui.view, SLOT( toggleAntialiasing(bool) ) );
	connect(ui.actionOpenGL_Renderer, SIGNAL( toggled(bool) ), ui.view, SLOT( toggleOpenGL(bool) ) );

	connect(ui.actionPrint, SIGNAL( triggered() ), this, SLOT( print() ) );
	connect(ui.actionMakeSvg, SIGNAL( triggered() ), this, SLOT( makeSvg() ) );

	connect(ui.actionDeleteFromDiagram, SIGNAL( triggered() ), this, SLOT( deleteFromDiagram() ) );

	connect(ui.actionOptions, SIGNAL( triggered() ), this, SLOT( showOptions() ) );
	connect(ui.actionRun_test_queries, SIGNAL( triggered() ), this, SLOT( runTestQueries() ));
	connect(ui.actionReconnect, SIGNAL( triggered() ), this, SLOT( reconnect() ));

	connect(ui.actionHelp, SIGNAL( triggered() ), this, SLOT( showHelp() ) );
	connect(ui.actionAbout, SIGNAL( triggered() ), this, SLOT( showAbout() ) );
	connect(ui.actionAboutQt, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) );

	connect(ui.minimapZoomSlider, SIGNAL( valueChanged(int) ), this, SLOT( adjustMinimapZoom(int) ) );
	adjustMinimapZoom(ui.minimapZoomSlider->value());

	// XXX: kludge... don't know how to do it in designer
	ui.diagramDock->setWidget(ui.diagramExplorer);
	// ui.objectDock->setWidget(ui.objectExplorer);

	ui.paletteDock->setWidget(ui.paletteToolbox);

	ui.propertyEditor->setModel(&propertyModel);
	ui.propertyEditor->verticalHeader()->hide();
	ui.propertyEditor->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
	ui.propertyEditor->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
	ui.propertyEditor->setItemDelegate(&delegate);

	ui.diagramExplorer->addAction(ui.actionDeleteFromDiagram);

	connect(ui.diagramExplorer, SIGNAL( clicked( const QModelIndex & ) ),
			&propertyModel, SLOT( setIndex( const QModelIndex & ) ) );

	show();

	QSettings settings("Tercom", "QReal");
	settings.beginGroup("MainWindow");
	resize(settings.value("size", QSize(1024, 800)).toSize());
	move(settings.value("pos", QPoint(0, 0)).toPoint());
	settings.endGroup();

	QApplication::processEvents();

	repoAddress = settings.value("repoAddress", "127.0.0.1").toString();
	repoPort = settings.value("repoPort", 6666).toInt();

	connDialog = new ConnectionDialog(repoAddress, repoPort);
	connect(connDialog, SIGNAL(dataAccepted(const QString&, const int)), this, SLOT(reconnectRepo(const QString&, const int)));

	connectRepo(&splash, repoAddress, repoPort);

	splash.close();
}

MainWindow::~MainWindow()
{
	delete connDialog;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	QSettings settings("Tercom", "QReal");
	settings.beginGroup("MainWindow");
	settings.setValue("size", size());
	settings.setValue("pos", pos());
	settings.endGroup();
	settings.setValue("repoAddress", repoAddress);
	settings.setValue("repoPort", repoPort);
	event->accept();
}

void MainWindow::adjustMinimapZoom(int zoom)
{
	ui.minimapView->resetMatrix();
	ui.minimapView->scale(0.01*zoom,0.01*zoom);
}

void MainWindow::connectRepo(QSplashScreen *splash, const QString &addr, const int port)
{
	closeRepo();
	qDebug() << "connecting..." << addr << port;
	model = new RealRepoModel(addr, port, this);
	if( model->getState() != QAbstractSocket::ConnectedState ){
		qDebug() << "repo model creation failed";
		if (splash != NULL)
			splash->close();
		QMessageBox::critical(0, tr("achtung!"), tr("cannot reach repo server at ")
			+ repoAddress + QString(":") + QString::number(repoPort)
			+ QString("\n")
			+ tr("make sure that it is running or use \"Tools/Setup repo server path\" ")
			+ tr("to select another server"));
		//qApp->exit();
		closeRepo();
		return;
	}
	ui.diagramExplorer->setModel(model);
	ui.diagramExplorer->setRootIndex(model->index(1,0,QModelIndex()));

	ui.objectExplorer->setModel(model);
//	ui.objectExplorer->setRowHidden(1,QModelIndex(),true);

	propertyModel.setSourceModel(model);

	ui.view->mvIface()->setModel(model);

	QModelIndex rootDiagram = model->createDefaultTopLevelItem();
	activateItemOrDiagram(rootDiagram);
	propertyModel.setIndex(rootDiagram);

	connect(ui.actionUndo, SIGNAL( triggered() ), model, SLOT( undo() ) );
	connect(ui.actionRedo, SIGNAL( triggered() ), model, SLOT( redo() ) );

	connect(ui.actionCommand_list, SIGNAL( triggered() ), model,
			SLOT( showCommandList() ));
}

void MainWindow::closeRepo()
{
	ui.diagramExplorer->setModel(0);
	ui.objectExplorer->setModel(0);
	propertyModel.setSourceModel(0);
	ui.view->mvIface()->setModel(0);

	if( model )
		delete model;

	model = 0;
}

void MainWindow::deleteFromExplorer()
{
	QModelIndex idx = ui.diagramExplorer->currentIndex();
	if (idx.isValid())
	{
		try
		{
			model->removeRow(idx.row(), idx.parent());
		}
		catch (QString e)
		{
			QMessageBox::warning(this, tr("Operation aborted"),
				tr("Repository can not delete this element."));
		}
	}
}

void MainWindow::deleteFromScene()
{
	foreach (QGraphicsItem *item, ui.view->scene()->selectedItems()) {
		if (UML::Element * elem = dynamic_cast < UML::Element * >(item))
			if (elem->index().isValid())
				model->removeRow(elem->index().row(), elem->index().parent());
	}
}

void MainWindow::deleteFromDiagram()
{
	if ( model ) {
		if (ui.diagramExplorer->hasFocus())
		{
			deleteFromExplorer();
		}
		else if (ui.view->hasFocus())
		{
			deleteFromScene();
		}
	}
}

void MainWindow::activateItemOrDiagram(const QModelIndex &idx)
{
	QModelIndex parent = idx.parent();

	/* Is level-one diagram? */
	if (parent == model->index(1,0,QModelIndex()))
	{
		/* activate this diagram */
		ui.view->mvIface()->setRootIndex(idx);
	}
	else
	{
		/* activate parent diagram */
		ui.view->mvIface()->setRootIndex(parent);
		/* select this item on diagram */
		ui.view->scene()->clearSelection();
		(dynamic_cast<EditorViewScene *>(ui.view->scene()))->getElemByModelIndex(idx)->setSelected(true);
	}
}

void MainWindow::sceneSelectionChanged()
{
	QList<QGraphicsItem*> graphicsItems =  ui.view->scene()->selectedItems();
	if (graphicsItems.size() == 1) {
		QGraphicsItem *item = graphicsItems[0];
		if (UML::Element *elem = dynamic_cast<UML::Element *>(item))
			if (elem->index().isValid()) {
				ui.diagramExplorer->setCurrentIndex(elem->index());
				propertyModel.setIndex(elem->index());
			}
	}
}

void MainWindow::print()
{
	QPrinter printer(QPrinter::HighResolution);
	QPrintDialog dialog(&printer, this);
	if (dialog.exec() == QDialog::Accepted) {
		QPainter painter(&printer);
//		QRect allScene = pieChart->mapFromScene(pieChart->scene()->sceneRect()).boundingRect();
		ui.view->scene()->render(&painter);
	}
}

void MainWindow::makeSvg()
{
	QSvgGenerator newSvg;

	QString fileName = QFileDialog::getSaveFileName(this);
	if (fileName.isEmpty())
		return;

	newSvg.setFileName(fileName);
	newSvg.setSize(QSize(800,600));

	QPainter painter(&newSvg);
	ui.view->scene()->render(&painter);
}

void MainWindow::showAbout()
{
	QMessageBox::about(this, tr("About QReal"),
		tr("This is <b>QReal</b><br>"
		"Just another CASE tool<br>"
		"<i>Burning heretics since 1724</i>"));
}

void MainWindow::showHelp()
{
	QMessageBox::warning(this, tr("Not implemented"),
			 tr("This feature is not implemented yet. "));
}

void MainWindow::showOptions()
{
	QReal::OptionsDialog dialog(ui.paletteToolbox->getSelectedTabs());
	dialog.exec();
	if (dialog.result() == QDialog::Accepted)
	{
		QVector<bool> selectedEditors = dialog.getSelectedEditors();
		ui.paletteToolbox->setEditors(selectedEditors);
	}
}

void MainWindow::runTestQueries() const
{
	RealRepoClient client(repoAddress, repoPort, 0);

	// Запросы к репозиторию писать здесь.

	qDebug() << "Done.";
}

void MainWindow::reconnect()
{
	qDebug() << "OLOLO!!!";
	connDialog->exec();
}

void MainWindow::reconnectRepo(const QString& addr, const int port)
{
	repoAddress = addr;
	repoPort = port;
	connectRepo(0, addr, port);
}