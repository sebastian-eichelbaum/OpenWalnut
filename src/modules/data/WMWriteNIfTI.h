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

#ifndef WMWRITENIFTI_H
#define WMWRITENIFTI_H

#include <map>
#include <string>
#include <vector>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Uniform>

#include "core/dataHandler/WDataSetScalar.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"

#include "core/common/math/linearAlgebra/WLinearAlgebra.h"

class WPickHandler;

/**
 * Write the input dataset to a NIfTI file
 * \problem It works only on uint8_t values sets so far.
 * \problem Might crash OpenWalnut
 *
 * \ingroup modules
 */
class WMWriteNIfTI : public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMWriteNIfTI();

    /**
     * Destructor.
     */
    ~WMWriteNIfTI();

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
     * Allows one to get a void* out of WValueSet.
     * \param returnData the casted data will be returned through this pointer.
     */
    template< typename T > void castData( void*& returnData );

    /**
     * This performs all work necessary to actually write the data to the file
     */
    void writeToFile();

    /**
     * The filename property -> where to write the nifty file
     */
    WPropFilename m_filename;
    WPropTrigger  m_saveTriggerProp; //!< This property triggers the actual writing,

    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;  //!< Input connector required by this module.
    boost::shared_ptr< WDataSetSingle > m_dataSet; //!< Pointer providing access to the treated data set in the whole module.
};
#endif  // WMWRITENIFTI_H
