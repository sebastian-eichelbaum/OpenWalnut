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

#include <QtGui/QAction>
#include <QtGui/QLabel>

#include "../WMainWindow.h"

#include "WQtCommandPrompt.h"

#include "WQtCommandPromptToolbar.h"
#include "WQtCommandPromptToolbar.moc"

WQtCommandPromptToolbar::WQtCommandPromptToolbar( const QString& title, WMainWindow* parent ):
    QToolBar( title, parent ),
    m_mainWindow( parent )
{
    setObjectName( title );
    this->setAllowedAreas( Qt::TopToolBarArea | Qt::BottomToolBarArea );
    setMinimumWidth( 50 );
    setMinimumHeight( 20 );
    setVisible( false );

    // toggle it using ":"
    QList< QKeySequence > commandPromptShortcut;
    commandPromptShortcut.append( QKeySequence( Qt::Key_Colon ) );
    toggleViewAction()->setShortcuts( commandPromptShortcut );

    QLabel* label = new QLabel( this );
    label->setText( "<b>:</b>" );
    label->setStyleSheet( "background: #080808;" );
    addWidget( label );

    m_prompt = new WQtCommandPrompt( this );
    addWidget( m_prompt );

    // some nice style
    // TODO(ebaum): make this configurable
    setStyleSheet( "background: #080808;"
                   "border: 2px solid #080808;"
                   "margin: 0px;"
                   "padding: 0px;"
                   "spacing: 0;"
                   "color: white;"
    );

    // if the toolbar is triggered:
    connect( toggleViewAction(), SIGNAL( triggered( bool ) ), this, SLOT( show() ) );
    connect( m_prompt, SIGNAL( done() ), this, SLOT( exit() ) );
}

WQtCommandPromptToolbar::~WQtCommandPromptToolbar()
{
    // cleanup
}

void WQtCommandPromptToolbar::show()
{
    setVisible( true );
    m_prompt->setFocus();
}

void WQtCommandPromptToolbar::exit()
{
    setVisible( false );
}

