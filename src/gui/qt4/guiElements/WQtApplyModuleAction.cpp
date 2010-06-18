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

#include "WQtApplyModuleAction.h"

WQtApplyModuleAction::WQtApplyModuleAction( QWidget* parent, WIconManager* iconManager, boost::shared_ptr< WApplyCombiner > combiner,
                                            bool advancedText ):
    QAction( parent ),
    m_combiner( combiner )
{
    // nice tooltip
    std::string from = "";

    // NOTE: all the tooltips and so on for this action are used from the first combiner in the group

    // might be null ( for example if a module should be added that does not require an input)
    if ( m_combiner->getSrcModule() )
    {
        from = m_combiner->getSrcModule()->getName() + ":" + m_combiner->getSrcConnector() + " -> ";
    }
    std::string tooltip = from + m_combiner->getTargetModule()->getName() + ":" + m_combiner->getTargetConnector();
    setToolTip( tooltip.c_str() );
    setText( advancedText ? tooltip.c_str() : m_combiner->getTargetModule()->getName().c_str() );
    setIconText( advancedText ? tooltip.c_str() : m_combiner->getTargetModule()->getName().c_str() );
    setIcon( iconManager->getIcon( m_combiner->getTargetModule()->getName().c_str() ) );

    // we need to use released signal here, as the pushed signal also gets emitted on newly created buttons which are under the mouse pointer with
    // pressed left button.
    connect( this, SIGNAL( triggered() ), this, SLOT( applyCombiner() ) );
}

WQtApplyModuleAction::~WQtApplyModuleAction()
{
}

void WQtApplyModuleAction::applyCombiner()
{
    m_combiner->run();
}

