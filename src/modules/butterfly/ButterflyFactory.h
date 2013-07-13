//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2013 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

/*
 * ButterflyFactory.h
 *
 *  Created on: 30.06.2013
 *      Author: renegade
 */

#ifndef BUTTERFLYFACTORY_H_
#define BUTTERFLYFACTORY_H_
#include "core/kernel/WModule.h"
#include "structure/VertexFactory.h"

using osg::Vec3;

namespace butterfly
{
    class ButterflyFactory
    {
    public:
        ButterflyFactory();
        virtual ~ButterflyFactory();
        void assignSettings( WPropInt m_iterations, WPropDouble m_maxTriangles10n );
        boost::shared_ptr< WTriangleMesh > getInterpolatedMesh( boost::shared_ptr< WTriangleMesh > inputMmesh );
    private:
        static float w, factor[4][7], s1, s2;

        void examineInputMesh();
        osg::Vec3 calcMid( size_t vertID1, size_t vertID2 );
        osg::Vec3 calcMean( size_t vertID1, size_t vertID2 );
        osg::Vec3 add( Vec3 base, Vec3 sum, float factor );
        Vec3 getKNeighbourValueBoundary( size_t stencilCenterVertID, size_t directedNeighbourVertID, bool isSecondK6, bool treatK6AsKn );

        size_t iterations;
        size_t maxTriangles;

        boost::shared_ptr< WTriangleMesh > mesh, triMesh;
        VertexFactory* verts;
        size_t triCount;
    };

} /* namespace butterfly */
#endif  // BUTTERFLYFACTORY_H
