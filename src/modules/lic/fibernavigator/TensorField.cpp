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
#include "../../../common/math/linearAlgebra/WLinearAlgebra.h"
#include "../../../dataHandler/WDataSetVector.h"
#include "../../../dataHandler/WGridRegular3D.h"

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
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_vectors->getGrid() );

    WAssert( grid,  "This data set has a grid whose type is not yet supported for interpolation." );
    WAssert( ( m_vectors->getValueSet()->order() == 1 &&  m_vectors->getValueSet()->dimension() == 3 ),
             "Only implemented for 3D Vectors so far." );
    WPosition pos( x, y, z );
    WAssert( grid->encloses( pos ), "Grid does not enclose position to interpolate" );
    bool isInside = true; // TODO(all): check for isInside here. How will we handle it?
    std::vector< size_t > vertexIds = grid->getCellVertexIds( grid->getCellId( pos, &isInside ) );

    float xMult = x - (int)x;
    float yMult = y - (int)y;
    float zMult = z - (int)z;

    FTensor txyz    = FTensor( m_vectors->getVectorAt( vertexIds[0] ) );
    FTensor tx1yz   = FTensor( m_vectors->getVectorAt( vertexIds[1] ) );
    FTensor txy1z   = FTensor( m_vectors->getVectorAt( vertexIds[2] ) );
    FTensor tx1y1z  = FTensor( m_vectors->getVectorAt( vertexIds[3] ) );
    FTensor txyz1   = FTensor( m_vectors->getVectorAt( vertexIds[4] ) );
    FTensor tx1yz1  = FTensor( m_vectors->getVectorAt( vertexIds[5] ) );
    FTensor txy1z1  = FTensor( m_vectors->getVectorAt( vertexIds[6] ) );
    FTensor tx1y1z1 = FTensor( m_vectors->getVectorAt( vertexIds[7] ) );

    FMatrix matxyz = createMatrix(txyz, txyz);
    FMatrix matx1yz = createMatrix(tx1yz, tx1yz);
    FMatrix matxy1z = createMatrix(txy1z, txy1z);
    FMatrix matx1y1z = createMatrix(tx1y1z, tx1y1z);
    FMatrix matxyz1 = createMatrix(txyz1, txyz1);
    FMatrix matx1yz1 = createMatrix(tx1yz1, tx1yz1);
    FMatrix matxy1z1 = createMatrix(txy1z1, txy1z1);
    FMatrix matx1y1z1 = createMatrix(tx1y1z1, tx1y1z1);

    FMatrix i1 = matxyz * (1. - zMult) + matxyz1 * zMult;
    FMatrix i2 = matxy1z * (1. - zMult) + matxy1z1 * zMult;
    FMatrix j1 = matx1yz * (1. - zMult) + matx1yz1 * zMult;
    FMatrix j2 = matx1y1z * (1. - zMult) + matx1y1z1 * zMult;

    FMatrix w1 = i1 * (1. - yMult) + i2 * yMult;
    FMatrix w2 = j1 * (1. - yMult) + j2 * yMult;

    FMatrix matResult = w1 * (1. - xMult) + w2 * xMult;
    std::vector<FArray>evecs;
    FArray vals(0,0,0);

    matResult.getEigenSystem(vals, evecs);

    if (vals[0] >= vals[1] && vals[0] > vals[2])      return FTensor(evecs[0]);
    else if (vals[1] > vals[0] && vals[1] >= vals[2]) return FTensor(evecs[1]);
    else if (vals[2] >= vals[0] && vals[2] > vals[1]) return FTensor(evecs[2]);
    else return FTensor(evecs[0]);
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
