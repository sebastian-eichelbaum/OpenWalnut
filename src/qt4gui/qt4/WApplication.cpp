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

void WApplication::commitData( QSessionManager& manager )
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
                manager.release();
                mainWindow->projectSaveAll();
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

