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

#ifndef WMDETTRACTCULLING_H
#define WMDETTRACTCULLING_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "core/common/datastructures/WFiber.h"
#include "core/common/WFlag.h"
#include "core/dataHandler/WDataSetFiberVector.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"

/**
 * Removes deterministic tracts and therefore implements a preprocessing step
 * for the deterministic tract clustering ala Zhang http://dx.doi.org/10.1109/TVCG.2008.52 .
 * It removes tracts which are considered to be not much useful for this special clustering approach.
 *
 * \ingroup modules
 */
class WMDetTractCulling : public WModule
{
friend class WMDetTractCullingTest;
public:
    /**
     * Constructs deterministic tract culling module.
     */
    WMDetTractCulling();

    /**
     * Destructs this DetTractCulling module.
     */
    virtual ~WMDetTractCulling();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     * \return The icon.
     */
    virtual const char** getXPMIcon() const;

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Detect and removes tracts that have a short distance in terms of the
     * dSt metric and are below the threshold given via the member
     * m_dSt_culling_t.
     *
     * \note This might take a while with 70,000 tracts approx 2h.
     */
    virtual void cullOutTracts();

    /**
     * Generates new data set out of the tracts which are not marked "unused"
     * and saves it in the file as given via the savePath property.
     *
     * \param unusedTracts Vector of bool marking tracts not to use with true.
     */
    virtual void saveGainedTracts( const std::vector< bool >& unusedTracts );

    /**
     * Generates the file name for saving the culled tracts out of some
     * culling parameters: the proximity threshold and the dSt distance.
     *
     * \param dataFileName The file name from which the data is loaded so only the extension will change
     *
     * \return Path in which to store the culled tracts.
     */
    boost::filesystem::path saveFileName( std::string dataFileName ) const;

    /**
     * Input connector for a tract dataset.
     */
    boost::shared_ptr< WModuleInputData< WDataSetFibers > >  m_tractIC;

    /**
     * Output connector for the culled tracts
     */
    boost::shared_ptr< WModuleOutputData< WDataSetFibers > > m_tractOC;

    /**
     * Pointer to the tract data set in WDataSetFiberVector format
     */
    boost::shared_ptr< WDataSetFiberVector > m_dataset;

    /**
     * Pointer to the tract data set in WDataSetFibers format
     */
    boost::shared_ptr< WDataSetFibers > m_rawDataset;

    /**
     * A condition which indicates complete recomputation
     */
    boost::shared_ptr< WCondition > m_recompute;

    /**
     * Minimum distance of two different tracts. If below, the shorter tract is culled out
     */
    WPropDouble m_dSt_culling_t;

    /**
     * Minimum distance of points of two tracts which should be considered
     */
    WPropDouble m_proximity_t;

    /**
     *  If true, remaining tracts are saved to a file
     */
    WPropBool  m_saveCulledCurves;

    /**
     * Path where remaining tracts should be stored
     */
    WPropFilename m_savePath;

    /**
     * Trigger button for starting the long time consuming culling operation
     */
    WPropTrigger m_run;

    /**
     * Displays the number of tracts which are processed
     */
    WPropInt m_numTracts;

    /**
     * Displays the number of tracts which were removed
     */
    WPropInt m_numRemovedTracts;


private:
};

inline const std::string WMDetTractCulling::getName() const
{
    return std::string( "Deterministic Tract Culling" );
}

inline const std::string WMDetTractCulling::getDescription() const
{
    return std::string( "Removes deterministic tracts from a WDataSetFiberVector" );
}

#endif  // WMDETTRACTCULLING_H
