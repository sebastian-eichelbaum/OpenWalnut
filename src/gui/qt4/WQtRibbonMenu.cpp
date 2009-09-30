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

#include <QtGui/QPushButton>
#include <QtGui/QTabWidget>
#include <QtGui/QHBoxLayout>

#include "WQtRibbonMenu.h"

#include "../icons/WIcons.h"

WQtRibbonMenu::WQtRibbonMenu( QWidget* parent )
    : QToolBar( parent )
{
    setup();
}

WQtRibbonMenu::~WQtRibbonMenu()
{
}

void WQtRibbonMenu::setup()
{
    m_quitIcon.addPixmap( QPixmap( quit_xpm ) );
    m_saveIcon.addPixmap( QPixmap( disc_xpm ) );
    m_loadIcon.addPixmap( QPixmap( fileopen_xpm ) );

    QWidget* filePage = new QWidget();
    QWidget* modulPage = new QWidget();
    QWidget* helpPage = new QWidget();

    m_loadButton = new QPushButton( m_loadIcon, QString( "load" ) );
    m_saveButton = new QPushButton( m_saveIcon, QString( "save" ) );
    m_quitButton = new QPushButton( m_quitIcon, QString( "exit" ) );

    m_loadButton->setMaximumWidth( 50 );
    m_saveButton->setMaximumWidth( 50 );
    m_quitButton->setMaximumWidth( 50 );

    QHBoxLayout* filePageLayout = new QHBoxLayout();
    filePageLayout->addWidget( m_loadButton );
    filePageLayout->addWidget( m_saveButton );
    filePageLayout->addWidget( m_quitButton );
    filePageLayout->addStretch();

    filePage->setLayout( filePageLayout );

    QTabWidget* menuTabWidget = new QTabWidget();
    menuTabWidget->addTab( filePage, QString( "File" ) );
    menuTabWidget->addTab( modulPage, QString( "Modul" ) );
    menuTabWidget->addTab( helpPage, QString( "Help" ) );

    this->addWidget( menuTabWidget );
    this->setAllowedAreas( Qt::TopToolBarArea | Qt::BottomToolBarArea );
}

QPushButton* WQtRibbonMenu::getLoadButton()
{
    return m_loadButton;
}


QPushButton* WQtRibbonMenu::getQuitButton()
{
    return m_quitButton;
}
