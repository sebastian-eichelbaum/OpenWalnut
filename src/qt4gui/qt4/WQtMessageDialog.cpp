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

#include <iostream>

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

#include "WMainWindow.h"

#include "WQtMessageDialog.h"
#include "WQtMessageDialog.moc"

WQtMessageDialog::WQtMessageDialog( QString msgID, QString title, QWidget* content, QSettings& settings, QWidget* parent ): // NOLINT - yes use a non-const ref
    QDialog( parent ),
    m_content( content ),
    m_msgID( msgID ),
    m_settings( settings )
{
    setWindowTitle( title );
    setModal( false );

    // setup contents
    QVBoxLayout* mainLayout = new QVBoxLayout();

    // text widget
    mainLayout->addWidget( m_content );

    // dialog buttons and checkbox to bottom layout
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    QWidget* bottomWidget = new QWidget( this );
    bottomWidget->setLayout( bottomLayout );
    mainLayout->addWidget( bottomWidget );
    this->setLayout( mainLayout );

    m_checkBox = new QCheckBox( bottomWidget );
    m_checkBox->setText( "Do not show again" );
    m_buttonBox = new QDialogButtonBox( bottomWidget );
    m_buttonBox->setOrientation( Qt::Horizontal );
    m_buttonBox->setStandardButtons( QDialogButtonBox::Ok );
    bottomLayout->addWidget( m_checkBox );
    bottomLayout->addWidget( m_buttonBox );

    // connect dialog box to close event
    QObject::connect( m_buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );
    QObject::connect( m_buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
}

WQtMessageDialog::~WQtMessageDialog()
{
    // cleanup
}

void WQtMessageDialog::show()
{
    show( false );  // NOTE: this also calls QDialog::show.
}

void WQtMessageDialog::show( bool force )
{
    // check if message is allowed
    bool show = m_settings.value( m_msgID + "_showAgain", true ).toBool();
    m_checkBox->setCheckState( show ? Qt::Unchecked : Qt::Checked );

    // forced to show?
    // NOTE: we set the m_checkBox earlier to keep the old showAgain state
    show = show || force;

    // only show if wanted by user
    if( !show )
    {
        return;
    }

    QDialog::show();
}

void WQtMessageDialog::reject()
{
    QDialog::reject();
}

void WQtMessageDialog::accept()
{
    handleClose();
    QDialog::accept();
}

void WQtMessageDialog::handleClose()
{
    m_settings.setValue( m_msgID + "_showAgain", m_checkBox->checkState() == Qt::Unchecked );
}

