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

#include "WMainWindow.h"

#include "WQtToolBarBase.h"

WQtToolBarBase::WQtToolBarBase( const QString & title, WMainWindow* parent ):
    QToolBar( title, parent ),
    m_mainWindow( parent )
{
    setObjectName( title );

    setAllowedAreas( Qt::AllToolBarAreas );

    setMinimumWidth( 60 );
    setMinimumHeight( 40 );

    // Set the style of the toolbar
    setToolButtonStyle( static_cast< Qt::ToolButtonStyle >( m_mainWindow->getSettings().value( "qt4gui/toolbarStyle",
                                                                                         Qt::ToolButtonIconOnly ).toInt() ) );
}

WQtToolBarBase::~WQtToolBarBase()
{
    // cleanup
}

QMenu* WQtToolBarBase::getStyleMenu( QString title ) const
{
    QMenu* tbStyleMenu = new QMenu( title );

    // create radio-button like behaviour
    QActionGroup* tbStyleMenuGroup = new QActionGroup( tbStyleMenu );
    QAction* tbStyleMenuActionIconOnly = new QAction( "Icon Only", tbStyleMenuGroup );
    QAction* tbStyleMenuActionTextOnly = new QAction( "Text Only", tbStyleMenuGroup );
    QAction* tbStyleMenuActionTextBesidesIcon = new QAction( "Text besides Icon", tbStyleMenuGroup );
    QAction* tbStyleMenuActionTextUnderIcon = new QAction( "Text under Icon", tbStyleMenuGroup );
    QAction* tbStyleMenuActionStyle = new QAction( "Follow Style", tbStyleMenuGroup );

    tbStyleMenuActionIconOnly->setCheckable( true );
    tbStyleMenuActionTextOnly->setCheckable( true );
    tbStyleMenuActionTextBesidesIcon->setCheckable( true );
    tbStyleMenuActionTextUnderIcon->setCheckable( true );
    tbStyleMenuActionStyle->setCheckable( true );

    tbStyleMenuActionIconOnly->setActionGroup( tbStyleMenuGroup );
    tbStyleMenuActionTextOnly->setActionGroup( tbStyleMenuGroup );
    tbStyleMenuActionTextBesidesIcon->setActionGroup( tbStyleMenuGroup );
    tbStyleMenuActionTextUnderIcon->setActionGroup( tbStyleMenuGroup );
    tbStyleMenuActionStyle->setActionGroup( tbStyleMenuGroup );

    // set stored default
    int style = m_mainWindow->getSettings().value( "qt4gui/toolbarStyle", Qt::ToolButtonIconOnly ).toInt();
    switch( style )
    {
        case Qt::ToolButtonIconOnly:
            tbStyleMenuActionIconOnly->setChecked( true );
            break;
        case Qt::ToolButtonTextOnly:
            tbStyleMenuActionTextOnly->setChecked( true );
            break;
        case Qt::ToolButtonTextBesideIcon:
            tbStyleMenuActionTextBesidesIcon->setChecked( true );
            break;
        case Qt::ToolButtonTextUnderIcon:
            tbStyleMenuActionTextUnderIcon->setChecked( true );
            break;
        case Qt::ToolButtonFollowStyle:
            tbStyleMenuActionStyle->setChecked( true );
            break;
    }

    tbStyleMenu->addAction( tbStyleMenuActionIconOnly );
    tbStyleMenu->addAction( tbStyleMenuActionTextOnly );
    tbStyleMenu->addAction( tbStyleMenuActionTextBesidesIcon );
    tbStyleMenu->addAction( tbStyleMenuActionTextUnderIcon );
    tbStyleMenu->addAction( tbStyleMenuActionStyle );

    return tbStyleMenu;
}

