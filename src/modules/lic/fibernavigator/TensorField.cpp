// vim:nolist
/*
 * TensorField.cpp
 *
 *  Created on: 16.11.2008
 *      Author: ralph
 */

#include <boost/shared_ptr.hpp>

#include "TensorField.h"
#include "../fantom/FVector.h"
#include "../../../common/WAssert.h"
#include "../../../dataHandler/WDataSetVector.h"

TensorField::TensorField( boost::shared_ptr< WDataSetVector > vectors )
    : m_cells( vectors->getGrid()->size() ),
      m_order( vectors->getValueSet()->order() ),
      m_posDim( vectors->getValueSet()->dimension() ),
      m_vectors( vectors )
{
    theField.clear();
    theField.reserve(m_cells);
    boost::shared_ptr< WValueSet< float > > values = boost::shared_dynamic_cast< WValueSet< float > >( vectors->getValueSet() );
    WAssert( values, "VectorDataSets based not on float aren't currently supported" );
    WAssert( values->rawSize() == vectors->getGrid()->size() * 3, "Raw values does not fit to voxel numbers" );
    for( int i = 0 ; i < m_cells ; ++i )
    {
        FTensor t( 3, 1, true );
        t.setValue(0, values->getScalar( i * 3 ) );
        t.setValue(1, values->getScalar( i * 3 + 1 ) );
        t.setValue(2, values->getScalar( i * 3 + 2 ) );
        theField.push_back(t);
    }
}

TensorField::~TensorField()
{
	theField.clear();
}

FTensor TensorField::getInterpolatedVector(float x, float y, float z)
{
    bool success = false;
    wmath::WVector3D result = m_vectors->interpolate( wmath::WPosition( x, y, z ), &success );
    WAssert( success, "Vector interpolation was not successful" );
    return FTensor( FArray( result[0], result[1], result[2] ) );

//    TODO(lmath) use std::min/std::max and grid for that
//    int nx = wxMin(m_dh->columns-1, wxMax(0,(int)x));
//    int ny = wxMin(m_dh->rows   -1, wxMax(0,(int)y));
//    int nz = wxMin(m_dh->frames -1, wxMax(0,(int)z));
//
//    float xMult = x - (int)x;
//    float yMult = y - (int)y;
//    float zMult = z - (int)z;
//
//    int nextX = wxMin(m_dh->columns-1, nx+1);
//    int nextY = wxMin(m_dh->rows-1, ny+1);
//    int nextZ = wxMin(m_dh->frames-1, nz+1);
//
//    int xyzIndex  =  nx + ny * m_dh->columns + nz * m_dh->columns * m_dh->rows;
//    int x1yzIndex = nextX + ny * m_dh->columns + nz * m_dh->columns * m_dh->rows;
//    int xy1zIndex =  nx + nextY  * m_dh->columns + nz * m_dh->columns * m_dh->rows;
//    int x1y1zIndex = nextX + nextY * m_dh->columns + nz * m_dh->columns * m_dh->rows;
//    int xyz1Index  =  nx + ny * m_dh->columns + nextZ * m_dh->columns * m_dh->rows;
//    int x1yz1Index = nextX + ny * m_dh->columns + nextZ * m_dh->columns * m_dh->rows;
//    int xy1z1Index =  nx + nextY  * m_dh->columns + nextZ * m_dh->columns * m_dh->rows;
//    int x1y1z1Index = nextX + nextY * m_dh->columns + nextZ * m_dh->columns * m_dh->rows;
//
//    FTensor txyz = getTensorAtIndex(xyzIndex);
//    FTensor tx1yz = getTensorAtIndex(x1yzIndex);
//    FTensor txy1z = getTensorAtIndex(xy1zIndex);
//    FTensor tx1y1z = getTensorAtIndex(x1y1zIndex);
//    FTensor txyz1 = getTensorAtIndex(xyz1Index);
//    FTensor tx1yz1 = getTensorAtIndex(x1yz1Index);
//    FTensor txy1z1 = getTensorAtIndex(xy1z1Index);
//    FTensor tx1y1z1 = getTensorAtIndex(x1y1z1Index);
//
//
//    FMatrix matxyz = createMatrix(txyz, txyz);
//    FMatrix matx1yz = createMatrix(tx1yz, tx1yz);
//    FMatrix matxy1z = createMatrix(txy1z, txy1z);
//    FMatrix matx1y1z = createMatrix(tx1y1z, tx1y1z);
//    FMatrix matxyz1 = createMatrix(txyz1, txyz1);
//    FMatrix matx1yz1 = createMatrix(tx1yz1, tx1yz1);
//    FMatrix matxy1z1 = createMatrix(txy1z1, txy1z1);
//    FMatrix matx1y1z1 = createMatrix(tx1y1z1, tx1y1z1);
//
//    FMatrix i1 = matxyz * (1. - zMult) + matxyz1 * zMult;
//    FMatrix i2 = matxy1z * (1. - zMult) + matxy1z1 * zMult;
//    FMatrix j1 = matx1yz * (1. - zMult) + matx1yz1 * zMult;
//    FMatrix j2 = matx1y1z * (1. - zMult) + matx1y1z1 * zMult;
//
//    FMatrix w1 = i1 * (1. - yMult) + i2 * yMult;
//    FMatrix w2 = j1 * (1. - yMult) + j2 * yMult;
//
//    FMatrix matResult = w1 * (1. - xMult) + w2 * xMult;
//    std::vector<FArray>evecs;
//    FArray vals(0,0,0);
//
//    matResult.getEigenSystem(vals, evecs);
//
//    if (vals[0] >= vals[1] && vals[0] > vals[2])      return FTensor(evecs[0]);
//    else if (vals[1] > vals[0] && vals[1] >= vals[2]) return FTensor(evecs[1]);
//    else if (vals[2] >= vals[0] && vals[2] > vals[1]) return FTensor(evecs[2]);
//    else return FTensor(evecs[0]);
}

FMatrix TensorField::createMatrix(FTensor lhs, FTensor rhs)
{
    FArray a1(lhs);
    FArray a2(rhs);
    FMatrix result(3,3);
    result(0,0) = a1[0] * a2[0];
    result(0,1) = a1[0] * a2[1];
    result(0,2) = a1[0] * a2[2];
    result(1,0) = a1[1] * a2[0];
    result(1,1) = a1[1] * a2[1];
    result(1,2) = a1[1] * a2[2];
    result(2,0) = a1[2] * a2[0];
    result(2,1) = a1[2] * a2[1];
    result(2,2) = a1[2] * a2[2];

    return result;
}
