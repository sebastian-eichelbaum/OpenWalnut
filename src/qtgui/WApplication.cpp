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
#include "core/common/WLogger.h"

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

bool WApplication::notify( QObject* object, QEvent* event )
{
    bool retval = false;
    QString exception_msg;
    try // do the default action, but catch exceptions
    {
        retval = QApplication::notify( object, event );
    }
    catch( const WException &we )
    {
        exception_msg = QString( we.what() );
    }
    catch( const std::exception &se )
    {
        exception_msg = QString( se.what() );
    }

    if( !exception_msg.isEmpty() )
    {
        QMessageBox msgBox( myMainWidget );
        msgBox.setIcon( QMessageBox::Critical );
        QString info( "An uncaught exception occurred which may be due to a corrupt installation or a programming bug. "
                      "Please check the openwalnut bug reporter for similar tickets and report the issue including the "
                      "following text: <br><br><i>" + exception_msg + "</i><br><br> Please report to: <br><a href=\""
                      OPENWALNUT_BUGREPORTER_URL "\">" OPENWALNUT_BUGREPORTER_URL "</a>" );
        msgBox.setInformativeText( tr( info.toStdString().c_str() ) );
        QPushButton* websiteButton = msgBox.addButton( tr( "Go to web site" ), QMessageBox::ActionRole );
        msgBox.setStandardButtons( QMessageBox::Ignore );
        msgBox.setEscapeButton( QMessageBox::Ignore );
        msgBox.exec();
        if( msgBox.clickedButton() == websiteButton )
        {
            QDesktopServices::openUrl( QUrl( OPENWALNUT_BUGREPORTER_URL ) ); // we do not have use for the bool return value
        }

        // also notify the logger, as it is setup, before WApplication::exec is called
        wlog::error( "WApplication" ) << info.remove( QRegExp( "<[^>]*>" ) ).toStdString();
    }

    return retval;
}
