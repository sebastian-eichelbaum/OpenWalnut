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

#include "core/common/WAssert.h"
#include "core/common/WProgress.h"
#include "core/common/WStringUtils.h"
#include "core/common/WTypeTraits.h"
#include "core/common/exceptions/WTypeMismatch.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/dataHandler/WDataHandlerEnums.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/exceptions/WDHValueSetMismatch.h"
#include "core/kernel/WKernel.h"
#include "WMVectorScale.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMVectorScale )

WMVectorScale::WMVectorScale() :
    WModule()
{
    // initialize
}

WMVectorScale::~WMVectorScale()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMVectorScale::factory() const
{
    return boost::shared_ptr< WModule >( new WMVectorScale() );
}

const std::string WMVectorScale::getName() const
{
    return "Vector Scaling";
}

const std::string WMVectorScale::getDescription() const
{
    return "Scales the given vector field by the given scalar field.";
}

void WMVectorScale::connectors()
{
    m_inputA = WModuleInputData< WDataSetVector >::createAndAdd( shared_from_this(), "operandA", "First operand of operation." );
    m_inputB = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "operandB", "Second operand of operation." );

    m_output = WModuleOutputData< WDataSetVector >::createAndAdd( shared_from_this(), "result", "Result of voxel-wise operation." );

    // call WModules initialization
    WModule::connectors();
}

void WMVectorScale::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );
    WModule::properties();
}

/**
 * The second visitor which got applied to the second value set. It discriminates the integral type and applies the operator in a per value
 * style.
 *
 * \tparam VSetAType The integral type of the first valueset.
 */
template< typename VSetAType >
class VisitorVSetB: public boost::static_visitor< boost::shared_ptr< WValueSetBase > >
{
public:
    /**
     * Creates visitor for the second level of cascading. Takes the first value set as parameter. This visitor applies the operation o to A and
     * B: o(A,B).
     *
     * \param vsetA the first value set
     */
    VisitorVSetB( const WValueSet< VSetAType >* const vsetA ):
        boost::static_visitor< result_type >(),
        m_vsetA( vsetA )
    {
    }

    /**
     * Visitor on the second valueset. This applies the operation.
     *
     * \tparam VSetBType the integral type of the currently visited valueset.
     * \param vsetB the valueset currently visited (B).
     *
     * \return the result of o(A,B)
     */
    template < typename VSetBType >
    result_type operator()( const WValueSet< VSetBType >* const& vsetB ) const      // NOLINT
    {
        // get best matching return scalar type
        typedef typename WTypeTraits::TypePromotion< VSetAType, VSetBType >::Result ResultT;

        dataType type = DataType< ResultT >::type;
        std::vector< ResultT > data;
        data.resize( m_vsetA->rawSize() );

        // discriminate the right operation with the correct type. It would be nicer to use some kind of strategy pattern here, but the template
        // character of the operators forbids it as template methods can't be virtual. Besides this, at some point in the module main the
        // selector needs to be queried and its index mapped to a pointer. This is what we do here.

        // apply op to each value
        const VSetAType* a = m_vsetA->rawData();
        const VSetBType* b =   vsetB->rawData();
        for( size_t i = 0; i < vsetB->rawSize(); ++i )
        {
            size_t vi = 3 * i;
            data[ vi + 0 ] = static_cast< ResultT >( a[ vi + 0 ] ) * static_cast< ResultT >( b[ i ] ) ;
            data[ vi + 1 ] = static_cast< ResultT >( a[ vi + 1 ] ) * static_cast< ResultT >( b[ i ] ) ;
            data[ vi + 2 ] = static_cast< ResultT >( a[ vi + 2 ] ) * static_cast< ResultT >( b[ i ] ) ;

        }

        // create result value set
        boost::shared_ptr< WValueSet< ResultT > > result = boost::shared_ptr< WValueSet< ResultT > >(
            new WValueSet< ResultT >( 1,
                                      3,
                                      boost::shared_ptr< std::vector< ResultT > >( new std::vector< ResultT >( data ) ),
                                      type )
        );
        return result;
    }

    /**
     * The first valueset.
     */
    const WValueSet< VSetAType >* const m_vsetA;
};

/**
 * Visitor for discriminating the type of the first valueset. It simply creates a new instance of VisitorVSetB with the proper integral type of
 * the first value set.
 */
class VisitorVSetA: public boost::static_visitor< boost::shared_ptr< WValueSetBase > >
{
public:
    /**
     * Create visitor instance. The specified valueset gets visited if the first one is visited using this visitor.
     *
     * \param vsetB The valueset to visit during this visit.
     */
    VisitorVSetA( WValueSetBase* vsetB ):
        boost::static_visitor< result_type >(),
        m_vsetB( vsetB )
    {
    }

    /**
     * Called by boost::varying during static visiting. Creates a new VisitorVSetB which finally applies the operation.
     *
     * \tparam T the real integral type of the first value set.
     * \param vsetA the first valueset currently visited.
     *
     * \return the result from the operation with this and the second value set
     */
    template < typename T >
    result_type operator()( const WValueSet< T >* const& vsetA ) const             // NOLINT
    {
        // visit the second value set as we now know the type of the first one
        VisitorVSetB< T > visitor( vsetA );
        return m_vsetB->applyFunction( visitor );
    }

    /**
     * The valueset where to cascade.
     */
    WValueSetBase* m_vsetB;
};

void WMVectorScale::moduleMain()
{
    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_inputA->getDataChangedCondition() );
    m_moduleState.add( m_inputB->getDataChangedCondition() );
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
        if( m_inputA->handledUpdate() || m_inputB->handledUpdate() )
        {
            boost::shared_ptr< WDataSetVector > dataSetA = m_inputA->getData();
            boost::shared_ptr< WDataSetScalar > dataSetB = m_inputB->getData();
            if( !dataSetA || !dataSetB )
            {
                // reset output if input was reset/disconnected
                debugLog() << "Resetting output.";
                m_output->reset();
                continue;
            }

            // the first value-set is always needed -> grab it
            boost::shared_ptr< WValueSetBase > valueSetA = dataSetA->getValueSet();
            boost::shared_ptr< WValueSetBase > valueSetB = dataSetB->getValueSet();

             // use a custom progress combiner
            boost::shared_ptr< WProgress > prog = boost::shared_ptr< WProgress >(
                new WProgress( "Applying operator on data" ) );
            m_progress->addSubProgress( prog );

            // apply the operation to each voxel
            debugLog() << "Processing ...";

            // both value sets need to be the same
            bool match = ( valueSetA->rawSize() == 3 * valueSetB->rawSize() );
            if( !match )
            {
                errorLog() << "Rawsize Mismatch: " << valueSetA->rawSize() << " != 3 * " << valueSetB->rawSize();
                throw WDHValueSetMismatch( std::string( "The both value sets are not of equal size." ) );
            }

            // this keeps the result
            boost::shared_ptr< WValueSetBase > newValueSet;
            VisitorVSetA visitor( valueSetB.get() );    // the visitor cascades to the second value set
            newValueSet = valueSetA->applyFunction( visitor );

            // Create the new dataset and export it
            m_output->updateData( boost::shared_ptr<WDataSetVector>( new WDataSetVector( newValueSet, m_inputA->getData()->getGrid() ) ) );

            // done
            prog->finish();
            m_progress->removeSubProgress( prog );
        }
    }
}

