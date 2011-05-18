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

#ifndef WMSUBTRACTDATASETSCALAR_H
#define WMSUBTRACTDATASETSCALAR_H

#include <string>
#include <vector>
#include <algorithm>

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"
#include "core/common/WThreadedFunction.h"
#include "core/dataHandler/WDataSetScalar.h"

/**
 * \class WMSubtractDataSetScalar
 *
 * Voxel-wise subtraction of two scalar datasets.
 *
 * \ingroup modules
 */
class WMSubtractDataSetScalar: public WModule
{
public:

    /**
     * Standard constructor.
     */
    WMSubtractDataSetScalar();

    /**
     * Standard destructor.
     */
    virtual ~WMSubtractDataSetScalar();

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
     * Subtract m_first and m_second.
     */
    void subtract();

    /**
     * Subtract m_first and m_second.
     *
     * \tparam T The datatype of he valuesets.
     */
    template< typename T >
    void subtractTyped();

    //! The first input Connector.
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input1;

    //! The second input Connector.
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input2;

    //! The output Connector.
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output;

    //! The first input dataset.
    boost::shared_ptr< WDataSetScalar > m_first;

    //! The second input dataset.
    boost::shared_ptr< WDataSetScalar > m_second;

    //! The result, i.e. m_first - m_second.
    boost::shared_ptr< WDataSetScalar > m_result;
};

template< typename T >
void WMSubtractDataSetScalar::subtractTyped()
{
    typedef WValueSet< T > VS;
    typedef boost::shared_ptr< VS > VSPtr;

    VSPtr firstValueSet = boost::shared_dynamic_cast< VS >( m_first->getValueSet() );
    VSPtr secondValueSet = boost::shared_dynamic_cast< VS >( m_second->getValueSet() );

    boost::shared_ptr< std::vector< T > > values = boost::shared_ptr< std::vector< T > >( new std::vector< T >( m_first->getGrid()->size() ) );
    for( std::size_t k = 0; k < m_first->getGrid()->size(); ++k )
    {
        ( *values )[k] = std::max( 0, firstValueSet->rawData()[ k ] - secondValueSet->rawData()[ k ] );
    }
    VSPtr vs( new VS( 0, 1, values, DataType< T >::type ) );
    m_result = boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( vs, m_first->getGrid() ) );
}

#endif  // WMSUBTRACTDATASETSCALAR_H
