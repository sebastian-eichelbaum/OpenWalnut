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
    QWidget* filePage = setupFileTab();
    QWidget* modulPage = setupModuleTab();
    QWidget* helpPage = new QWidget();


    QTabWidget* menuTabWidget = new QTabWidget();
    menuTabWidget->addTab( filePage, QString( "File" ) );
    menuTabWidget->addTab( modulPage, QString( "Modul" ) );
    menuTabWidget->addTab( helpPage, QString( "Help" ) );

    this->addWidget( menuTabWidget );
    this->setAllowedAreas( Qt::TopToolBarArea | Qt::BottomToolBarArea );
}


QWidget* WQtRibbonMenu::setupFileTab()
{
    m_quitIcon.addPixmap( QPixmap( quit_xpm ) );
    m_saveIcon.addPixmap( QPixmap( disc_xpm ) );
    m_loadIcon.addPixmap( QPixmap( fileopen_xpm ) );

    QWidget* filePage = new QWidget();
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

    return filePage;
}


QWidget* WQtRibbonMenu::setupModuleTab()
{
    m_axiIcon.addPixmap( QPixmap( axial_xpm ) );
    m_corIcon.addPixmap( QPixmap( cor_xpm ) );
    m_sagIcon.addPixmap( QPixmap( sag_xpm ) );

    QWidget* modulPage = new QWidget();

    m_axiButton = new QPushButton( m_axiIcon, QString( "" ) );
    m_corButton = new QPushButton( m_corIcon, QString( "" ) );
    m_sagButton = new QPushButton( m_sagIcon, QString( "" ) );

    m_axiButton->setMaximumWidth( 24 );
    m_corButton->setMaximumWidth( 24 );
    m_sagButton->setMaximumWidth( 24 );

    m_axiButton->setCheckable( true );
    m_corButton->setCheckable( true );
    m_sagButton->setCheckable( true );

    m_axiButton->setChecked( true );
    m_corButton->setChecked( true );
    m_sagButton->setChecked( true );

    QHBoxLayout* modulPageLayout = new QHBoxLayout();
    modulPageLayout->addWidget( m_axiButton );
    modulPageLayout->addWidget( m_corButton );
    modulPageLayout->addWidget( m_sagButton );
    modulPageLayout->addStretch();

    modulPage->setLayout( modulPageLayout );


    return modulPage;
}

QPushButton* WQtRibbonMenu::getLoadButton()
{
    return m_loadButton;
}


QPushButton* WQtRibbonMenu::getQuitButton()
{
    return m_quitButton;
}


QPushButton* WQtRibbonMenu::getAxiButton()
{
    return m_axiButton;
}


QPushButton* WQtRibbonMenu::getCorButton()
{
    return m_corButton;
}


QPushButton* WQtRibbonMenu::getSagButton()
{
    return m_sagButton;
}
