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

#include <iostream>

#include <QtGui/QAction>
#include <QtGui/QLineEdit>
#include <QtGui/QKeyEvent>

#include "../WMainWindow.h"

#include "WQtCommandPrompt.h"
#include "WQtCommandPrompt.moc"

WQtCommandPrompt::WQtCommandPrompt( QWidget* parent ):
    QLineEdit( parent )
{
    // some nice style
    // TODO(ebaum): make this configurable
    setStyleSheet( "color: white;"
                   "border: 0px solid gray;"
                   "border-radius: 0px;"
                   "border-color: #080808;"
                   "padding: 0 0px;"
                   "background: #080808;"
                   "selection-background-color: gray;" );

    setText( "Not yet implemented. But as you tried the command prompt I assume you are a cool VIM user ;-)." );
    connect( this, SIGNAL( returnPressed() ), this, SLOT( commit() ) );
}

WQtCommandPrompt::~WQtCommandPrompt()
{
    // cleanup
}

void WQtCommandPrompt::abort()
{
    setText( "" );
    emit done();
}

void WQtCommandPrompt::commit()
{
    setText( "" );
    emit done();
}

void WQtCommandPrompt::keyPressEvent( QKeyEvent* event )
{
    QLineEdit::keyPressEvent( event );
}

bool WQtCommandPrompt::event( QEvent* event )
{
    // we need to override this to be able to grab the escape key
    if( event->type() == QEvent::KeyRelease )
    {
        QKeyEvent* keyEvent = dynamic_cast< QKeyEvent* >( event );

        // escape quits the prompt
        if( keyEvent && ( keyEvent->key() == Qt::Key_Escape ) )
        {
            abort();
            event->accept();
            return true;
        }
    }

    return QLineEdit::event( event );
}

