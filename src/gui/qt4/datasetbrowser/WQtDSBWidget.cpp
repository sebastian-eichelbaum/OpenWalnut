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
    m_controlLayout(),
    m_pageLayout()
{
    m_pageLayout.addLayout( &m_controlLayout );
}


WQtDSBWidget::~WQtDSBWidget()
{
}


QPushButton* WQtDSBWidget::addPushButton( QString label )
{
    int row = m_controlLayout.rowCount();

    QPushButton* button = new QPushButton();
    button->setText( label );

    m_controlLayout.addWidget( button, row, 0 );

    setLayout( &m_pageLayout );
    return button;
}


WQtCheckBox* WQtDSBWidget::addCheckBox( QString label, bool isChecked )
{
    int row = m_controlLayout.rowCount();
    QLabel* qlabel = new QLabel( label );

    WQtCheckBox* checkBox = new WQtCheckBox();
    checkBox->setName( label );
    checkBox->setChecked( isChecked );

    m_controlLayout.addWidget( qlabel, row, 0 );
    m_controlLayout.addWidget( checkBox, row, 1 );

    setLayout( &m_pageLayout );

    return checkBox;
}


WQtLineEdit* WQtDSBWidget::addLineEdit( QString label, QString text )
{
    int row = m_controlLayout.rowCount();
    QLabel* qlabel = new QLabel( label );

    WQtLineEdit* lineEdit = new WQtLineEdit();
    lineEdit->setName( label );
    lineEdit->setText( text );

    m_controlLayout.addWidget( qlabel, row, 0 );
    m_controlLayout.addWidget( lineEdit, row, 1 );

    setLayout( &m_pageLayout );

    return lineEdit;
}


WQtSliderWithEdit* WQtDSBWidget::addSliderInt( QString label, int value, int min, int max )
{
    int row = m_controlLayout.rowCount();
    QLabel* qlabel = new QLabel( label );

    WQtSliderWithEdit* slider = new WQtSliderWithEdit( label );

    slider->setMin( min );
    slider->setMax( max );
    slider->setValue( value );

    m_controlLayout.addWidget( qlabel, row, 0 );
    m_controlLayout.addWidget( slider, row, 1 );

    setLayout( &m_pageLayout );

    return slider;
}

void WQtDSBWidget::addSpacer()
{
    m_pageLayout.addStretch();
    setLayout( &m_pageLayout );
}

QString WQtDSBWidget::getName()
{
    return m_name;
}
