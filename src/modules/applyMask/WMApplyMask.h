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

#ifndef WMAPPLYMASK_H
#define WMAPPLYMASK_H

#include <map>
#include <string>
#include <vector>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Uniform>

#include "core/common/math/linearAlgebra/WLinearAlgebra.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"


class WPickHandler;

/**
 * Set all voxels in a dataset to zero if the other dataset is zero there.
 *
 * \ingroup modules
 */

class WMApplyMask : public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMApplyMask();

    /**
     * Destructor.
     */
    ~WMApplyMask();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description of module.
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


protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

private:
    /**
     * Apply the mask to the data.
     * \param valSet the values which are set to zero if the mask is zero
     * \param type The enumeration representing of the data type of contained in the value set.
     */
    template< typename T > void applyMask( boost::shared_ptr< WValueSet< T > > valSet, dataType type );

    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_dataInput;  //!< Input connector for getting the data.
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_maskInput;  //!< Input connector for getting the mask.
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output; //!< The only output of this mask module.
    boost::shared_ptr< WDataSetScalar > m_dataSet; //!< Pointer providing access to the data set in the whole module.
    boost::shared_ptr< WDataSetScalar > m_dataSetOut; //!< Pointer providing access to the resulting data set in the whole module.
    boost::shared_ptr< WDataSetScalar > m_mask; //!< Pointer providing access to the mask in the whole module.
};

#endif  // WMAPPLYMASK_H
