/*
 * PointProperty.cpp
 *
 *  Created on: 12.06.2013
 *      Author: renegade
 */

#include "VertexProperty.h"

VertexProperty::VertexProperty() {
	x=-1, y=-1, z=-1;
}

VertexProperty::~VertexProperty() {}

int VertexProperty::getTriangleCount(){						return attachedTriangle.size(); }
osg::Vec3 VertexProperty::getCoords(){						return osg::Vec3(this->x, this->y, this->z);}
std::vector<long> VertexProperty::getAttachedTriangles(){	return attachedTriangle;}
vector<long> VertexProperty::getNeighbourVertices(){		return neighbourVertices;}
void VertexProperty::setNeighbourVertices(vector<long> vertices){this->neighbourVertices = vertices;}
int VertexProperty::getNeighbourVertexCount(){				return neighbourVertices.size();}
int VertexProperty::getBoundCountClass(){					return boundCountClass;}
void VertexProperty::setBoundCountClass(int boundCount){	boundCountClass = boundCount;}

void VertexProperty::registerTriangle(long idx){
	int size = attachedTriangle.size();
	attachedTriangle.reserve(size+1);
	attachedTriangle.resize(size+1);
	attachedTriangle[size] = idx;
}
bool VertexProperty::containsTriangle(long triangleID){
	for (int i=0; i<getTriangleCount(); i++){
		int id = attachedTriangle[i];
		if (id == triangleID)
			return true;
	}
	return false;
}
void VertexProperty::attachMid(long toID, long midID){
	unsigned int idx = 0;
	while (idx<this->attachedMid.size() && this->attachedMid[idx]->getToID()!=toID) idx++;

	if (idx >= attachedMid.size()){
		unsigned int oldSize = attachedMid.size();
		attachedMid.reserve(oldSize+1);
		attachedMid.resize(oldSize+1);
		unsigned int newSize = attachedMid.size();
		attachedMid[newSize-1] = new MidPoint(toID, midID);
	}
}

bool VertexProperty::toExists(long toID){
	for (unsigned int i=0; i<this->attachedMid.size(); i++)
		if (this->attachedMid[i]->getToID() == toID)
			return true;
	return false;
}

long VertexProperty::getMidID(long toID){
	unsigned int size = this->attachedMid.size();
	for (unsigned int i=0; i<size; i++)
		if (this->attachedMid[i]->getToID() == toID)
			return attachedMid[i]->getMidID();
	return -1;
}


