//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#include "../../../kernel/WKernel.h"
#include "../../emptyIcon.xpm" // Please put a real icon here.

#include "WMGpView.h"

W_LOADABLE_MODULE( WMGpView )

WMGpView::WMGpView():
    WModule()
{
}

WMGpView::~WMGpView()
{
}

boost::shared_ptr< WModule > WMGpView::factory() const
{
    return boost::shared_ptr< WModule >( new WMGpView() );
}

const char** WMGpView::getXPMIcon() const
{
    return emptyIcon_xpm; // Please put a real icon here.
}
const std::string WMGpView::getName() const
{
    return "Gaussian Process Viewer";
}

const std::string WMGpView::getDescription() const
{
    return "Displays gaussian processes. It is intended to display espically GP representing deterministic tracts.";
}

void WMGpView::connectors()
{
    typedef WModuleInputData< WDataSetGP > GpIC_t;
    m_gpIC = boost::shared_ptr< GpIC_t >( new GpIC_t( shared_from_this(), "gpIn", "The Gaussian processes" ) );

    WModule::connectors();
}

void WMGpView::properties()
{
    WModule::properties();
}

void WMGpView::moduleMain()
{
    m_moduleState.setResetable( true, true ); // remember actions when actually not waiting for actions
    m_moduleState.add( m_gpIC->getDataChangedCondition() );

    ready();

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        if ( !m_gpIC->getData().get() ) // ok, the output has not yet sent data
        {
            m_moduleState.wait();
            continue;
        }

        // To query whether an input was updated, simply ask the input:
        bool dataUpdated = m_gpIC->handledUpdate();
        boost::shared_ptr< WDataSetGP > dataSet = m_gpIC->getData();
        bool dataValid = ( dataSet );
        if( !dataValid )
        {
            continue;
        }
        if( dataUpdated )
        {
        }
    }
}
