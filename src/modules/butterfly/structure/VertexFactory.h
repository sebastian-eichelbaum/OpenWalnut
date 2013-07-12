/*
 * VertexFactory.h
 *
 *  Created on: 15.06.2013
 *      Author: renegade
 */

#ifndef VERTEXFACTORY_H_
#define VERTEXFACTORY_H_

#include<vector>
#include"VertexProperty.h"
#include "core/graphicsEngine/WTriangleMesh.h"
#include "osg/Vec3"

namespace std {

class VertexFactory {
public:
	VertexFactory(boost::shared_ptr< WTriangleMesh > triangleMesh);
	VertexFactory();
	virtual ~VertexFactory();
	void resize(long count);
	void registerTriangle(long vertexID, long triangleID);
	vector<long> getAttachedTriangleIDs(long triangleID, long vertex1, long vertex2);
	long get3rdVertexID(long triangleID, long vertex1, long vertex2);
	long getVertexCount();
	int getTriangleCount(long vertexID);
	long getTriangleID(long vertexID, int triangleIndex);
	int getNeighbourIndex(long vertexID, long neighbourID);
	bool neighborExists(long fromID, long toID);
	void attachMid(long fromID, long toID, long midID);
	VertexProperty* getVertexProperty(long vertexID);
	long getMidID(long fromID, long toID);
	void examineCircularityAll();
	int getNeighbourVertexCount(long vertexID);
	int getBoundCountClass(long vertexID);

private:
    std::vector<VertexProperty*> vertProp;
    boost::shared_ptr< WTriangleMesh > triangleMesh;
    long vertexCount;
};

} /* namespace std */
#endif /* VERTEXFACTORY_H_ */
