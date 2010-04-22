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

#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

#include "../../common/WCondition.h"
#include "../../common/WPropertyTypes.h"
#include "../../common/math/WMatrix.h"
#include "../../common/math/WVector3D.h"
#include "../../dataHandler/WDataSetFiberVector.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"


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

    wmath::WMatrix< double > m_transformationMatrix; //!< matrix which is multiplied with each point to linear transform it
    wmath::WVector3D m_transformationVector; //!< vector which is added to each point (after multiplication with the matrix) to translate it

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

inline const std::string WMFiberTransform::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return std::string( "Fiber Transform" );
}

inline const std::string WMFiberTransform::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    // See "src/modules/template/" for an extensively documented example.
    return std::string( "Transforms a fiber dataset" );
}

#endif  // WMFIBERTRANSFORM_H