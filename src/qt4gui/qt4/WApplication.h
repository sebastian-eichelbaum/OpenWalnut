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

#ifndef WAPPLICATION_H
#define WAPPLICATION_H

#include <QtGui/QApplication>

class WMainWindow;

/**
 * Overloaded base class for our application that has initial handling of
 * session data and catches uncaught exceptions.
 */
class WApplication : public QApplication
{
    Q_OBJECT
public:
    /** default constructor, see QApplication */
    WApplication( int argc, char** argv, bool GUIenabled = true );

    /** manage save dialogs when the session manager asks us to
     * take care of our data.
     * This is an overloaded function from QT.
     */
    virtual void commitData( QSessionManager& manager ); // NOLINT
    
    /**
     * store the main widget for error reporting and session management
     */
    void setMyMainWidget( QWidget* widget );

    /**
     * Overloaded to catch uncaught exceptions in event handlers and displays a bug-warning.
     */
    virtual bool notify( QObject* receiver, QEvent* e );
    
protected:
    QWidget* myMainWidget;
};

#endif  // WAPPLICATION_H
