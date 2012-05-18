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

#include "core/common/WLogger.h"

#include "WQtMenuFiltered.h"
#include "WQtMenuFiltered.moc"

WQtMenuFiltered::WQtMenuFiltered( QWidget* parent ):
    QMenu( parent )
{
    WQtMenuFiltered::setupFilter( this );
}

WQtMenuFiltered::WQtMenuFiltered( const QString& title, QWidget* parent ):
    QMenu( title, parent )
{
    WQtMenuFiltered::setupFilter( this );
}

WQtMenuFiltered::~WQtMenuFiltered()
{
    // cleanup
}

void WQtMenuFiltered::setupFilter( WQtMenuFiltered* to )
{
    // the widget contains the label and text filter box
    QWidget* widget = new QWidget( to );
    QHBoxLayout* layout = new QHBoxLayout( widget );
    widget->setLayout( layout );

    QLabel* label = new QLabel( "Filter:", widget );
    to->m_edit = new QLineEdit( widget );
    layout->addWidget( label );
    layout->addWidget( to->m_edit );

    // Add the widget as QWidgetAction
    QWidgetAction* action = new QWidgetAction( to );
    action->setDefaultWidget( widget );
    to->addAction( action );

    // give focus by default
    to->m_edit->setFocus();

    // update filter if text changes
    connect( to->m_edit, SIGNAL( textChanged( const QString& ) ), to, SLOT( filterUpdate() ) );
}

void WQtMenuFiltered::keyPressEvent( QKeyEvent* e )
{
    // Fake default QMenu feeling for these keys:
    // Tab and Cursor
    // Enter
    // Escape
    // But handle keys a-z,A-Z
    if( ( ( e->key() >= Qt::Key_A ) && ( e->key() <= Qt::Key_Z ) ) ||
        ( ( e->key() >= Qt::Key_0 ) && ( e->key() <= Qt::Key_9 ) ) ||
        ( e->key() == Qt::Key_Underscore ) ||
        ( e->key() == Qt::Key_Space ) ||
        ( e->key() == Qt::Key_Minus ) ||
        ( e->key() == Qt::Key_Plus )
      )
    {
        m_edit->setText( m_edit->text() + e->text() );
    }
    else if( ( e->key() == Qt::Key_Backspace ) )
    {
        m_edit->backspace();
    }
    else if( ( e->key() == Qt::Key_Escape ) )    // if escape and currently a filter is active: remove filter
    {
        if( m_edit->text() != "" )
        {
            resetFilter();
        }
        else
        {
            QMenu::keyPressEvent( e );
        }
    }
    else
    {
        QMenu::keyPressEvent( e );
    }
}

void WQtMenuFiltered::hideEvent( QHideEvent* /* e */ )
{
    resetFilter();
}

void WQtMenuFiltered::filterUpdate()
{
    QString filter = m_edit->text();

    // NOTE: we ignore the first element. It is the filter widget.

    // Important: we need to use a COPY of the action list as Qt sometimes removes actions there. I do not fully understand why.
    typedef QList< QAction* > Actions;
    Actions allActions = this->actions();
    size_t nbLeft = 0;  // keep track how many items are left
    QAction* lastVisibleAction = NULL;
    for( Actions::const_iterator a = allActions.begin() + 1; a != allActions.end(); ++a )
    {
        // strange but I had the issue that there was a NULL item in this list.
        if( ( *a ) != NULL )
        {
            QString s = ( *a )->text();
            // match value against filter
            if( s.contains( filter, Qt::CaseInsensitive ) )
            {
                ( *a )->setVisible( true );
                nbLeft++;
                lastVisibleAction = *a;
            }
            else
            {
                ( *a )->setVisible( false );
            }
        }
    }

    // if only one item is left, select it automatically
    if( nbLeft == 1 )
    {
        setActiveAction( lastVisibleAction );
    }
}

void WQtMenuFiltered::resetFilter()
{
    // reset text filter. This triggers filterUpdate!
    m_edit->setText( "" );
}

