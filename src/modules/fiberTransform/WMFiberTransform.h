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

#ifndef WMFIBERTRANSFORM_H
#define WMFIBERTRANSFORM_H

#include <string>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * Transforms a fiber dataset
 * \ingroup modules
 */
class WMFiberTransform : public WModule
{
public:
    /**
     * Constructor
     */
    WMFiberTransform();

    /**
    * Destructor
    */
    virtual ~WMFiberTransform();

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
    boost::shared_ptr< WModuleInputData< WDataSetFibers > >  m_fiberInput; //!< Input connector for a fiber dataset.
    boost::shared_ptr< WDataSetFibers >                      m_rawDataset; //!< Pointer to the fiber data set in WDataSetFibers format
    boost::shared_ptr< WModuleOutputData< WDataSetFibers > > m_output; //!< Output connector for the culled fibers

    boost::shared_ptr< WCondition > m_recompute; //!< A condition which indicates complete recomputation

    WPropBool     m_save; //!< If true, transformed fibers are saved to a file
    WPropFilename m_savePath; //!< Path where transformed fibers should be stored
    WPropTrigger  m_run; //!< Indicates if the algorithm should start

    WPropPosition   m_translationProp; //!< Translation part of the transformation.
    WPropPosition   m_matrix0Prop; //!< Row 0 of matrix part of the transformation.
    WPropPosition   m_matrix1Prop; //!< Row 1 of matrix part of the transformation.
    WPropPosition   m_matrix2Prop; //!< Row 2 of matrix part of the transformation.
    WPropPosition   m_matrix3Prop; //!< Row 3 of matrix part of the transformation.

    /**
     * ReTransforms the scene.
     */
    void update();

    /**
     * Generates the file name for saving the transformed fibers.
     *
     * \param dataFileName The file name from which the data is loaded so only the extension will change
     * \return Path in which to store the transformed fibers.
     */
    boost::filesystem::path saveFileName( std::string dataFileName ) const;
};

#endif  // WMFIBERTRANSFORM_H
