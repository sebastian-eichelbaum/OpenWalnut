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

#include <set>
#include <string>
#include <vector>

#include <QApplication>

#include "../events/WEventTypes.h"
#include "../events/WModuleConnectorEvent.h"
#include "../events/WModuleDeleteEvent.h"
#include "../events/WPropertyChangedEvent.h"
#include "../WMainWindow.h"
#include "../WQt4Gui.h"
#include "core/common/WLogger.h"
#include "core/common/WProgress.h"
#include "core/common/WProgressCombiner.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleInputConnector.h"
#include "core/kernel/WModuleOutputConnector.h"
#include "WQtControlPanel.h"
#include "WQtTreeItem.h"
#include "WTreeItemTypes.h"

WQtTreeItem::WQtTreeItem( QTreeWidgetItem * parent, WTreeItemType type, boost::shared_ptr< WModule > module ) :
    QTreeWidgetItem( parent, type ),
    m_deleteInProgress( false ),
    m_needPostDeleteEvent( true ),
    m_handledInput( "" )
{
    m_module = module;
    m_name = module->getName();

    if( module->getProperties()->getProperty( "active" )->toPropBool()->get() )
    {
        this->setCheckState( 0, Qt::Checked );
    }
    else
    {
        this->setCheckState( 0, Qt::Unchecked );
    }

    this->setFlags( Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );

    // grab the runtime name property
    // replace the name by the filename
    boost::shared_ptr< WPropertyBase > p = module->getProperties()->findProperty( "Name" );

    // always ensure that findProperty really found something
    if( p )
    {
        m_nameProp = p->toPropString();
    }

    // was it a string prop?
    if( m_nameProp )
    {
        m_name = m_nameProp->get( true );
        m_nameProp->getUpdateCondition()->subscribeSignal( boost::bind( &WQtTreeItem::nameChanged, this ) );
    }

    m_updateTimer = boost::shared_ptr< QTimer >( new QTimer() );
    connect( m_updateTimer.get(), SIGNAL( timeout() ), this, SLOT( update() ) );
    m_updateTimer->start( 500 );

    // we need to get informed about updated outputs.
    const WModule::OutputConnectorList& outs = module->getOutputConnectors();
    t_GenericSignalHandlerType changeSignal = boost::bind( &WQtTreeItem::slotDataChanged, this, _2 );
    for( WModule::OutputConnectorList::const_iterator iter = outs.begin(); iter != outs.end(); ++iter )
    {
        // subscribe each connectors update signal
        m_outputUpdateConnections.push_back( ( *iter )->subscribeSignal( DATA_CHANGED, changeSignal ) );
    }
}

WQtTreeItem::~WQtTreeItem()
{
    // disconnect each subscription
    for( std::vector< boost::signals2::connection >::iterator iter = m_outputUpdateConnections.begin();
         iter != m_outputUpdateConnections.end(); ++iter )
    {
        ( *iter ).disconnect();
    }
}

boost::shared_ptr< WModule > WQtTreeItem::getModule()
{
    return m_module;
}

std::string WQtTreeItem::getName()
{
    return m_name;
}

std::string WQtTreeItem::createTooltip( WModule::SPtr module )
{
    // create the tooltip text
    std::string tooltip = "";
    tooltip += "<b>Module: </b>" + module->getName() + "<br/>";
    if( module->isCrashed()() )
    {
        tooltip += "<b>State: </b>crashed<br/>";
        tooltip += "<b>Crash-Message: </b>" + module->getCrashMessage() + "<br/>";
    }
    else
    {
        tooltip += "<b>State: </b>normal<br/>";
    }
    tooltip += "<b>Connectors: </b>";

    // also list the connectors
    std::string conList = "";
    WModule::InputConnectorList consIn = module->getInputConnectors();
    WModule::OutputConnectorList consOut = module->getOutputConnectors();
    conList += "<table><tr><th>Name</th><th>Description</th><th>Type (I/O)</th><th>Connected</th></tr>";
    int conCount = 0;
    for( WModule::InputConnectorList::const_iterator it = consIn.begin(); it != consIn.end(); ++it )
    {
        ++conCount;
        conList += "<tr><td><b>" + ( *it )->getName() + "&nbsp;</b></td><td>" + ( *it )->getDescription() + "&nbsp;</td>";
        conList += "<td><center>In</center></td>";
        conList += ( *it )->isConnected() ? "<td><center>Yes</center></td>" : "<td><center>No</center></td>";
        conList += "</tr>";
    }
    for( WModule::OutputConnectorList::const_iterator it = consOut.begin(); it != consOut.end(); ++it )
    {
        ++conCount;
        conList += "<tr><td><b>" + ( *it )->getName() + "&nbsp;</b></td><td>" + ( *it )->getDescription() + "&nbsp;</td>";
        conList += "<td><center>Out</center></td>";
        conList += ( *it )->isConnected() ? "<td></center>Yes</center></td>" : "<td><center>No</center></td>";
        conList += "</tr>";
    }
    conList += "</table>";

    tooltip += conCount ? "Yes" + conList + "<br/><br/>" : "None<br/>";
    tooltip += "<b>Module Description: </b><br/>" + module->getMetaInformation()->getDescription();

    return tooltip;
}

void WQtTreeItem::slotDataChanged( boost::shared_ptr<WModuleConnector> connector )
{
    // post event
    QCoreApplication::postEvent( WQt4Gui::getMainWindow()->getControlPanel(), new WModuleConnectorEvent( m_module, connector ) );
}

void WQtTreeItem::update()
{
    updateState();
}

void WQtTreeItem::updateState()
{
    boost::shared_ptr< WProgressCombiner> p = m_module->getRootProgressCombiner();

    // update the progress combiners internal state
    p->update();

    std::string connInfo = "";
    if( ( m_handledOutput != "" ) && ( m_handledInput != "" ) )
    {
        connInfo = " (" + m_handledOutput + "->" + m_handledInput + ") ";
    }

    // is it pending?
    std::string progress = "Waiting";
    if( m_module->isCrashed()() )
    {
        progress = "Problem occurred";
        setText( 0, ( m_name + " (problem occurred)"  + connInfo ).c_str() );

        // strike out the name of the module to show the crash visually.
        QFont curFont = font( 0 );
        curFont.setStrikeOut( true );
        setFont( 0, curFont );
        setIcon( 0, WQt4Gui::getMainWindow()->getIconManager()->getIcon( "moduleCrashed" ) );

        // this ensures that crashed modules can be deleted
        setDisabled( false );
    }
    else if( p->isPending() )
    {
        progress = "Busy " + p->getCombinedNames();
        setIcon( 0, WQt4Gui::getMainWindow()->getIconManager()->getIcon( "moduleBusy" ) );
        std::ostringstream progressText;

        // construct a name for the progress indicator
        std::string name = p->getName();
        if( !name.empty() )
        {
            name = " [" + name + "]";
        }

        if( p->isDetermined() )
        {
            progressText.setf( std::ios::fixed );
            progressText.precision( 0 );
            progressText << p->getProgress() << "%" << name;
        }
        else
        {
            progressText << "Pending" << name;
        }

        setText( 0, ( m_name + " - " + progressText.str() + connInfo ).c_str() );
    }
    else
    {
        setIcon( 0, QIcon() );
        setText( 0, ( m_name + connInfo ).c_str() );
    }

    // if the user requested it to be deleted: disable and color it
    if( m_deleteInProgress )
    {
        setForeground( 0, QBrush( QColor::fromRgb( 255, 0, 0 ) ) );
        setDisabled( true );
    }

    // is finished?
    if( m_deleteInProgress && !m_module->isRunning().get() && m_needPostDeleteEvent )
    {
        m_needPostDeleteEvent = false;  // this ensures the event is only posted once
        QCoreApplication::postEvent( WQt4Gui::getMainWindow()->getNetworkEditor(), new WModuleDeleteEvent( this ) );
        QCoreApplication::postEvent( WQt4Gui::getMainWindow()->getControlPanel(), new WModuleDeleteEvent( this ) );
    }

    // active ?
    if( m_module->getProperties()->getProperty( "active" )->toPropBool()->get() )
    {
        setCheckState( 0, Qt::Checked );
    }
    else
    {
        setCheckState( 0, Qt::Unchecked );
    }

    // update tooltip
    setToolTip( 0, WQtTreeItem::createTooltip( m_module ).c_str() );
}

void WQtTreeItem::gotRemoved()
{
    // this ensures a visual feedback to the user while waiting for the module to finish.

    // update tree item state
    m_deleteInProgress = true;
}

void WQtTreeItem::nameChanged()
{
    // luckily, the update mechanism of WQtTreeItem regularly sets the name using m_name. So we do not even need to post some kind of event.
    m_name = m_nameProp->get( true );
}

std::string WQtTreeItem::getHandledInput() const
{
    return m_handledInput;
}

void WQtTreeItem::setHandledInput( std::string in )
{
    m_handledInput = in;
}

std::string WQtTreeItem::getHandledOutput() const
{
    return m_handledOutput;
}

void WQtTreeItem::setHandledOutput( std::string out )
{
    m_handledOutput = out;
}

void WQtTreeItem::handleCheckStateChange()
{
    // active ?
    if( checkState( 0 ) == Qt::Checked )
    {
        m_module->getProperties()->getProperty( "active" )->toPropBool()->set( true );
    }
    else
    {
        m_module->getProperties()->getProperty( "active" )->toPropBool()->set( false );
    }
}

