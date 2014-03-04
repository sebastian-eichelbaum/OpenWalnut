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

#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>

#include "core/common/WLogger.h"
#include "core/common/WStringUtils.h"
#include "core/kernel/WProjectFile.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleContainer.h"
#include "core/kernel/WModule.h"

#include "WQtNetworkEditor.h"
#include "WQtNetworkItemGrid.h"
#include "WQtNetworkEditorProjectFileIO.h"

WQtNetworkEditorProjectFileIO::WQtNetworkEditorProjectFileIO( WQtNetworkEditor* ne ):
    WProjectFileIO(),
    m_networkEditor( ne )
{
    // initialize members
    // We want to set the layout before actually starting the network.
    setApplyOrder( PRE_MODULES );
}

WQtNetworkEditorProjectFileIO::~WQtNetworkEditorProjectFileIO()
{
    // cleanup
}

WProjectFileIO::SPtr WQtNetworkEditorProjectFileIO::clone( WProjectFile* project ) const
{
    // nothing special. Simply create new instance.
    WProjectFileIO::SPtr p( new WQtNetworkEditorProjectFileIO( m_networkEditor ) );
    p->setProject( project );
    return p;
}

bool WQtNetworkEditorProjectFileIO::parse( std::string line, unsigned int lineNumber )
{
    return false;
}

void WQtNetworkEditorProjectFileIO::done()
{
    // apply changes
}

void WQtNetworkEditorProjectFileIO::save( std::ostream& output ) // NOLINT
{
    output << "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl <<
              "// Qt4GUI Network Information" << std::endl <<
              "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl <<
              std::endl;

    // write the relative grid positions of each module
    WQtNetworkItemGrid* grid = m_networkEditor->getLayout()->getGrid();

    // iterate each module:
    // Grab container and lock
    WModuleContainer::ModuleSharedContainerType::ReadTicket container = WKernel::getRunningKernel()->getRootContainer()->getModules();
    // Iterate
    for( WModuleContainer::ModuleConstIterator iter = container->get().begin(); iter != container->get().end(); ++iter )
    {
        // a module has its own project file specific ID:
        unsigned int id = getProject()->mapFromModule( *iter );

        // get the graphical item for this module
        WQtNetworkItem* item = m_networkEditor->findItemByModule( *iter );

        // as grid for position
        if( grid->isInGrid( item ) )
        {
            QPoint p = grid->whereIs( item );
            output << "QT4GUI_NETWORK:" << id << "=" << p.x() << ";" << p.y() << std::endl;
        }
        // else: not in grid. We do not save info for this module
    }
}

