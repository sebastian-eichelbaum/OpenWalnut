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
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <QtCore/QDir>
#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFileDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>

#include "core/common/WPathHelper.h"
#include "core/kernel/WModuleFactory.h"
#include "WMainWindow.h"
#include "WQt4Gui.h"
#include "WQtModuleConfig.h"
#include "WQtModuleConfig.moc"

bool nameSort( WModule::ConstSPtr i, WModule::ConstSPtr j )
{
    return ( i->getName() < j->getName() );
}

WQtModuleConfig::WQtModuleConfig( QWidget* parent, Qt::WindowFlags f ):
    QDialog( parent, f )
{
    // configure the dialog
    setModal( true );

    // some minimum size
    resize( 600, 400 );

    // get a list of all the modules available
    WModuleFactory::PrototypeSharedContainerType::ReadTicket r = WModuleFactory::getModuleFactory()->getPrototypes();
    typedef WModuleFactory::PrototypeContainerConstIteratorType ProtoIter;
    for( ProtoIter iter = r->get().begin(); iter != r->get().end(); ++iter )
    {
        m_moduleList.push_back( *iter );
    }
    std::sort( m_moduleList.begin(), m_moduleList.end(), nameSort );

    // initialize members
    QVBoxLayout* layoutAllowedModules = new QVBoxLayout;
    QWidget* p1 = new QWidget();
    p1->setLayout( layoutAllowedModules );

    QVBoxLayout* layoutModulePaths = new QVBoxLayout;
    QWidget* p2 = new QWidget();
    p2->setLayout( layoutModulePaths );

    // setup tab widget.
    QTabWidget* tab = new QTabWidget( this );
    tab->addTab( p1, "Allowed Modules" );
    tab->addTab( p2, "Module Paths" );
    QVBoxLayout* masterLayout = new QVBoxLayout();
    masterLayout->addWidget( tab );
    setLayout( masterLayout );

    QString helpText = "This dialog allows you to modify the list of modules used everywhere in OpenWalnut. The list contains all loaded modules. "
                       "By default, the list is interpreted as white-list. This means, you have to select the modules you want "
                       "to use and disable those you won't use. You can switch to black-listing. This allows you to select all modules "
                       "that should not be used.";
    QLabel* hint = new QLabel( helpText );
    hint->setWordWrap( true );
    layoutAllowedModules->addWidget( hint );

    QString helpTextPaths = "You can add more search paths here, where OpenWalnut searches modules during startup. Each path is searched "
                            "recursively. Use this list to help OpenWalnut find your downloaded or self-made modules. An alternative option is to "
                            "set the environment variable \"OW_MODULE_PATH\", which is a semicolon-separated list of search paths. After "
                            "restarting OpenWalnut, the modules in the added paths appear in the list of allowed modules.";
    QLabel* hintPaths = new QLabel( helpTextPaths );
    hintPaths->setWordWrap( true );
    layoutModulePaths->addWidget( hintPaths );



    // always show all modules?
    m_showThemAll = new QCheckBox( "Always show all modules.", this );
    m_showThemAll->setToolTip(
        "Recommended option. This ensures that all modules get shown all them time, regardless of the list below."
    );
    connect( m_showThemAll, SIGNAL( stateChanged( int ) ), this, SLOT( showThemAllUpdated() ) );
    layoutAllowedModules->addWidget( m_showThemAll );

    m_asBlackList = new QCheckBox( "Use as black-list", this );
    m_asBlackList->setToolTip( "Activate this option if you want all modules to be active in OpenWalnut that are not explicitly deactivated here." );
    layoutAllowedModules->addWidget( m_asBlackList );

    // create the module list
    m_list = new QListWidget();
    layoutAllowedModules->addWidget( m_list );

    // the path list
    m_pathList = new QListWidget();
    layoutModulePaths->addWidget( m_pathList );

    // the path list also needs some add/remove buttons
    QHBoxLayout* addRemLayout = new QHBoxLayout();
    QWidget* addRemWidget = new QWidget();
    addRemWidget->setLayout( addRemLayout );
    layoutModulePaths->addWidget( addRemWidget );
    QPushButton* addButton = new QPushButton( "Add Path" );
    m_removePathButton = new QPushButton( "Remove Path" );
    m_removePathButton->setEnabled( false );
    addRemLayout->addWidget( addButton );
    addRemLayout->addWidget( m_removePathButton );

    connect( addButton, SIGNAL( clicked( bool ) ), this, SLOT( addModulePath() ) );
    connect( m_removePathButton, SIGNAL( clicked( bool ) ), this, SLOT( removeModulePath() ) );
    connect( m_pathList, SIGNAL( itemSelectionChanged() ), this, SLOT( pathListSelectionChanged() ) );

    // for modules without icon, use this
    QIcon noIcon = WQt4Gui::getMainWindow()->getIconManager()->getIcon( "DefaultModuleIcon" );

    // fill the list item
    for( std::vector< WModule::ConstSPtr >::const_iterator iter = m_moduleList.begin(); iter != m_moduleList.end(); ++iter )
    {
        // Create a custom widget which contains the name and description
        QWidget* widget = new QWidget( m_list );
        {
            QGridLayout* layoutWidget = new QGridLayout();

            QSizePolicy sizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed ); // <-- scale it down

            int column = 0;

            // create a checkbox for this module
            QCheckBox* check = new QCheckBox();
            check->setSizePolicy( sizePolicy );
            layoutWidget->addWidget( check, 0, 0, 2, 1 );

            // we later need to find the checkbox for one module easily:
            m_moduleItemMap[ ( *iter )->getName() ] = check;

            ++column;

            // now, an icon
            QLabel* icon = new QLabel();
            icon->setSizePolicy( sizePolicy );

            // we need to enforce some size
            QIcon modIco = WQt4Gui::getMainWindow()->getIconManager()->getIcon( ( *iter )->getName(), noIcon );
            QPixmap qicon( modIco.pixmap( 32, 32 ) );
            qicon = qicon.scaled( 32, 32, Qt::KeepAspectRatio );
            icon->setPixmap( qicon );

            layoutWidget->addWidget( icon, 0, column, 2, 1 );
            ++column;

            // Add Name and Description
            layoutWidget->addWidget( new QLabel( "<b>" + QString::fromStdString( ( *iter )->getName() )+ "</b>" ), 0, column );
            // if there is no description -> no widget added to save space
            if( !( *iter )->getDescription().empty() )
            {
                layoutWidget->addWidget(  new QLabel( QString::fromStdString( ( *iter )->getDescription() ) ), 1, column );
            }

            layoutWidget->setSizeConstraint( QLayout::SetMaximumSize );
            widget->setLayout( layoutWidget );
        }

        QListWidgetItem* item = new QListWidgetItem( m_list );
        item->setSizeHint( widget->sizeHint() );

        m_list->addItem( item );
        m_list->setItemWidget( item, widget );
        m_list->setMinimumHeight( 150 );
    }

    // provide some default buttons here
    QDialogButtonBox* defButtons = new QDialogButtonBox( QDialogButtonBox::Save | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults,
                                                         Qt::Horizontal, this );
    connect( defButtons, SIGNAL( accepted() ), this, SLOT( accept() ) );
    connect( defButtons, SIGNAL( rejected() ), this, SLOT( reject() ) );
    connect( defButtons->button( QDialogButtonBox::RestoreDefaults ), SIGNAL( clicked() ), this, SLOT( reset() ) );
    masterLayout->addWidget( defButtons );

    // initialize the widgets
    loadListsFromSettings();
}

WQtModuleConfig::~WQtModuleConfig()
{
    // cleanup
}

void WQtModuleConfig::initPathHelper()
{
    // we allow the user to specify additional module paths. They need to be loaded before the WModuleFactory initiates the module-load stuff.
    // Therefore, we grab the setting here and add it to WPathHelper
    QList< QVariant > paths = WQt4Gui::getSettings().value( "qt4gui/additionalModulePaths" ).toList();
    for( QList< QVariant >::const_iterator it = paths.begin(); it != paths.end(); ++it )
    {
        std::string p = ( *it ).toString().toStdString();
        WPathHelper::getPathHelper()->addAdditionalModulePath( p );
    }
}

void WQtModuleConfig::loadListsFromSettings( bool defaultModulePaths )
{
    // update checkbox too
    bool ignoreAllowedList = WQt4Gui::getSettings().value( "qt4gui/modules/IgnoreAllowedList", true ).toBool();
    m_showThemAll->setCheckState( ignoreAllowedList ? Qt::Checked : Qt::Unchecked );

    bool asBlackList = WQt4Gui::getSettings().value( "qt4gui/modules/asBlackList", false ).toBool();
    m_asBlackList->setCheckState( asBlackList ? Qt::Checked : Qt::Unchecked );

    // read settings
    std::string allowedModules = WQt4Gui::getSettings().value( "qt4gui/modules/allowedList", "" ).toString().toStdString();
    m_allowedModules = string_utils::tokenize( allowedModules, "," );

    // set dialog according to the settings
    for( AllowedModuleList::const_iterator iter = m_allowedModules.begin(); iter != m_allowedModules.end(); ++iter )
    {
        if( m_moduleItemMap.count( *iter ) )
        {
            m_moduleItemMap[ *iter ]->setCheckState( Qt::Checked );
        }
    }

    if( !defaultModulePaths )
    {
        // now, also fill the list
        // NOTE: we do not use the list in WPathHelper. This list will NOT be updated directly to ensure consistency between the path list in
        // WPathHelper and the loaded modules in WModuleFactory. WPathHelper is set correctly on next restart.
        QList< QVariant > paths = WQt4Gui::getSettings().value( "qt4gui/additionalModulePaths" ).toList();
        for( QList< QVariant >::const_iterator it = paths.begin(); it != paths.end(); ++it )
        {
            std::string p = ( *it ).toString().toStdString();
            m_pathList->addItem( QString::fromStdString( p ) );
        }
    }
}

void WQtModuleConfig::saveListToSettings()
{
    // rebuild list of allowed modules
    m_allowedModules.clear();
    std::string allowedAsString;    // this is needed since QList cannot be stored in a QSettings in a read-able format right now.

    // iterate the widgets and store in settings:
    for( ModuleItemMapType::const_iterator iter = m_moduleItemMap.begin(); iter != m_moduleItemMap.end(); ++iter )
    {
        // if it is checked, use.
        if( iter->second->checkState() == Qt::Checked ) // save only checked items
        {
            m_allowedModules.push_back( iter->first );
            allowedAsString += iter->first + ",";
        }
    }

    // store this list in settings
    WQt4Gui::getSettings().setValue( "qt4gui/modules/allowedList", QString::fromStdString( allowedAsString ) );
    WQt4Gui::getSettings().setValue( "qt4gui/modules/IgnoreAllowedList", ( m_showThemAll->checkState() == Qt::Checked ) );
    WQt4Gui::getSettings().setValue( "qt4gui/modules/asBlackList", ( m_asBlackList->checkState() == Qt::Checked ) );

    // also write the path list
    QList< QVariant > paths;
    for( int i = 0; i < m_pathList->count(); ++i )
    {
        paths.push_back( m_pathList->item( i )->text() );
    }
    WQt4Gui::getSettings().setValue( "qt4gui/additionalModulePaths", paths );
}

bool WQtModuleConfig::operator()( std::string const& name ) const
{
    bool blackList = ( m_asBlackList->checkState() == Qt::Checked );
    bool ignoreList = ( m_showThemAll->checkState() == Qt::Checked );
    bool isInList = ( std::find( m_allowedModules.begin(), m_allowedModules.end(), name ) != m_allowedModules.end() );

    return !ignoreList && ( ( blackList && isInList ) || ( !blackList && !isInList ) );
}

bool WQtModuleConfig::operator()( WModule::ConstSPtr module ) const
{
    return operator()( module->getName() );
}

void WQtModuleConfig::configure()
{
    show();
}

QAction* WQtModuleConfig::getConfigureAction() const
{
    QAction* a = new QAction( "Configure Modules", parent() );
    a->setToolTip( "Allows you to configure the list of modules, which is used for selecting modules in OpenWalnut (i.e. in the toolbar)." );
    connect( a, SIGNAL( triggered( bool ) ), this, SLOT( configure() ) );
    return a;
}

void WQtModuleConfig::accept()
{
    saveListToSettings();
    emit updated();
    QDialog::accept();
}

void WQtModuleConfig::reject()
{
    // reset everything to the current state in the settings:
    loadListsFromSettings();

    QDialog::reject();
}

void WQtModuleConfig::showThemAllUpdated()
{
    if( m_showThemAll->checkState() == Qt::Checked )
    {
        m_list->setDisabled( true );
    }
    else
    {
        m_list->setDisabled( false );
    }
}

void WQtModuleConfig::reset()
{
    m_pathList->clear();

    // reset all checkboxes
    for( std::vector< WModule::ConstSPtr >::const_iterator iter = m_moduleList.begin(); iter != m_moduleList.end(); ++iter )
    {
        // we later need to find the checkbox for one module easily:
        m_moduleItemMap[ ( *iter )->getName() ]->setCheckState( Qt::Unchecked );
    }
    loadListsFromSettings( true );

    m_showThemAll->setCheckState( Qt::Unchecked );
    m_asBlackList->setCheckState( Qt::Unchecked );

    m_list->setDisabled( false );
}

void WQtModuleConfig::addModulePath()
{
    QString dir = QFileDialog::getExistingDirectory( this, "Select Directory",
                                                           QString::fromStdString( WPathHelper::getHomePath().string() ),
                                                           QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    m_pathList->addItem( dir );
}

void WQtModuleConfig::removeModulePath()
{
    qDeleteAll( m_pathList->selectedItems() );
}

void WQtModuleConfig::pathListSelectionChanged()
{
    if( m_pathList->selectedItems().size() )
    {
        m_removePathButton->setEnabled( true );
    }
    else
    {
        m_removePathButton->setEnabled( false );
    }
}
