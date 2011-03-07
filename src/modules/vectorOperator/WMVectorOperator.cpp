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

#include <stdint.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/variant.hpp>

#include "../../common/math/WPosition.h"
#include "../../common/math/WVector3D.h"
#include "../../common/WAssert.h"
#include "../../common/WProgress.h"
#include "../../common/WStringUtils.h"
#include "../../common/WTypeTraits.h"
#include "../../common/exceptions/WTypeMismatch.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/WDataHandlerEnums.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/exceptions/WDHValueSetMismatch.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../kernel/WKernel.h"
#include "WMVectorOperator.xpm"
#include "WMVectorOperator.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMVectorOperator )

WMVectorOperator::WMVectorOperator() :
    WModule()
{
    // initialize
}

WMVectorOperator::~WMVectorOperator()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMVectorOperator::factory() const
{
    return boost::shared_ptr< WModule >( new WMVectorOperator() );
}

const char** WMVectorOperator::getXPMIcon() const
{
    return WMVectorOperator_xpm;
}

const std::string WMVectorOperator::getName() const
{
    return "Vector Operator";
}

const std::string WMVectorOperator::getDescription() const
{
    return "Applies an selected operator on a specified vector field.";
}

void WMVectorOperator::connectors()
{
    m_inputA = WModuleInputData< WDataSetVector >::createAndAdd( shared_from_this(), "operandA", "First operand of operation.." );

    m_output = WModuleOutputData< WDataSetScalar >::createAndAdd( shared_from_this(), "result", "Result of voxel-wise operation( A, B )." );

    // call WModules initialization
    WModule::connectors();
}

void WMVectorOperator::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // create a list of operations here
    m_operations = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_operations->addItem( "Length", "Length of the vector." );          // NOTE: you can add XPM images here.
    m_operations->addItem( "Curvature", "Curvature at each voxel." );

    m_opSelection = m_properties->addProperty( "Operation", "The operation to apply on A and B.", m_operations->getSelectorFirst(),
                                               m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_opSelection );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_opSelection );

    WModule::properties();
}

template< typename T >
T opLength( const WVector3D& vec,
            const WVector3D& /*mx*/, const WVector3D& /*px*/,
            const WVector3D& /*my*/, const WVector3D& /*py*/,
            const WVector3D& /*mz*/, const WVector3D& /*pz*/ )
{
    return vec.length();
}

template< typename T >
T opCurvature( const WVector3D& vec,
               const WVector3D& mx, const WVector3D& px,
               const WVector3D& my, const WVector3D& py,
               const WVector3D& mz, const WVector3D& pz )
{
    // get partial differentiation in x direction:
    WVector3D dx = ( px - mx ) / 2.0;    // NOTE: step size h=2
    WVector3D dy = ( py - my ) / 2.0;
    WVector3D dz = ( pz - mz ) / 2.0;

    // get second derivative of tangent curve
    WVector3D L2 = ( vec.x() * dx ) + ( vec.y() * dy ) + ( vec.z() * dz );

    WVector3D::value_type l = vec.length();
    return ( vec.crossProduct( L2 ) ).length() / ( l * l * l );
}

/**
 * Get the ID of a voxel with the given coordinates.
 *
 * \param xDim number of voxels in x direction
 * \param yDim number of voxels in y direction
 * \param zDim number of voxels in z direction
 * \param x x coordinate of point to get index for
 * \param y y coordinate of point to get index for
 * \param z z coordinate of point to get index for
 * \param offset which coordinate? 0 = x, 3 = w
 * \param elements number of elements
 *
 * \return the index
 */
size_t getId( size_t xDim, size_t yDim, size_t /*zDim*/, size_t x, size_t y, size_t z, size_t offset = 0, size_t elements = 1 )
{
    return offset + ( elements * ( z * xDim * yDim + y * xDim + x ) );
}

/**
 * Visitor for discriminating the type of the first valueset.
 */
class VisitorVSetA: public boost::static_visitor< boost::shared_ptr< WValueSetBase > >
{
public:
    /**
     * Create visitor instance.
     *
     * \param opIdx The operator index.
     * \param grid the underlying grid
     */
    VisitorVSetA( boost::shared_ptr< WGridRegular3D > grid, size_t opIdx = 0 ):
        boost::static_visitor< result_type >(),
        m_grid( grid ),
        m_opIdx( opIdx )
    {
    }

    /**
     * Called by boost::varying during static visiting.
     *
     * \tparam T the real integral type of the first value set.
     * \param vsetA the first valueset currently visited.
     *
     * \return the result from the operation
     */
    template < typename T >
    result_type operator()( const WValueSet< T >* const& vsetA ) const             // NOLINT
    {
        // get some info
        std::vector< T > data;
        data.resize( vsetA->size() );

        // discriminate the right operation with the correct type. It would be nicer to use some kind of strategy pattern here, but the template
        // character of the operators forbids it as template methods can't be virtual. Besides this, at some point in the module main the
        // selector needs to be queried and its index mapped to a pointer. This is what we do here.
        boost::function< T( const WVector3D&,
                            const WVector3D&, const WVector3D&,
                            const WVector3D&, const WVector3D&,
                            const WVector3D&, const WVector3D& ) > op;
        switch ( m_opIdx )
        {
            case 1:
                op = &opCurvature< T >;
                break;
            case 0:
            default:
                op = &opLength< T >;
                break;
        }

        // some info needed for indexing the vector components
        size_t nX = m_grid->getNbCoordsX();
        size_t nY = m_grid->getNbCoordsY();
        size_t nZ = m_grid->getNbCoordsZ();

        // apply op to each value
        // iterate field
        for( size_t z = 1; z < nZ - 1; z++ )
        {
            for( size_t y = 1; y < nY - 1; y++ )
            {
                for( size_t x = 1; x < nX - 1; x++ )
                {
                    // this is ugly. We'll fix this crap with the upcoming new data handler
                    size_t idx = getId( nX, nY, nZ, x, y, z );
                    WVector3D vec = vsetA->getVector3D( idx );

                    // also get the neighbours
                    WVector3D mx = vsetA->getVector3D( getId( nX, nY, nZ, x - 1, y, z ) );
                    WVector3D px = vsetA->getVector3D( getId( nX, nY, nZ, x + 1, y, z ) );
                    WVector3D my = vsetA->getVector3D( getId( nX, nY, nZ, x, y - 1, z ) );
                    WVector3D py = vsetA->getVector3D( getId( nX, nY, nZ, x, y + 1, z ) );
                    WVector3D mz = vsetA->getVector3D( getId( nX, nY, nZ, x, y, z - 1 ) );
                    WVector3D pz = vsetA->getVector3D( getId( nX, nY, nZ, x, y, z + 1 ) );

                    data[ idx ] = op( vec, mx, px, my, py, mz, pz );
                }
            }
        }

        // init the border values
        for( size_t z = 0; z < 2; z++ )
        {
            for( size_t y = 1; y < 2; y++ )
            {
                for( size_t x = 1; x < 2; x++ )
                {
                    size_t idx = getId( nX, nY, nZ, x * ( nX - 1 ), y * ( nY - 1 ), z * ( nZ - 1 ) );
                    data[ idx ] = 0.0;
                }
            }
        }

        // create result value set
        return boost::shared_ptr< WValueSet< T > >( new WValueSet< T >( 0,
                                                                        1,
                                                                        boost::shared_ptr< std::vector< T > >(
                                                                            new std::vector< T >( data ) ),
                                                                        DataType< T >::type ) );
    }

    /**
     * The underlying grid.
     */
    boost::shared_ptr< WGridRegular3D > m_grid;

    /**
     * The operator index.
     */
    size_t m_opIdx;
};

void WMVectorOperator::moduleMain()
{
    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_inputA->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        // has the data changed?
        if( m_opSelection->changed() || m_inputA->handledUpdate() )
        {
            boost::shared_ptr< WDataSetVector > dataSetA = m_inputA->getData();

            WItemSelector s = m_opSelection->get( true );

            // valid data?
            if( dataSetA )
            {
                boost::shared_ptr< WValueSetBase > valueSetA = dataSetA->getValueSet();

                // use a custom progress combiner
                boost::shared_ptr< WProgress > prog = boost::shared_ptr< WProgress >(
                    new WProgress( "Applying operator on data" ) );
                m_progress->addSubProgress( prog );

                // apply the operation to each voxel
                debugLog() << "Processing ...";
                boost::shared_ptr< WValueSetBase > newValueSet = valueSetA->applyFunction( VisitorVSetA(
                    boost::shared_dynamic_cast< WGridRegular3D >( dataSetA->getGrid() ), s )
                );

                // Create the new dataset and export it
                m_output->updateData( boost::shared_ptr<WDataSetScalar>( new WDataSetScalar( newValueSet, m_inputA->getData()->getGrid() ) ) );

                // done
                prog->finish();
                m_progress->removeSubProgress( prog );
            }
            else
            {
                debugLog() << "Resetting output.";
                m_output->reset();
            }
        }
    }
}

