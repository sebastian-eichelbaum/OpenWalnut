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

#include <string>

#include "WQtNumberEdit.h"

#include "WQtDSBWidget.h"

WQtDSBWidget::WQtDSBWidget( std::string name, QWidget* parent  )
    : QWidget( parent ),
    m_name( name.c_str() ),
    m_layout()
{
}


WQtDSBWidget::~WQtDSBWidget()
{
}


QPushButton* WQtDSBWidget::addPushButton( std::string label )
{
    int row = m_layout.rowCount();

    QPushButton* button = new QPushButton();
    button->setText( QString( label.c_str() ) );

    m_layout.addWidget( button, row, 0 );

    setLayout( &m_layout );
    return button;
}


WQtCheckBox* WQtDSBWidget::addCheckBox( std::string label, bool isChecked )
{
    int row = m_layout.rowCount();
    QLabel* qlabel = new QLabel( label.c_str() );

    WQtCheckBox* checkBox = new WQtCheckBox();
    checkBox->setName( label );
    checkBox->setChecked( isChecked );

    m_layout.addWidget( qlabel, row, 0 );
    m_layout.addWidget( checkBox, row, 1 );

    setLayout( &m_layout );

    return checkBox;
}


WQtLineEdit* WQtDSBWidget::addLineEdit( std::string label, std::string text )
{
    int row = m_layout.rowCount();
    QLabel* qlabel = new QLabel( label.c_str() );

    WQtLineEdit* lineEdit = new WQtLineEdit();
    lineEdit->setName( label );
    lineEdit->setText( QString( text.c_str() ) );

    m_layout.addWidget( qlabel, row, 0 );
    m_layout.addWidget( lineEdit, row, 1 );

    setLayout( &m_layout );

    return lineEdit;
}


WQtSliderWithEdit* WQtDSBWidget::addSliderInt( std::string label, int value, int min, int max )
{
    int row = m_layout.rowCount();
    QLabel* qlabel = new QLabel( label.c_str() );

    WQtSliderWithEdit* slider = new WQtSliderWithEdit( label );

    slider->setMin( min );
    slider->setMax( max );
    slider->setValue( value );

    m_layout.addWidget( qlabel, row, 0 );
    m_layout.addWidget( slider, row, 1 );

    setLayout( &m_layout );

    return slider;
}

QString WQtDSBWidget::getName()
{
    return m_name;
}
