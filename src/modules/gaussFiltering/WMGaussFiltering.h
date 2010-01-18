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

#ifndef WMGAUSSFILTERING_H
#define WMGAUSSFILTERING_H

#include <map>
#include <string>
#include <vector>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Uniform>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"

#include "../../math/WVector3D.h"

class WPickHandler;

/**
 * Gauss filtering for WDataSetSingle
 *
 * \problem It works only on double value sets so far.
 * \reminder The boundary values will not be touched an considered to be zero. 
 *
 * \ingroup modules
 */
class WMGaussFiltering : public WModule
{
public:
    /**
     * Standard constructor.
     */
    WMGaussFiltering();

    /**
     * Destructor.
     */
    ~WMGaussFiltering();

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
     * Determine what to do if a property was changed.
     * \param propertyName Name of the property.
     */
    void slotPropertyChanged( std::string propertyName );

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

    /**
     * Simple convolution using a small gauss-like mask
     * \param nX number of positions in x direction
     * \param nY number of positions in y direction
     * \param nZ number of positions in z direction
     * \param x index for x direction
     * \param y index for x direction
     * \param z index for x direction
     */
    double filterSimple( size_t nX, size_t nY, size_t nZ, size_t x, size_t y, size_t z );

    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;  //!< Input connector required by this module.
    boost::shared_ptr< WModuleOutputData< WDataSetSingle > > m_output; //!< The only output of this filter module.
    boost::shared_ptr< WDataSetSingle > m_dataSet; //!< Pointer providing access to the treated data set in the whole module.
    boost::shared_ptr< WValueSet< double > > m_vals; //!< Pointer providing access to the treated value set in the whole module.
};
#endif  // WMGAUSSFILTERING_H
