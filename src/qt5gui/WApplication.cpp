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

#include <exception>

#include <QtGui>

#include "WMainWindow.h"

#include "WApplication.h"

#include "core/common/WException.h"

#define OPENWALNUT_BUGREPORTER_URL "http://www.openwalnut.org/projects/openwalnut/issues"

WApplication::WApplication( int &argc, char** argv, bool GUIenabled ) // NOLINT
    : QApplication( argc, argv, GUIenabled )
    , myMainWidget( 0 )
{
}

void WApplication::setMyMainWidget( QWidget* widget )
{
    myMainWidget = widget;
}

void WApplication::commitData( QSessionManager& manager ) // NOLINT
{
    if( !myMainWidget )
    {
        manager.release(); // no main window, nothing to store, yet
        return;
    }

    WMainWindow* mainWindow = dynamic_cast<WMainWindow*>( myMainWidget );
    if( !mainWindow )
    {
        manager.release(); // not our main class, can't store
        return;
    }

    QApplication::commitData( manager );
    if( manager.allowsInteraction() )
    {
        int ret =  QMessageBox::warning(
                myMainWidget,
                tr( "OpenWalnut" ),
                tr( "Save changes?" ),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );
        switch( ret )
        {
            case QMessageBox::Save:
                if( mainWindow->projectSaveAll() )
                {
                    // we want to save and saving was successful, we are ready!
                    manager.release();
                }
                else
                {
                    // if we want to save but saving failed, cancel action
                    manager.cancel();
                }
                break;
            case QMessageBox::Discard:
                break;
            case QMessageBox::Cancel:
            default:
                manager.cancel();
        }
    }
    else
    {
        // no interaction allowed, what should we do?
    }
}

bool WApplication::notify( QObject* object, QEvent* event )
{
    // Question: can we assume that WLogger is running here?
    // if so, we should log the message to the logger as well.
    bool retval = false;
    try
    {
        // do the default action, but catch exceptions
        retval = QApplication::notify( object, event );
    }
    catch( const WException &we )
    {
        QMessageBox msgBox( myMainWidget );
        msgBox.setIcon( QMessageBox::Critical );
        msgBox.setInformativeText( tr( "An uncaught exception occurred which may be due to a corrupt installation or a programming bug. "
                                       "Please check the openwalnut bug reporter for similar tickets and report the "
                                       "issue including the following text:<br><br><i>" )
                                  + we.what() +
                                        "</i><br><br>Please report to<br><a href=\""
                                        OPENWALNUT_BUGREPORTER_URL
                                        "\">" OPENWALNUT_BUGREPORTER_URL "</a>" );
        msgBox.setText( tr( "Uncaught Exception" ) );
        QPushButton* websiteButton = msgBox.addButton( tr( "Go to web site" ), QMessageBox::ActionRole );
        msgBox.setStandardButtons( QMessageBox::Ignore );
        msgBox.setEscapeButton( QMessageBox::Ignore );

        msgBox.exec();
        if( msgBox.clickedButton() == websiteButton )
        {
            /* bool success = */ QDesktopServices::openUrl( QUrl( OPENWALNUT_BUGREPORTER_URL ) );
        }
    }
    catch( const std::exception &se )
    {
        QMessageBox msgBox( myMainWidget );
        msgBox.setIcon( QMessageBox::Critical );
        msgBox.setInformativeText( tr( "An uncaught exception occurred which may be due to a corrupt installation or a programming bug. "
                                      "Please check the openwalnut bug reporter for similar tickets and report the "
                                      "issue including the following text:<br><br><i>" )
                                  + se.what() +
                                  "</i><br><br>Please report to<br><a href=\""
                                  OPENWALNUT_BUGREPORTER_URL
                                  "\">" OPENWALNUT_BUGREPORTER_URL "</a>" );
        msgBox.setText( tr( "Uncaught Exception" ) );
        QPushButton* websiteButton = msgBox.addButton( tr( "Go to web site" ), QMessageBox::ActionRole );
        msgBox.setStandardButtons( QMessageBox::Ignore );
        msgBox.setEscapeButton( QMessageBox::Ignore );

        msgBox.exec();
        if( msgBox.clickedButton() == websiteButton )
        {
            /* bool success = */ QDesktopServices::openUrl( QUrl( OPENWALNUT_BUGREPORTER_URL ) );
        }
    }
    return retval;
}
