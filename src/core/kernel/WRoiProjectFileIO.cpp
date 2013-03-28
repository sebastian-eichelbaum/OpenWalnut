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

#include "../common/WLogger.h"
#include "../graphicsEngine/WROI.h"
#include "../graphicsEngine/WROIBox.h"
#include "WROIManager.h"
#include "WKernel.h"

#include "WRoiProjectFileIO.h"

WRoiProjectFileIO::WRoiProjectFileIO():
    WProjectFileIO()
{
    // initialize members
}

WRoiProjectFileIO::~WRoiProjectFileIO()
{
    // cleanup
}

bool WRoiProjectFileIO::parse( std::string /* line */, unsigned int /* lineNumber */ )
{
    // read something
    return false;
}

void WRoiProjectFileIO::done()
{
    // apply
}

void WRoiProjectFileIO::save( std::ostream& output )   // NOLINT
{
    // save here
    output << "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl <<
              "// ROI Structure" << std::endl <<
              "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl <<
              std::endl;

    // write all branches
    WROIManager::Branches branches =WKernel::getRunningKernel()->getRoiManager()->getBranches();
    unsigned int branchIndex = 0;
    for( WROIManager::Branches::const_iterator branchIter = branches.begin(); branchIter != branches.end(); ++branchIter )
    {
        // get the branch
        WRMBranch::SPtr branch = *branchIter;

        // write this branch's properties
        output << "SELECTOR_BRANCH:" << branchIndex << std::endl;
        printProperties( output, branch->getProperties(), "", "", branchIndex, "SELECTOR_BRANCH" );
        output << std::endl;

        // handle this branch's ROIs
        WROIManager::ROIs rois = branch->getRois();
        unsigned int roiIndex = 0;
        for( WROIManager::ROIs::const_iterator roiIter = rois.begin(); roiIter != rois.end(); ++roiIter )
        {
            // differentiate the type of roi, currently we will support only WROiBox
            WROIBox* roiBox = dynamic_cast< WROIBox* >( ( *roiIter ).get() );

            output << "SELECTOR_ROIBOX:" << roiIndex << ":SELECTOR_BRANCH" << branchIndex << std::endl;
            printProperties( output, roiBox->getProperties(), "", "", roiIndex, "SELECTOR_ROIBOX" );
            output << std::endl;

            // done
            roiIndex++;
        }

        // done
        branchIndex++;
    }
}

