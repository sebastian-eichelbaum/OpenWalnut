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

namespace std {

class ButterflyFactory {
public:
	ButterflyFactory();
	virtual ~ButterflyFactory();
	void assignSettings(WPropInt m_iterations, WPropDouble m_maxTriangles10n);
	boost::shared_ptr< WTriangleMesh > getInterpolatedMesh(boost::shared_ptr< WTriangleMesh > inputMmesh);
private:
    static float w, factor[4][7], s1, s2;

    void examineInputMesh();
    osg::Vec3 calcMid(long vertID1, long vertID2);
    osg::Vec3 calcMean(long vertID1, long vertID2);
    osg::Vec3 add(Vec3 base, Vec3 sum, float factor);
    Vec3 getKNeighbourValueBoundary(long stencilCenterVertID, long directedNeighbourVertID, bool isSecondK6, bool treatK6AsKn);

    int iterations; long maxTriangles;

    boost::shared_ptr< WTriangleMesh > mesh, triMesh;
    VertexFactory* verts;
    long triCount;
};

} /* namespace std */
#endif /* BUTTERFLYFACTORY_H_ */
