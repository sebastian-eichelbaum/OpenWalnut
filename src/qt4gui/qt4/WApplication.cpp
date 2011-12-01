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

#include <QtGui/QtGui>

#include "WMainWindow.h"

#include "WApplication.h"

WApplication::WApplication( int argc, char** argv, bool GUIenabled )
    : QApplication( argc, argv, GUIenabled ),
    mainWindow( 0 )
{
}

void WApplication::setMainWindow( WMainWindow* window )
{
    mainWindow =  window;
}

void WApplication::commitData( QSessionManager& manager ) // NOLINT
{
    if ( manager.allowsInteraction() )
    {
        int ret =  QMessageBox::warning(
                mainWindow,
                tr( "OpenWalnut" ),
                tr( "Save changes?" ),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );
        switch ( ret )
        {
            case QMessageBox::Save:
                if ( mainWindow->projectSaveAll() )
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

