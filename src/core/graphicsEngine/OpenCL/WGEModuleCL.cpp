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

#include "WGERenderNodeCL.h"
#include "WGEModuleCL.h"

WGEModuleCL::ViewProperties::ViewProperties( const CLViewData& viewData )
{
    const osg::Matrix& pm = *viewData.m_projection.get();

    double pNear, pFar, pLeft, pRight, pTop, pBottom;

    if ( pm( 3, 3 ) == 0.0 )
    {
        // perspective projection
        //
        //  pm :
        //
        //  -------------------------------------------------------------------------------------------------------------
        //  ( 2 * near / (right - left)         |0                                  |0                              |0  )
        //  ( 0                                 |2 * near / (top - bottom)          |0                              |0  )
        //  ( (right + left) / (right - left)   |(top + bottom) / (top - bottom)    |-(far + near) / (far - near)   |-1 )
        //  ( 0                                 |0                                  |-2 * far * near / (far - near) |0  )
        //  -------------------------------------------------------------------------------------------------------------

        m_type = PERSPECTIVE;

        pNear = pm( 3, 2 ) / ( pm( 2, 2 ) - 1.0 );
        pFar = pm( 3, 2 ) / ( pm( 2, 2 ) + 1.0 );

        pLeft = pNear * ( pm( 2, 0 ) - 1.0 ) / pm( 0, 0 );
        pRight = pNear * ( pm( 2, 0 ) + 1.0 ) / pm( 0, 0 );

        pBottom = pNear * ( pm( 2, 1 ) - 1.0 ) / pm( 1, 1 );
        pTop = pNear * ( pm( 2, 1 ) + 1.0 ) / pm( 1, 1 );
    }
    else
    {
        // orthographic projection
        //
        //  pm :
        //
        //  ------------------------------------------------------------------------------------------------------------
        //  ( 2 / (right - left)                |0                                  |0                              |0 )
        //  ( 0                                 |2 / (top - bottom)                 |0                              |0 )
        //  ( 0                                 |0                                  |-2 / (far - near)              |0 )
        //  ( -(right + left) / (right - left)  |-(top + bottom) / (top - bottom)   |-(far + near) / (far - near)   |1 )
        //  ------------------------------------------------------------------------------------------------------------

        m_type = ORTHOGRAPHIC;

        pNear = ( pm( 3, 2 ) + 1.0 ) / pm( 2, 2 );
        pFar = ( pm( 3, 2 ) - 1.0 ) / pm( 2, 2 );

        pLeft = -( 1.0 + pm( 3, 0 ) ) / pm( 0, 0 );
        pRight = ( 1.0 - pm( 3, 0 ) ) / pm( 0, 0 );

        pBottom = -( 1.0 + pm( 3, 1 ) ) / pm( 1, 1 );
        pTop = ( 1.0 - pm( 3, 1 ) ) / pm( 1, 1 );
    }

    m_planeNear = static_cast< cl_float >( pNear );
    m_planeFar = static_cast< cl_float >( pFar );

    const osg::Matrix& mvm = *viewData.m_modelView.get();

    //  mvm :
    //
    //  ----------------------------------------
    //  ( RS[0][0]  |RS[0][1]   |RS[0][2]   |0 )
    //  ( RS[1][0]  |RS[1][1]   |RS[1][2]   |0 )
    //  ( RS[2][0]  |RS[2][2]   |RS[2][2]   |0 )
    //  ( tx        |ty         |tz         |1 )
    //  ----------------------------------------
    //
    //  Inverse(mvm) :
    //
    //  ----------------------------------------------------------------
    //  ( Inverse(RS)[0][0] |Inverse(RS)[0][1]  |Inverse(RS)[0][2]  |0 )
    //  ( Inverse(RS)[1][0] |Inverse(RS)[1][1]  |Inverse(RS)[1][2]  |0 )
    //  ( Inverse(RS)[2][0] |Inverse(RS)[2][2]  |Inverse(RS)[2][2]  |0 )
    //  ( invTx             |invTy              |invTz              |1 )
    //  ----------------------------------------------------------------

    double invMvm[ 4 ][ 3 ];
    double invDet;

    invMvm[ 0 ][ 0 ] = mvm( 1, 1 ) * mvm( 2, 2 ) - mvm( 1, 2 ) * mvm( 2, 1 );
    invMvm[ 0 ][ 1 ] = mvm( 0, 2 ) * mvm( 2, 1 ) - mvm( 0, 1 ) * mvm( 2, 2 );
    invMvm[ 0 ][ 2 ] = mvm( 0, 1 ) * mvm( 1, 2 ) - mvm( 0, 2 ) * mvm( 1, 1 );
    invMvm[ 1 ][ 0 ] = mvm( 1, 2 ) * mvm( 2, 0 ) - mvm( 1, 0 ) * mvm( 2, 2 );
    invMvm[ 1 ][ 1 ] = mvm( 0, 0 ) * mvm( 2, 2 ) - mvm( 0, 2 ) * mvm( 2, 0 );
    invMvm[ 1 ][ 2 ] = mvm( 0, 2 ) * mvm( 1, 0 ) - mvm( 0, 0 ) * mvm( 1, 2 );
    invMvm[ 2 ][ 0 ] = mvm( 1, 0 ) * mvm( 2, 1 ) - mvm( 1, 1 ) * mvm( 2, 0 );
    invMvm[ 2 ][ 1 ] = mvm( 0, 1 ) * mvm( 2, 0 ) - mvm( 0, 0 ) * mvm( 2, 1 );
    invMvm[ 2 ][ 2 ] = mvm( 0, 0 ) * mvm( 1, 1 ) - mvm( 0, 1 ) * mvm( 1, 0 );

    invDet = 1.0 / ( mvm( 0, 0 ) * invMvm[ 0 ][ 0 ] + mvm( 1, 0 ) * invMvm[ 0 ][ 1 ] + mvm( 2, 0 ) * invMvm[ 0 ][ 2 ] );

    invMvm[ 0 ][ 0 ] *= invDet;
    invMvm[ 0 ][ 1 ] *= invDet;
    invMvm[ 0 ][ 2 ] *= invDet;
    invMvm[ 1 ][ 0 ] *= invDet;
    invMvm[ 1 ][ 1 ] *= invDet;
    invMvm[ 1 ][ 2 ] *= invDet;
    invMvm[ 2 ][ 0 ] *= invDet;
    invMvm[ 2 ][ 1 ] *= invDet;
    invMvm[ 2 ][ 2 ] *= invDet;

    // (invTx,invTy,invTz) = -(tx,ty,tz) * Inverse(RS) ----------------------------------------------------------------

    invMvm[ 3 ][ 0 ] = -( mvm( 3, 0 ) * invMvm[ 0 ][ 0 ] + mvm( 3, 1 ) * invMvm[ 1 ][ 0 ] + mvm( 3, 2 ) * invMvm[ 2 ][ 0 ] );
    invMvm[ 3 ][ 1 ] = -( mvm( 3, 0 ) * invMvm[ 0 ][ 1 ] + mvm( 3, 1 ) * invMvm[ 1 ][ 1 ] + mvm( 3, 2 ) * invMvm[ 2 ][ 1 ] );
    invMvm[ 3 ][ 2 ] = -( mvm( 3, 0 ) * invMvm[ 0 ][ 2 ] + mvm( 3, 1 ) * invMvm[ 1 ][ 2 ] + mvm( 3, 2 ) * invMvm[ 2 ][ 2 ] );

    if ( m_type == PERSPECTIVE )
    {
        // origin = (0,0,0,1) * Inverse(mvm) --------------------------------------------------------------------------

        m_origin.s[ 0 ] = static_cast< cl_float >( invMvm[ 3 ][ 0 ] );
        m_origin.s[ 1 ] = static_cast< cl_float >( invMvm[ 3 ][ 1 ] );
        m_origin.s[ 2 ] = static_cast< cl_float >( invMvm[ 3 ][ 2 ] );
        m_origin.s[ 3 ] = static_cast< cl_float >( 1.0f );

        // originToLowerLeft = (left,bottom,-near,0) * Inverse(mvm) ---------------------------------------------------

        m_originToLowerLeft.s[ 0 ] = static_cast< cl_float >( pLeft * invMvm[ 0 ][ 0 ] + pBottom * invMvm[ 1 ][ 0 ] - pNear * invMvm[ 2 ][ 0 ] );
        m_originToLowerLeft.s[ 1 ] = static_cast< cl_float >( pLeft * invMvm[ 0 ][ 1 ] + pBottom * invMvm[ 1 ][ 1 ] - pNear * invMvm[ 2 ][ 1 ] );
        m_originToLowerLeft.s[ 2 ] = static_cast< cl_float >( pLeft * invMvm[ 0 ][ 2 ] + pBottom * invMvm[ 1 ][ 2 ] - pNear * invMvm[ 2 ][ 2 ] );
        m_originToLowerLeft.s[ 3 ] = static_cast< cl_float >( 0.0f );
    }
    else
    {
        // origin = (left,bottom,0,1) * Inverse(mvm) ------------------------------------------------------------------

        m_origin.s[ 0 ] = static_cast< cl_float >( pLeft * invMvm[ 0 ][ 0 ] + pBottom * invMvm[ 1 ][ 0 ] + invMvm[ 3 ][ 0 ] );
        m_origin.s[ 1 ] = static_cast< cl_float >( pLeft * invMvm[ 0 ][ 1 ] + pBottom * invMvm[ 1 ][ 1 ] + invMvm[ 3 ][ 1 ] );
        m_origin.s[ 2 ] = static_cast< cl_float >( pLeft * invMvm[ 0 ][ 2 ] + pBottom * invMvm[ 1 ][ 2 ] + invMvm[ 3 ][ 2 ] );
        m_origin.s[ 3 ] = static_cast< cl_float >( 0.0f );

        // originToLowerLeft = (0,0,-near,0) * Inverse(mvm) -----------------------------------------------------------

        m_originToLowerLeft.s[ 0 ] = static_cast< cl_float >( -pNear * invMvm[ 2 ][ 0 ] );
        m_originToLowerLeft.s[ 1 ] = static_cast< cl_float >( -pNear * invMvm[ 2 ][ 1 ] );
        m_originToLowerLeft.s[ 2 ] = static_cast< cl_float >( -pNear * invMvm[ 2 ][ 2 ] );
        m_originToLowerLeft.s[ 3 ] = static_cast< cl_float >( 0.0f );
    }

    // edgeX = (right - left,0,0,0) * Inverse(mvm) --------------------------------------------------------------------

    m_edgeX.s[ 0 ] = static_cast< cl_float >( ( pRight - pLeft ) * invMvm[ 0 ][ 0 ] );
    m_edgeX.s[ 1 ] = static_cast< cl_float >( ( pRight - pLeft ) * invMvm[ 0 ][ 1 ] );
    m_edgeX.s[ 2 ] = static_cast< cl_float >( ( pRight - pLeft ) * invMvm[ 0 ][ 2 ] );
    m_edgeX.s[ 3 ] = static_cast< cl_float >( 0.0f );

    // edgeY = (0,top - bottom,0,0) * Inverse(mvm) --------------------------------------------------------------------

    m_edgeY.s[ 0 ] = static_cast< cl_float >( ( pTop - pBottom ) * invMvm[ 1 ][ 0 ] );
    m_edgeY.s[ 1 ] = static_cast< cl_float >( ( pTop - pBottom ) * invMvm[ 1 ][ 1 ] );
    m_edgeY.s[ 2 ] = static_cast< cl_float >( ( pTop - pBottom ) * invMvm[ 1 ][ 2 ] );
    m_edgeY.s[ 3 ] = static_cast< cl_float >( 0.0f );
}

WGEModuleCL::CLViewData::CLViewData(): m_width( 0 ), m_height( 0 ), m_buffers( 2 )
{
    // initialize
}

WGEModuleCL::WGEModuleCL(): Referenced(), m_node( 0 ), m_boundComputed( false )
{
    // initialize
}

WGEModuleCL::~WGEModuleCL()
{
    // clean up
}

void WGEModuleCL::dirtyBound()
{
    m_boundComputed = false;

    if ( m_node != 0 )
    {
        m_node->dirtyBound();
    }
}

osg::BoundingBox WGEModuleCL::computeBoundingBox() const
{
    return osg::BoundingBox();
}

void WGEModuleCL::changeCLData( const CLDataChangeCallback& callback )
{
    osg::buffered_object< WGERenderNodeCL::PerContextInformation >& perContextInfos = m_node->m_perContextInformation;

    unsigned int size = perContextInfos.size();

    for ( unsigned int i = 0; i < size; i++ )
    {
        WGERenderNodeCL::PerContextInformation& perContextInfo = perContextInfos[ i ];

        if ( !perContextInfo.m_invalid )
        {
            callback.change( perContextInfo.m_clViewData, *perContextInfo.m_clData );
        }
    }
}

cl::NDRange WGEModuleCL::computeGlobalWorkSize( const cl::NDRange& localWorkSize, const cl::NDRange& workItems ) const
{
    if ( localWorkSize.dimensions() != workItems.dimensions() )
    {
        return cl::NDRange();
    }

    size_t dimensions = localWorkSize.dimensions();

    // check validity -------------------------------------------------------------------------------------------------

    for ( unsigned int i = 0; i < dimensions; i++ )
    {
        if ( ( localWorkSize[ i ] == 0 ) || ( workItems[ i ] == 0 ) )
        {
            return cl::NDRange();
        }
    }

    // compute global work size ---------------------------------------------------------------------------------------

    size_t globalSize[ 3 ];

    for ( unsigned int i = 0; i < dimensions; i++ )
    {
        globalSize[ i ] = workItems[ i ] - workItems[ i ] % localWorkSize[ i ];

        if ( globalSize[ i ] != workItems[ i ] )
        {
            globalSize[ i ] += localWorkSize[ i ];
        }
    }

    switch ( dimensions )
    {
        case 1:     return cl::NDRange( globalSize[ 0 ] );
        case 2:     return cl::NDRange( globalSize[ 0 ], globalSize[ 1 ] );
        case 3:     return cl::NDRange( globalSize[ 0 ], globalSize[ 1 ], globalSize[ 2 ] );
        default:    return cl::NDRange();
    }
}

