#ifndef BCMAINWINDOW_H
#define BCMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

#include "../icons/icons.h"

class BCMainWindow
{
public:
	QAction *actionLoad;
	QAction *actionSave;
	QAction *actionPreferences;
	QAction *actionExit;
	QAction *actionAbout_BrainCognize;
	QWidget *centralwidget;
	QMenuBar *menubar;
	QMenu *menuFile;
	QMenu *menuHelp;
//	QStatusBar *statusBar;
	QToolBar *toolBar;


	void setupGUI(QMainWindow *MainWindow)
	{
	    QIcon mainWindowIcon;
	    QIcon quitIcon;
	    QIcon saveIcon;
	    QIcon loadIcon;
	    QIcon aboutIcon;


	    mainWindowIcon.addPixmap( QPixmap( logoIcon_xpm ) );
	    quitIcon.addPixmap( QPixmap( quit_xpm ) );
	    saveIcon.addPixmap( QPixmap( disc_xpm ) );
	    loadIcon.addPixmap( QPixmap( fileopen_xpm ) );
	    aboutIcon.addPixmap( QPixmap( logoIcon_xpm ) );

		if (MainWindow->objectName().isEmpty())
			MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
		MainWindow->resize(946, 632);
		MainWindow->setWindowIcon(mainWindowIcon);

		actionLoad = new QAction(MainWindow);
		actionLoad->setObjectName(QString::fromUtf8("actionLoad"));
	    actionLoad->setIcon(loadIcon);
		actionLoad->setText(QApplication::translate("MainWindow", "Load", 0, QApplication::UnicodeUTF8));
		actionLoad->setShortcut(QApplication::translate("MainWindow", "Ctrl+L", 0, QApplication::UnicodeUTF8));

		actionSave = new QAction(MainWindow);
		actionSave->setObjectName(QString::fromUtf8("actionSave"));
	    actionSave->setIcon(saveIcon);
		actionSave->setText(QApplication::translate("MainWindow", "Save", 0, QApplication::UnicodeUTF8));

		actionPreferences = new QAction(MainWindow);
		actionPreferences->setObjectName(QString::fromUtf8("actionPreferences"));
		actionPreferences->setText(QApplication::translate("MainWindow", "Preferences", 0, QApplication::UnicodeUTF8));

		actionExit = new QAction(MainWindow);
		actionExit->setObjectName(QString::fromUtf8("actionExit"));
	    actionExit->setIcon(quitIcon);
		actionExit->setText(QApplication::translate("MainWindow", "E&xit", 0, QApplication::UnicodeUTF8));
		actionExit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));

		actionAbout_BrainCognize = new QAction(MainWindow);
		actionAbout_BrainCognize->setObjectName(QString::fromUtf8("actionAbout_BrainCognize"));
	    actionAbout_BrainCognize->setIcon(aboutIcon);
		actionAbout_BrainCognize->setText(QApplication::translate("MainWindow", "About BrainCognize", 0, QApplication::UnicodeUTF8));

		centralwidget = new QWidget(MainWindow);
		centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
		MainWindow->setCentralWidget(centralwidget);

		menubar = new QMenuBar(MainWindow);
		menubar->setObjectName(QString::fromUtf8("menubar"));
		menubar->setGeometry(QRect(0, 0, 946, 24));

		menuFile = new QMenu(menubar);
		menuFile->setObjectName(QString::fromUtf8("menuFile"));

		menuHelp = new QMenu(menubar);
		menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
		MainWindow->setMenuBar(menubar);

		toolBar = new QToolBar(MainWindow);
		toolBar->setObjectName(QString::fromUtf8("toolBar"));
		MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

		menubar->addAction(menuFile->menuAction());
		menubar->addAction(menuHelp->menuAction());

		menuFile->addAction(actionLoad);
		menuFile->addAction(actionSave);
		menuFile->addSeparator();
		menuFile->addAction(actionPreferences);
		menuFile->addSeparator();
		menuFile->addAction(actionExit);
		menuFile->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));

		menuHelp->addAction(actionAbout_BrainCognize);
		menuHelp->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));
		toolBar->addSeparator();

		MainWindow->setWindowTitle( QApplication::translate("MainWindow", "BrainCognize", 0, QApplication::UnicodeUTF8) );
		toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0, QApplication::UnicodeUTF8));

		QObject::connect(actionExit, SIGNAL(activated()), MainWindow, SLOT(close()));

		QMetaObject::connectSlotsByName(MainWindow);
	} // setupUi

};

#endif // BCMAINWINDOW_H
