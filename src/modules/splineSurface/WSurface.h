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

#ifndef WSURFACE_H
#define WSURFACE_H

#include <vector>

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

#include "WBSplineSurface.h"

#include "../../graphicsEngine/WTriangleMesh2.h"
#include "../../common/math/WTensorSym.h"

class WSurface
{
public:
    WSurface();
    ~WSurface();

    boost::shared_ptr< WTriangleMesh2 > execute();

    std::vector< std::vector< double > > getSplinePoints();
    void setSetSampleRate( float r );

private:

    wmath::WTensorSym< 2, 3, double > getCovarianceMatrix( std::vector< std::vector< double > > points );
    void getSplineSurfaceDeBoorPoints( std::vector< std::vector< double > > &givenPoints, std::vector< std::vector< double > > &deBoorPoints, int numRows, int numCols ); // NOLINT

    boost::shared_ptr< WTriangleMesh2 > m_tMesh;

    double m_radius;
    double m_my;
    int m_numDeBoorRows;
    int m_numDeBoorCols;
    int m_order;
    double m_sampleRateT;
    double m_sampleRateU;
    double m_xAverage;
    double m_yAverage;
    double m_zAverage;

    std::vector< std::vector< double > > m_splinePoints;

    int m_renderpointsPerCol;
    int m_renderpointsPerRow;
    int m_numPoints;
};

#endif  // WSURFACE_H
