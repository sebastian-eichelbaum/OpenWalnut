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

#include "../common/WStringUtils.h"
#include "../common/WProperties.h"
#include "../common/WPropertyBase.h"
#include "../common/WPropertyVariable.h"
#include "../common/WPropertyTypes.h"
#include "../common/exceptions/WFileNotFound.h"
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

bool WRoiProjectFileIO::parse( std::string line, unsigned int lineNumber )
{
    // this is the proper regular expression for modules
    static const boost::regex branchRe( "^ *SELECTOR_BRANCH:([0-9]*)$" );
    static const boost::regex roiBoxRe( "^ *SELECTOR_ROIBOX:([0-9]*):SELECTOR_BRANCH([0-9]*)$" );
    static const boost::regex branchPropRe( "^ *PROPERTY:\\(SELECTOR_BRANCH([0-9]*),(.*)\\)=(.*)$" );
    static const boost::regex roiBoxPropRe( "^ *PROPERTY:\\(SELECTOR_ROIBOX([0-9]*),(.*)\\)=(.*)$" );

    boost::smatch matches;  // the list of matches
    if( boost::regex_match( line, matches, branchRe ) )
    {
        Branch branch =  string_utils::fromString< Branch >( matches[1] );
        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Branch with ID " << branch;

        // store info
        m_branches.push_back( branch );
    }
    else if( boost::regex_match( line, matches, roiBoxRe ) )
    {
        Branch parentBranch = string_utils::fromString< Branch >( matches[2] );
        RoiID roiID = string_utils::fromString< RoiID >( matches[1] );
        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": ROI with ID " << roiID << " in Branch " << parentBranch;

        // store info
        m_rois.push_back( Roi( roiID, parentBranch ) );
    }
    else if( boost::regex_match( line, matches, branchPropRe ) )
    {
        Branch branch = string_utils::fromString< Branch >( matches[1] );
        std::string prop = matches[2];
        std::string propValue = matches[3];
        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Property \"" << prop << "\" of Branch " << branch
                                                                          << " set to " << propValue;
        // store info
        m_branchProperties[ branch ] = Property( prop, propValue );
    }
    else if( boost::regex_match( line, matches, roiBoxPropRe ) )
    {
        RoiID roiID = string_utils::fromString< RoiID >( matches[1] );
        std::string prop = matches[2];
        std::string propValue = matches[3];
        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Property \"" << prop << "\" of ROI " << roiID
                                                                          << " set to " << propValue;
        // store info
        m_roiProperties[ roiID ] = Property( prop, propValue );
    }
    else
    {
        return false;
    }

    // read something
    return true;
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

