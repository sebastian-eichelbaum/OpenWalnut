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

#ifndef WMDATATYPECONVERSION_H
#define WMDATATYPECONVERSION_H

#include <map>
#include <string>
#include <vector>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Uniform>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"

#include "core/common/math/linearAlgebra/WLinearAlgebra.h"

/**
 * Provides a new field with the approximately the same data, but with another data type. E.g., it
 * can convert double into float.

 * \ingroup modules
 */
class WMDataTypeConversion : public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMDataTypeConversion();

    /**
     * Destructor.
     */
    ~WMDataTypeConversion();

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

    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;  //!< Input connector required by this module.
    boost::shared_ptr< WModuleOutputData< WDataSetSingle > > m_output; //!< The only output of this filter module.
    boost::shared_ptr< WDataSetSingle > m_dataSet; //!< Pointer providing access to the treated data set in the whole module.

    /**
     * The heart of this module. it does the conversion of the values.
     * \param vals valueset of data set.
     */
    template< typename T > void convertDataSet( boost::shared_ptr< WValueSet< T > > vals );
};
#endif  // WMDATATYPECONVERSION_H
