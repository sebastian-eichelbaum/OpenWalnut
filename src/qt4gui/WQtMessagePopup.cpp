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

#include <algorithm>

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QCloseEvent>
#include <QtGui/QHideEvent>
#include <QtGui/QShowEvent>
#include <QtGui/QMessageBox>

#include "core/common/WLogger.h"

#include "guiElements/WScaleLabel.h"

#include "WMainWindow.h"
#include "WIconManager.h"

#include "WQtMessagePopup.h"
#include "WQtMessagePopup.moc"

#define OUTERMARGIN 10
#define BORDERWIDTH 2
#define CONTENTHEIGHT 16
#define MAXWIDTH 600

WQtMessagePopup::WQtMessagePopup( QWidget* parent, const QString& title, const QString& message, MessageType type ):
    QDialog( parent, Qt::Popup | Qt::FramelessWindowHint ),
    m_title( title ),
    m_message( message ),
    m_type( type ),
    m_autoClose( true ),
    m_autoMove( true )
{
    setAutoClose( true );

    setWindowRole( "MessagePopup" );

    // these settings seem to be ignored somehow
    setWindowModality( Qt::NonModal );

    // determine a width and height for the popup
    unsigned int w = std::min( parent->width() - ( 2 * OUTERMARGIN ), MAXWIDTH );
    unsigned int h = CONTENTHEIGHT + BORDERWIDTH + BORDERWIDTH;

    // change size of popup
    resize( w, h );

    QString borderColor = "red";
    QString titlePrefix = "";

    switch( m_type )
    {
        case LL_ERROR:
            borderColor = "#f44141";
            titlePrefix = "Error: ";
            break;
        case LL_WARNING:
            borderColor = "#ffa200";
            titlePrefix = "Warning: ";
            break;
        case LL_INFO:
            borderColor = "#40aca0";
            titlePrefix = "Info: ";
            break;
        case LL_DEBUG:
        default:
            borderColor = "#7d729c";
            titlePrefix = "Debug: ";
            break;
    }

    // setup widget
    setStyleSheet(
            "QDialog#popupDialog{"
            "background: #222222;"
            "border-style: solid;"
            "border-width: " + QString::number( BORDERWIDTH ) + "px;"
            "border-color: " + borderColor + ";"
            "}"
            "QWidget#popupDialogTitle{"
            "background: " + borderColor + ";"
            "color: white;"
            "font-weight: bold;"
            "}"
            "QLabel#popupDialogTitle{"
            "background: " + borderColor + ";"
            "color: white;"
            "font-weight: bold;"
            "}"
            "QLabel#popupDialogMessage{"
            "background: #222222;"
            "color: white;"
            "}"
            "QPushButton#popupDialogButton{"
            "border-style: none;"
            "background: " + borderColor + ";"
            "color: white;"
            "}"
            );

    setContentsMargins( 0, 0, 0, 0 );

    QHBoxLayout* topLayout = new QHBoxLayout( this );
    topLayout->setContentsMargins( 0, 0, 0, 0 );
    // this is the layout of some widget
    QWidget* titleWidget = new QWidget();
    titleWidget->setFixedHeight( CONTENTHEIGHT + BORDERWIDTH + BORDERWIDTH );
    titleWidget->setContentsMargins( 5, 0, 0, 0 );
    titleWidget->setLayout( topLayout );

    WScaleLabel* titleLabel = new WScaleLabel( this );
    titleLabel->setText( titlePrefix + title );
    topLayout->addWidget( titleLabel );

    QPushButton* detailsBtn = new QPushButton( "", this );
    detailsBtn->setContentsMargins( 0, 0, 0, 0 );
    detailsBtn->setIcon( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "popup_more" ) );
    detailsBtn->setFixedWidth( CONTENTHEIGHT );
    detailsBtn->setFixedHeight( CONTENTHEIGHT );
    detailsBtn->setToolTip( "Show complete message" );
    topLayout->addWidget( detailsBtn );
    connect( detailsBtn, SIGNAL( released() ), this, SLOT( showMessage() ) );

    m_closeBtn = new QPushButton( "", this );
    m_closeBtn->setContentsMargins( 0, 0, 0, 0 );
    m_closeBtn->setIcon( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "popup_close" ) );
    m_closeBtn->setFixedWidth( CONTENTHEIGHT );
    m_closeBtn->setFixedHeight( CONTENTHEIGHT );
    m_closeBtn->setToolTip( "Close this message" );
    topLayout->addWidget( m_closeBtn );
    connect( m_closeBtn, SIGNAL( released() ), this, SLOT( closePopup() ) );

    QVBoxLayout* popupLayout = new QVBoxLayout( this );
    popupLayout->setSpacing( 0 );
    popupLayout->setContentsMargins( BORDERWIDTH, 0, BORDERWIDTH, BORDERWIDTH );

    WScaleLabel* messageLabel = new WScaleLabel( this );
    messageLabel->setText( message );
    messageLabel->setContentsMargins( BORDERWIDTH, 0, 0, BORDERWIDTH );
    popupLayout->addWidget( titleWidget );
    popupLayout->addWidget( messageLabel );

    setLayout( popupLayout );

    // set the names. Needed for the stylesheet to work
    setObjectName( "popupDialog" );
    titleLabel->setObjectName( "popupDialogTitle" );
    titleWidget->setObjectName( "popupDialogTitle" );
    messageLabel->setObjectName( "popupDialogMessage" );
    m_closeBtn->setObjectName( "popupDialogButton" );
    detailsBtn->setObjectName( "popupDialogButton" );
}

void WQtMessagePopup::showEvent( QShowEvent* event )
{
    // move widget to correct position
    if( m_autoMove )
    {
        // get top left corner
        QPoint p = parentWidget()->mapToGlobal( QPoint( parentWidget()->width() / 2, parentWidget()->height() ) );

        // set position, include margins
        move( p.x() - width() / 2 - OUTERMARGIN / 2, p.y() - height()- OUTERMARGIN );
    }

    QDialog::showEvent( event );
}

WQtMessagePopup::~WQtMessagePopup()
{
    // cleanup
}

void WQtMessagePopup::showMessage()
{
    if( m_autoClose )
    {
        closePopup();
    }
    switch( m_type )
    {
        case LL_ERROR:
            QMessageBox::critical( this, m_title, m_message );
            break;
        case LL_WARNING:
            QMessageBox::warning( this, m_title, m_message );
            break;
        case LL_INFO:
        case LL_DEBUG:
        default:
            QMessageBox::information( this, m_title, m_message );
            break;
    }
}

void WQtMessagePopup::setAutoClose( bool autoClose )
{
    m_autoClose = autoClose;

    // use popup type of window if auto close is enabled
    if( autoClose )
    {
        setWindowFlags( Qt::Popup );
    }
    else
    {
        setWindowFlags( Qt::Widget );
    }
}

void WQtMessagePopup::closePopup()
{
    emit onClose( this );
    close();
}

void WQtMessagePopup::setShowCloseButton( bool showCloseButton )
{
    m_closeBtn->setHidden( !showCloseButton );
}

void WQtMessagePopup::setAutoPosition( bool autoPosition )
{
    m_autoMove = autoPosition;
}

WQtMessagePopup::MessageType WQtMessagePopup::getType() const
{
    return m_type;
}
