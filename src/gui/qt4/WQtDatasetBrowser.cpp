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

#include <string>

#include "WQtDatasetBrowser.h"

WQtDatasetBrowser::WQtDatasetBrowser( QWidget* parent )
    : QDockWidget( parent )
{
    m_panel = new QWidget( this );
    m_treeWidget = new QTreeWidget( m_panel );

    m_treeWidget->setHeaderLabel( QString( "Dataset Browser" ) );
    m_tabWidget = new QTabWidget( m_panel );

    m_tab1 = new QWidget();
    m_tab2 = new QWidget();
    m_tab3 = new QWidget();

    m_tabWidget->addTab( m_tab1, QString( "tab1" ) );
    m_tabWidget->addTab( m_tab2, QString( "tab2" ) );
    m_tabWidget->addTab( m_tab3, QString( "tab3" ) );

    m_layout = new QVBoxLayout();

    m_layout->addWidget( m_treeWidget );
    m_layout->addWidget( m_tabWidget );

    m_panel->setLayout( m_layout );

    this->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    this->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    this->setWidget( m_panel );
}

WQtDatasetBrowser::~WQtDatasetBrowser()
{
}


WQtSubjectTreeItem* WQtDatasetBrowser::addSubject( std::string name )
{
    WQtSubjectTreeItem* subject = new WQtSubjectTreeItem( m_treeWidget );
    subject->setText( 0, QString( name.c_str() ) );

    return subject;
}


WQtDatasetTreeItem* WQtDatasetBrowser::addDataset( int subjectId, std::string name )
{
    WQtSubjectTreeItem* subject = ( WQtSubjectTreeItem* )m_treeWidget->topLevelItem( subjectId );
    return subject->addDatasetItem( name );
}
