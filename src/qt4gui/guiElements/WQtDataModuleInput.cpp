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

#include <QtGui/QAction>
#include <QtGui/QFileDialog>

#include "../WQt4Gui.h"
#include "../WIconManager.h"
#include "../controlPanel/WQtPropertyGroupWidget.h"

#include "core/kernel/WDataModuleInputFilterFile.h"
#include "core/kernel/WDataModuleInputFile.h"

#include "WQtDataModuleInput.h"
#include "WQtDataModuleInput.moc"

WQtDataModuleInput::WQtDataModuleInput( WDataModule::SPtr module, QWidget* parent ):
    QWidget( parent ),
    m_module( module )
{
    setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    m_layout = new QVBoxLayout();
    m_layout->setMargin( 0 );
    m_layout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );
    setLayout( m_layout );

    m_mainLayout = new QHBoxLayout();
    m_mainLayout->setMargin( 0 );
    m_mainLayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    m_mainWidget = new QWidget();
    m_mainWidget->setLayout( m_mainLayout );

    // create the clickable toolbutton to open the file
    m_changeButton = new QToolButton();
    m_reloadButton = new QToolButton();
    m_clearButton = new QToolButton();

    m_label = new WScaleLabel();
    m_label->setText( "UNSET" );

    m_changeButton->setAutoRaise( true );
    m_reloadButton->setAutoRaise( true );
    m_clearButton->setAutoRaise( true );
    m_reloadButton->setToolTip( "Force a reload of the data." );
    m_changeButton->setToolTip( "Change the source." );
    m_clearButton->setToolTip( "Clear the source and set no source." );

    m_mainLayout->addWidget( m_reloadButton );
    m_mainLayout->addWidget( m_clearButton );
    m_mainLayout->addWidget( m_changeButton );
    m_mainLayout->addWidget( m_label );

    // design the actions for both buttons
    WIconManager* iconMgr = WQt4Gui::getIconManager();
    QAction* changeAction = new QAction( iconMgr->getIcon( "load" ), "Change Source", this );
    m_changeButton->setDefaultAction( changeAction );
    QAction* reloadAction = new QAction( iconMgr->getIcon( "reload" ), "Reload Source", this );
    m_reloadButton->setDefaultAction( reloadAction );
    QAction* clearAction = new QAction( iconMgr->getIcon( "clear" ), "Clear Source", this );
    m_clearButton->setDefaultAction( clearAction );

    m_container = WQtPropertyGroupWidget::createPropertyGroupBox( m_mainWidget, false, this, "Source", 2 );
    m_layout->addWidget( m_container );

    // get notified when the input changes:
    m_inputChangeConnection = module->getInputChangedCondition()->subscribeSignal( boost::bind( &WQtDataModuleInput::onInputChange, this ) );
    onInputChangeGUI();

    // connect the actions
    connect( changeAction, SIGNAL( triggered( bool ) ), this, SLOT( onChange() ) );
    connect( reloadAction, SIGNAL( triggered( bool ) ), this, SLOT( onReload() ) );
    connect( clearAction, SIGNAL( triggered( bool ) ), this, SLOT( onClear() ) );
}

WQtDataModuleInput::~WQtDataModuleInput()
{
    m_inputChangeConnection.disconnect();

}

void WQtDataModuleInput::onInputChange()
{
    WQt4Gui::execInGUIThreadAsync( boost::bind( &WQtDataModuleInput::onInputChangeGUI, this ) );
}

void WQtDataModuleInput::onInputChangeGUI()
{
    if( m_module->getInput() )
    {
        m_label->setText( QString::fromStdString( m_module->getInput()->asString() ) );
        m_label->setToolTip( QString::fromStdString( m_module->getInput()->getExtendedInfo() ) );
    }
    else
    {
        m_label->setText( "UNSET" );
    }
}

void WQtDataModuleInput::onChange()
{
    QString result;
    QString all;

    std::vector< WDataModuleInputFilter::ConstSPtr > filters = m_module->getInputFilter();
    for( std::vector< WDataModuleInputFilter::ConstSPtr >::const_iterator filterIter = filters.begin(); filterIter != filters.end();
            ++filterIter )
    {
        WDataModuleInputFilterFile::ConstSPtr ff = boost::dynamic_pointer_cast< const WDataModuleInputFilterFile >( *filterIter );
        if( ff )
        {
            QString description = QString::fromStdString( ff->getDescription() );
            QString extension = QString::fromStdString( ff->getExtension() );
            all += QString( " *." ) + extension;
            result += description + QString( "(*." ) + extension + QString( ");;" );
        }
    }

    result = QString( "Known file types (" ) + all + QString( ");;" ) + result;
    result += QString( "Any files (*)" );

    // any current path?
    WDataModuleInputFile::SPtr inFile = m_module->getInputAs< WDataModuleInputFile >();
    QString defaultPath = "";
    if( inFile )
    {
        boost::filesystem::path p = inFile->getFilename();
        defaultPath = QString::fromStdString( p.parent_path().string() );
    }

    QStringList filenames = QFileDialog::getOpenFileNames( this, "Open Data", defaultPath, result );
    if( filenames.empty() )
    {
        return;
    }
}

void WQtDataModuleInput::onReload()
{
    m_module->reload();
}

void WQtDataModuleInput::onClear()
{
    m_module->setInput( WDataModuleInput::SPtr() );
}

