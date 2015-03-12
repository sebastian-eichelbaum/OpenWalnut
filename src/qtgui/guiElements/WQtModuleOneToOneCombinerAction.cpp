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

#include "WQtModuleOneToOneCombinerAction.h"

WQtModuleOneToOneCombinerAction::WQtModuleOneToOneCombinerAction( QWidget* parent, WIconManager* iconManager,
                                                                  boost::shared_ptr< WModuleOneToOneCombiner > combiner,
                                                                  bool advancedText ):
    QAction( parent ),
    m_combiner( combiner )
{
    // nice tooltip
    std::string from = "";

    // NOTE: all the tooltips and so on for this action are used from the first combiner in the group

    // use the name property of the modules
    std::string srcName = "";
    std::string targetName = m_combiner->getTargetModule()->getProperties()->getProperty( "Name" )->toPropString()->get();

    // might be null ( for example if a module should be added that does not require an input)
    if( m_combiner->getSrcModule() )
    {
        srcName = m_combiner->getSrcModule()->getProperties()->getProperty( "Name" )->toPropString()->get();

        // append real module name if it is different from user specified name
        if( srcName != m_combiner->getSrcModule()->getName() )
        {
            srcName += " (" + m_combiner->getSrcModule()->getName() + ")";
        }
    }

    // append real module name if it is different from user specified name
    if( targetName != m_combiner->getTargetModule()->getName() )
    {
        targetName += " (" + m_combiner->getTargetModule()->getName() + ")";
    }

    from = srcName + ": <i>" + m_combiner->getSrcConnector() + "</i> &nbsp;&nbsp;&#10140;&nbsp;&nbsp; ";
    std::string tooltip = "<b>" + targetName + "</b><br><nobr>" + from + targetName + ": <i>" + m_combiner->getTargetConnector() + "</i></nobr>";
    tooltip += "<br><b>Module Description: </b><br/>" + m_combiner->getTargetModule()->getDescription();

    // We need this because menu items seem not to support HTML
    std::string advancedTextString = srcName + ":" + m_combiner->getSrcConnector() + " -> " + targetName + ":" + m_combiner->getTargetConnector();

    setToolTip( tooltip.c_str() );
    setText( advancedText ? advancedTextString.c_str() : targetName.c_str() );
    setIconText( advancedText ? tooltip.c_str() : targetName.c_str() );

    // get an icon for this module
    setIcon( iconManager->getIcon( m_combiner->getTargetModule()->getName().c_str(), iconManager->getIcon( "DefaultModuleIcon" ) ) );

    // we need to use released signal here, as the pushed signal also gets emitted on newly created buttons which are under the mouse pointer with
    // pressed left button.
    connect( this, SIGNAL( triggered() ), this, SLOT( applyCombiner() ) );
}

WQtModuleOneToOneCombinerAction::~WQtModuleOneToOneCombinerAction()
{
}

void WQtModuleOneToOneCombinerAction::applyCombiner()
{
    m_combiner->run();
}

