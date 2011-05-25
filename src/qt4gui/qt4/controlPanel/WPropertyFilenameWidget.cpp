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

#include <boost/lexical_cast.hpp>

#include <QtGui/QFileDialog>

#include "core/common/WLogger.h"
#include "core/common/WPropertyVariable.h"
#include "core/common/constraints/WPropertyConstraintTypes.h"
#include "../WGuiConsts.h"

#include "WPropertyFilenameWidget.h"
#include "WPropertyFilenameWidget.moc"

WPropertyFilenameWidget::WPropertyFilenameWidget( WPropFilename property, QGridLayout* propertyGrid, QWidget* parent ):
    WPropertyWidget( property, propertyGrid, parent ),
    m_fnProperty( property ),
    m_button( &m_parameterWidgets ),
    m_layout( &m_parameterWidgets ),
    m_asText( &m_informationWidgets ),
    m_infoLayout( &m_informationWidgets )
{
    // initialize members
    m_parameterWidgets.setLayout( &m_layout );

    // layout both against each other
    m_layout.addWidget( &m_button );
    m_layout.setMargin( WGLOBAL_MARGIN );
    m_layout.setSpacing( WGLOBAL_SPACING );

    // Information Output ( Property Purpose = PV_PURPOSE_INFORMATION )
    m_infoLayout.addWidget( &m_asText );
    m_infoLayout.setMargin( WGLOBAL_MARGIN );
    m_infoLayout.setSpacing( WGLOBAL_SPACING );
    m_informationWidgets.setLayout( &m_infoLayout );

    // set the initial values
    update();

    // connect the modification signal of the edit and slider with our callback
    connect( &m_button, SIGNAL( released() ), this, SLOT( buttonReleased() ) );
}

WPropertyFilenameWidget::~WPropertyFilenameWidget()
{
    // cleanup
}

void WPropertyFilenameWidget::update()
{
    QString val = QString::fromStdString( m_fnProperty->get().filename() );
    m_button.setText( val );
    m_asText.setText( val );
}

void WPropertyFilenameWidget::buttonReleased()
{
    QString path;

    // if there is a "IsDirectory" constraint -> set a special option in the dialog
    if( m_fnProperty->countConstraint( PC_ISDIRECTORY ) != 0 )
    {
        // OK there should only be directories allowed
        path = QFileDialog::getExistingDirectory( this,
                QString::fromStdString( "Select directory for " + m_fnProperty->getName() ),
                QString::fromStdString( m_fnProperty->get().file_string() ),
                QFileDialog::DontConfirmOverwrite );
    }
    else if( m_fnProperty->countConstraint( PC_PATHEXISTS ) != 0 )
    {
        path = QFileDialog::getOpenFileName( this,
                QString::fromStdString( "Select existing file for " + m_fnProperty->getName() ),
                QString::fromStdString( m_fnProperty->get().file_string() ), QString(), 0,
                QFileDialog::DontConfirmOverwrite );
    }
    else
    {
        path = QFileDialog::getSaveFileName( this,
                QString::fromStdString( "Select file for " + m_fnProperty->getName() ),
                QString::fromStdString( m_fnProperty->get().file_string() ), QString(), 0,
                QFileDialog::DontConfirmOverwrite );
    }

    // convert it back to a WColor
    invalidate( !m_fnProperty->set( boost::filesystem::path( path.toStdString() ) ) ); // NOTE: set automatically checks the validity of the value

    // set button
    m_button.setText( QString::fromStdString( m_fnProperty->get().filename() ) );
}

