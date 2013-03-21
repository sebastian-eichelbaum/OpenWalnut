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

#include <cmath>
#include <string>


#include <QtGui/QColorDialog>
#include <QtGui/QDropEvent>
#include <QtGui/QToolButton>

#include "core/common/WLogger.h"
#include "core/common/WPropertyVariable.h"
#include "../WGuiConsts.h"
#include "../WQt4Gui.h"
#include "../WMainWindow.h"

#include "WPropertyColorWidget.h"
#include "WPropertyColorWidget.moc"

WPropertyColorWidget::WPropertyColorWidget( WPropColor property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_colorProperty( property ),
    m_widget( &m_parameterWidgets ),
    m_layout(),
    m_asText( &m_informationWidgets ),
    m_infoLayout( &m_informationWidgets )
{
    // initialize members
    m_parameterWidgets.setLayout( &m_layout );

    // layout both against each other
    m_layout.addWidget( &m_widget );
    m_layout.setMargin( WGLOBAL_MARGIN );
    m_layout.setSpacing( WGLOBAL_SPACING );

    // Information Output ( Property Purpose = PV_PURPOSE_INFORMATION )
    m_infoLayout.addWidget( &m_asText );
    m_infoLayout.setMargin( WGLOBAL_MARGIN );
    m_infoLayout.setSpacing( WGLOBAL_SPACING );
    m_informationWidgets.setLayout( &m_infoLayout );

    QHBoxLayout* wLayout = new QHBoxLayout( &m_widget );
    wLayout->setContentsMargins( 0, 0, 0, 0 );
    wLayout->setSpacing( 0 );

    m_colButton = new QToolButton( this );
    m_colorPickerAction = new QAction( WQt4Gui::getMainWindow()->getIconManager()->getIcon( "colorwheel" ), "Select Color", this );
    connect( m_colorPickerAction, SIGNAL( triggered( bool ) ), this, SLOT( buttonClicked() ) );
    m_colButton->setDefaultAction( m_colorPickerAction );
    m_colButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
    m_colButton->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );

    m_colPanel = new QPushButton( &m_widget );
    m_colPanel->setSizePolicy( QSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred ) );
    m_colPanel->setMinimumSize( 24, 24 );
    connect( m_colPanel, SIGNAL( clicked( bool ) ), this, SLOT( buttonClicked() ) );

    wLayout->addWidget( m_colPanel );
    wLayout->addWidget( m_colButton );

    // accept drag and drop
    setAcceptDrops( true );

    // set the initial values
    update();
}

WPropertyColorWidget::~WPropertyColorWidget()
{
    // cleanup
}

void WPropertyColorWidget::setColorPickerButtonHidden( bool hide )
{
    m_colButton->setVisible( !hide );
}

void WPropertyColorWidget::update()
{
    QColor bgColor = toQColor( m_colorProperty->get() );

    setColor( bgColor );
}

void WPropertyColorWidget::setColor( const QColor& bgColor )
{
    std::stringstream buttonColorStr;
    buttonColorStr << "* { background-color: rgb("
                   << bgColor.red() << ","
                   << bgColor.green() << ","
                   << bgColor.blue() << ");"
                   << "border:none;"
                   << "}";

    std::stringstream buttonColorText;
    buttonColorText << "RGBA( " << bgColor.red() << ", "
                    << bgColor.green() << ", "
                    << bgColor.blue() << ", "
                    << bgColor.alpha() << " )";

    m_colorPickerAction->setText( QString().fromStdString( buttonColorText.str() ) );

    m_colPanel->setStyleSheet( QString().fromStdString( buttonColorStr.str() ) );


    // if this is a info property -> set background of label and some text
    m_asText.setText( QString::fromStdString( m_colorProperty->getAsString() ) );
    QPalette pal = QPalette();
    pal.setColor( QPalette::Background, toQColor( m_colorProperty->get() ) );
    m_asText.setAutoFillBackground( true );
    m_asText.setPalette( pal );
}

QColor WPropertyColorWidget::toQColor( WColor color )
{
    QColor tmp;
    tmp.setRgbF( color[0],
                 color[1],
                 color[2],
                 color[3] );

    return tmp;
}

WColor WPropertyColorWidget::toWColor( QColor color )
{
    return WColor( color.redF(), color.greenF(), color.blueF(), color.alphaF() );
}

void WPropertyColorWidget::buttonClicked()
{
    QColor current = toQColor( m_colorProperty->get() );

#if QT_VERSION >= 0x040500
    current = QColorDialog::getColor( current, this, QString( "Select Color" ), QColorDialog::ShowAlphaChannel );
#else
    current = QColorDialog::getColor( current, this );
#endif
    if( current.isValid() )
    {
        // set the button background to the appropriate color
        setColor( current );

        // convert it back to a WColor
        invalidate( !m_colorProperty->set( toWColor( current ) ) ); // NOTE: set automatically checks the validity of the value
    }
}

void WPropertyColorWidget::dragEnterEvent( QDragEnterEvent* event )
{
    if( event->mimeData()->hasColor() )
    {
        event->setAccepted( true );
    }
}

void WPropertyColorWidget::dropEvent( QDropEvent* event )
{
    if( event->mimeData()->hasColor() )
    {
        QColor color = qvariant_cast<QColor>( event->mimeData()->colorData() );
        if( color.isValid() )
        {
            setColor( color );

            // now we have to trigger an update
            invalidate( !m_colorProperty->set( toWColor( color ) ) ); // NOTE: set automatically checks the validity of the value
        }
    }
}

