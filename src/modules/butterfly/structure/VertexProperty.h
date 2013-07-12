/*
 * PointProperty.h
 *
 *  Created on: 12.06.2013
 *      Author: renegade
 */

#ifndef POINTPROPERTY_H_
#define POINTPROPERTY_H_

#include<vector>
#include "MidPoint.h"
#include "osg/Vec3"



using namespace std;

class VertexProperty {
public:
	VertexProperty();
	virtual ~VertexProperty();
	int getTriangleCount();
	void registerTriangle(long idx);
	std::vector<long> getAttachedTriangles();
	vector<long> getNeighbourVertices();
	void setNeighbourVertices(vector<long> vertices);
	int getNeighbourVertexCount();
	int getBoundCountClass();
	void setBoundCountClass(int boundCountClass);
	bool containsTriangle(long triangleID);
	void attachMid(long toID, long midID);
	long getMidID(long toID);
	bool toExists(long toID);
	osg::Vec3 getCoords();


private:
	std::vector<long> attachedTriangle;
	std::vector<MidPoint*> attachedMid;
	float x, y, z;
	vector<long> neighbourVertices;
	int boundCountClass;
};

#endif /* POINTPROPERTY_H_ */
