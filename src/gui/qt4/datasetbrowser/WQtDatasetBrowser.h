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

#include <QtGui/QDockWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

#include "WQtDSBWidget.h"
#include "WQtSubjectTreeItem.h"
#include "WQtTreeWidget.h"
/**
 * TODO(schurade): Document this!
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
     *
     */
    void addTabWidgetContent( WQtDSBWidget* content );

    /**
     *
     */
    WQtSubjectTreeItem* addSubject( std::string name );

    /**
     *
     */
    WQtDatasetTreeItem* addDataset( int subjectId, std::string name );

    /**
     *
     */
    void connectSlots();

    /**
     *
     */
    void testBoostSignal( int test );

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
};

#endif  // WQTDATASETBROWSER_H
