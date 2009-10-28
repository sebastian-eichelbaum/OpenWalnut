//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
// For more information see http://www.openwalnut.org/copying
//
// This file is part of OpenWalnut.
//
// OpenWalnut is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenWalnut is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#ifndef WQTDATASETBROWSER_H
#define WQTDATASETBROWSER_H

#include <string>
#include <vector>

#include <QtGui/QDockWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

#include "WQtDSBWidget.h"
#include "WQtSubjectTreeItem.h"
#include "WQtTreeWidget.h"

/**
 * container widget for a tree widget with context menu and some control widgets
 */
class WQtDatasetBrowser : public QDockWidget
{
    Q_OBJECT

public:
    /**
     * Default constructor.
     *
     * \param parent Parent widget.
     *
     * \return
     */
    explicit WQtDatasetBrowser( QWidget* parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WQtDatasetBrowser();

    /**
     * adds a page to the context widget
     */
    void addTabWidgetContent( WQtDSBWidget* content );

    /**
     * adds a subject entry to the tree widget
     */
    WQtSubjectTreeItem* addSubject( std::string name );

    /**
     * adds a dataset entry to any given subject in the tree widget
     */
    WQtDatasetTreeItem* addDataset( boost::shared_ptr< WModule > module, int subjectId = 0 );

    /**
     * returns a vector of pointers to the loaded datasets for a given subject
     */
    std::vector< boost::shared_ptr< WModule > >getDataSetList( int subjectId );


    /**
     * helper funtion to connect all qt widgets with their functions
     */
    void connectSlots();

public slots:
    void slotSetIntProperty( QString name, int value );
    void slotSetBoolProperty( QString name, bool value );
    void slotSetStringProperty( QString name, QString value );


protected:


private:
    WQtTreeWidget* m_treeWidget;
    QTabWidget* m_tabWidget;
    QPushButton* m_downButton;
    QPushButton* m_upButton;

    QWidget* m_tab1;
    QWidget* m_tab2;
    QWidget* m_tab3;
    QWidget* m_panel;
    QVBoxLayout* m_layout;

private slots:
    /**
     *
     */
    void selectTreeItem();

    /**
     *
     */
    void changeTreeItem();

    /**
     * change order of items, move currently selected item down
     */
    void moveTreeItemDown();

    /**
     * change order of items, move currently selected item up
     */
    void moveTreeItemUp();


signals:
    void dataSetBrowserEvent( QString name, bool value );
};

#endif  // WQTDATASETBROWSER_H
