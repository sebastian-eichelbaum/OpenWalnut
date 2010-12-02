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
#include "WMScalarOperator.xpm"
#include "WMScalarOperator.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMScalarOperator )

WMScalarOperator::WMScalarOperator() :
    WModule()
{
    // initialize
}

WMScalarOperator::~WMScalarOperator()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMScalarOperator::factory() const
{
    return boost::shared_ptr< WModule >( new WMScalarOperator() );
}

const char** WMScalarOperator::getXPMIcon() const
{
    return WMScalarOperator_xpm;
}

const std::string WMScalarOperator::getName() const
{
    return "Scalar Operator";
}

const std::string WMScalarOperator::getDescription() const
{
    return "Applies an selected operator on both datasets on a per-voxel basis. Until now, it assumes that both grids are the same.";
}

void WMScalarOperator::connectors()
{
    m_inputA = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "operandA", "First operand of operation( A, B )." );
    m_inputB = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "operandB", "Second operand of operation( A, B )." );

    m_output = WModuleOutputData< WDataSetScalar >::createAndAdd( shared_from_this(), "result", "Result of voxel-wise operation( A, B )." );

    // call WModules initialization
    WModule::connectors();
}

void WMScalarOperator::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    // create a list of operations here
    m_operations = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_operations->addItem( "A + B", "Add A to B." );          // NOTE: you can add XPM images here.
    m_operations->addItem( "A - B", "Subtract B from A." );
    m_operations->addItem( "A * B", "Scale A by B." );
    m_operations->addItem( "A / B", "Divide A by B." );
    m_operations->addItem( "abs( A - B )", "Absolute value of A - B." );

    m_opSelection = m_properties->addProperty( "Operation", "The operation to apply on A and B.", m_operations->getSelectorFirst(),
                                               m_propCondition );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_opSelection );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_opSelection );

    WModule::properties();
}

/**
 * Operator applying some op to both arguments.
 *
 * \tparam T Type of each parameter and the result
 * \param a the first operant
 * \param b the second operant
 *
 * \return result
 */
template< typename T >
inline T opPlus( T a, T b )
{
    return a + b;
}

/**
 * Operator applying some op to both arguments.
 *
 * \tparam T Type of each parameter and the result
 * \param a the first operant
 * \param b the second operant
 *
 * \return result
 */
template< typename T >
inline T opMinus( T a, T b )
{
    return a - b;
}

/**
 * Operator applying some op to both arguments.
 *
 * \tparam T Type of each parameter and the result
 * \param a the first operant
 * \param b the second operant
 *
 * \return result
 */
template< typename T >
inline T opTimes( T a, T b )
{
    return a * b;
}

/**
 * Operator applying some op to both arguments.
 *
 * \tparam T Type of each parameter and the result
 * \param a the first operant
 * \param b the second operant
 *
 * \return result
 */
template< typename T >
inline T opDiv( T a, T b )
{
    return a / b;
}

/**
 * Some math ops.
 */
namespace math
{
    using std::abs;

    /**
     * Absolute value of the specified parameter. This is a template specialization for std::abs as it does not allow unsigned types.
     *
     * \param u the value for which the absolute value should be returned
     *
     * \return absolute of u
     */
    inline uint8_t abs( uint8_t u )
    {
        return u;
    }

    /**
     * Absolute value of the specified parameter. This is a template specialization for std::abs as it does not allow unsigned types.
     *
     * \param u the value for which the absolute value should be returned
     *
     * \return absolute of u
     */
    inline uint16_t abs( uint16_t u )
    {
        return u;
    }

    /**
     * Absolute value of the specified parameter. This is a template specialization for std::abs as it does not allow unsigned types.
     *
     * \param u the value for which the absolute value should be returned
     *
     * \return absolute of u
     */
    inline uint32_t abs( uint32_t u )
    {
        return u;
    }

    /**
     * Absolute value of the specified parameter. This is a template specialization for std::abs as it does not allow unsigned types.
     *
     * \param u the value for which the absolute value should be returned
     *
     * \return absolute of u
     */
    inline uint64_t abs( uint64_t u )
    {
        return u;
    }
}

/**
 * Operator applying some op to both arguments.
 *
 * \tparam T Type of each parameter and the result
 * \param a the first operant
 * \param b the second operant
 *
 * \return result
 */
template< typename T >
inline T opAbsMinus( T a, T b )
{
    return math::abs( a - b );
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
     * \param opIdx The operator index. Depending on the index, the right operation is selected
     */
    VisitorVSetB( const WValueSet< VSetAType >* const vsetA, size_t opIdx = 0 ):
        boost::static_visitor< result_type >(),
        m_vsetA( vsetA ),
        m_opIdx( opIdx )
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

        size_t order = m_vsetA->order();
        size_t dim = m_vsetA->dimension();
        dataType type = DataType< ResultT >::type;
        std::vector< ResultT > data;
        data.resize( m_vsetA->rawSize() );

        // discriminate the right operation with the correct type. It would be nicer to use some kind of strategy pattern here, but the template
        // character of the operators forbids it as template methods can't be virtual. Besides this, at some point in the module main the
        // selector needs to be queried and its index mapped to a pointer. This is what we do here.
        boost::function< ResultT( ResultT, ResultT ) > op;
        switch ( m_opIdx )
        {
            case 1:
                op = &opMinus< ResultT >;
                break;
            case 2:
                op = &opTimes< ResultT >;
                break;
            case 3:
                op = &opDiv< ResultT >;
                break;
            case 4:
                op = &opAbsMinus< ResultT >;
                break;
            case 0:
            default:
                op = &opPlus< ResultT >;
                break;
        }

        // apply op to each value
        const VSetAType* a = m_vsetA->rawData();
        const VSetBType* b =   vsetB->rawData();
        for ( size_t i = 0; i < m_vsetA->rawSize(); ++i )
        {
            data[ i ] = op( static_cast< ResultT >( a[ i ] ), static_cast< ResultT >( b[ i ] ) );
        }

        // create result value set
        boost::shared_ptr< WValueSet< ResultT > > result = boost::shared_ptr< WValueSet< ResultT > >(
            new WValueSet< ResultT >( order, dim, data, type )
        );
        return result;
    }

    /**
     * The first valueset.
     */
    const WValueSet< VSetAType >* const m_vsetA;

    /**
     * The operator index.
     */
    size_t m_opIdx;
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
     * \param opIdx The operator index. Forwarded to VisitorVSetB
     */
    VisitorVSetA( WValueSetBase* vsetB, size_t opIdx = 0 ):
        boost::static_visitor< result_type >(),
        m_vsetB( vsetB ),
        m_opIdx( opIdx )
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
        VisitorVSetB< T > visitor( vsetA, m_opIdx );
        return m_vsetB->applyFunction( visitor );
    }

    /**
     * The valueset where to cascade.
     */
    WValueSetBase* m_vsetB;

    /**
     * The operator index.
     */
    size_t m_opIdx;
};

void WMScalarOperator::moduleMain()
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
        if( m_opSelection->changed() || m_inputA->handledUpdate() || m_inputB->handledUpdate() )
        {
            boost::shared_ptr< WDataSetScalar > dataSetA = m_inputA->getData();
            boost::shared_ptr< WDataSetScalar > dataSetB = m_inputB->getData();

            WItemSelector s = m_opSelection->get( true );

            // valid data?
            if( dataSetA && dataSetB )
            {
                boost::shared_ptr< WValueSetBase > valueSetA = dataSetA->getValueSet();
                boost::shared_ptr< WValueSetBase > valueSetB = dataSetB->getValueSet();

                // both value sets need to be the same
                bool match = ( valueSetA->dimension() == valueSetB->dimension() ) &&
                             ( valueSetA->order() == valueSetB->order() ) &&
                             ( valueSetA->rawSize() == valueSetB->rawSize() );
                if ( !match )
                {
                    throw WDHValueSetMismatch( std::string( "The both value sets are not of equal size, dimension and order." ) );
                }

                // use a custom progress combiner
                boost::shared_ptr< WProgress > prog = boost::shared_ptr< WProgress >(
                    new WProgress( "Applying operator on data" ) );
                m_progress->addSubProgress( prog );

                // apply the operation to each voxel
                debugLog() << "Processing ...";
                VisitorVSetA visitor( valueSetB.get(), s );    // the visitor cascades to the second value set
                boost::shared_ptr< WValueSetBase > newValueSet = valueSetA->applyFunction( visitor );

                // Create the new dataset and export it
                m_output->updateData( boost::shared_ptr<WDataSetScalar>( new WDataSetScalar( newValueSet, m_inputA->getData()->getGrid() ) ) );

                // done
                prog->finish();
                m_progress->removeSubProgress( prog );
            }
        }
    }
}
