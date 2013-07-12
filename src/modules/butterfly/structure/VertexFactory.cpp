/*
 * VertexFactory.cpp
 *
 *  Created on: 15.06.2013
 *      Author: renegade
 */

#include "VertexFactory.h"
#include <vector>

namespace std {

VertexFactory::VertexFactory() {
	vertexCount = 0;
}
VertexFactory::VertexFactory(boost::shared_ptr< WTriangleMesh > triangleMesh) {
	this->triangleMesh = triangleMesh;
	vertexCount = 0;
}

VertexFactory::~VertexFactory() {
	// TODO Auto-generated destructor stub
}

void VertexFactory::resize(long finalSize){
	int size = this->vertProp.size();
	this->vertProp.reserve(finalSize);
	this->vertProp.resize(finalSize);
	for (int i=size; i<finalSize; i++)
		this->vertProp[i] = new VertexProperty();
}

void VertexFactory::registerTriangle(long vertexID, long triangleID){
	if((unsigned long)vertexID >= vertProp.size()) resize(vertexID+1);
	this->vertProp[vertexID]->registerTriangle(triangleID);
}
long VertexFactory::getVertexCount(){
	return this->vertProp.size();
}
int VertexFactory::getTriangleCount(long vertexID){
	return this->vertProp[vertexID]->getTriangleCount();
}
long VertexFactory::getTriangleID(long vertID, int triangleIndex){
	return (this->vertProp[vertID]->getAttachedTriangles())[triangleIndex];
}
vector<long> VertexFactory::getAttachedTriangleIDs(long triangleID, long vert1, long vert2){
	vector<long> tris;
	int sizeN = getTriangleCount(vert1);
	for (int i=0; i<sizeN; i++){
		long newTriID = getTriangleID(vert1, i);
		if (newTriID!=triangleID && this->vertProp[vert2]->containsTriangle(newTriID)){
			int size = tris.size();
			tris.reserve(size + 1);
			tris.resize(size + 1);
			tris[size] = newTriID;
		}
	}
	return tris;
}
long VertexFactory::get3rdVertexID(long triangleID, long vert1, long vert2){
	long id0 = triangleMesh->getTriVertId0(triangleID),
			id1 = triangleMesh->getTriVertId1(triangleID),
			id2 = triangleMesh->getTriVertId2(triangleID);
	if (vert1!=id0 && vert2!=id0) return id0;
	if (vert1!=id1 && vert2!=id1) return id1;
	return id2;
}
//TODO: Method isTriangleAttached
int VertexFactory::getNeighbourIndex(long vertexID, long neighbourID){
	VertexProperty* prop = this->vertProp[vertexID];
	for (int i=0; i<prop->getNeighbourVertexCount(); i++)
		if (prop->getNeighbourVertices()[i]==neighbourID)
			return i;
	return -1;
}
bool VertexFactory::neighborExists(long fromID, long toID){
	if (fromID == toID) return true;
	if (fromID > toID){
		long tmp = fromID; fromID = toID; toID = tmp;
	}
	return this->vertProp[fromID]->toExists(toID);
}
void VertexFactory::attachMid(long fromID, long toID, long midID){
	if (fromID == toID) return;
	if (fromID > toID){
		long tmp = fromID; fromID = toID; toID = tmp;}
	this->vertProp[fromID]->attachMid(toID, midID);
}
VertexProperty* VertexFactory::getVertexProperty(long vertexID){	return this->vertProp[vertexID];}
long VertexFactory::getMidID(long fromID, long toID){
	if (fromID == toID) return fromID;
	if (fromID > toID){
		long tmp = fromID; fromID = toID; toID = tmp;}
	return vertProp[fromID]->getMidID(toID);
}
void VertexFactory::examineCircularityAll(){
	for (unsigned long vert=0; vert<this->vertProp.size(); vert++){
	unsigned long firstTri = this->vertProp[vert]->getAttachedTriangles()[0],
				id0 = this->triangleMesh->getTriVertId0(firstTri),
				vertBound = id0!=vert ?id0 :this->triangleMesh->getTriVertId1(firstTri),
				firstVert = vertBound;
		vector<long> tris = getAttachedTriangleIDs(-1, vert, firstVert),
				neighbours;
		bool moreThanTwo = tris.size()>2;
		long firstDirection = tris[0];
		long ranTriangles = 0;
		do {
			ranTriangles++;
			int size = neighbours.size();
			neighbours.reserve(size+1); neighbours.resize(size+1);
			neighbours[size] = vertBound;
			vertBound = get3rdVertexID(tris[0], vert, vertBound);
			tris = getAttachedTriangleIDs(tris[0], vert, vertBound);
		} while(vertBound!=firstVert && tris.size()==1);
		if (vertBound!=firstVert && tris.size()<2){
			unsigned int size = neighbours.size()+1;
			neighbours.reserve(size); neighbours.resize(size);
			neighbours[size-1] = vertBound;
			for (unsigned int i=0; i<size/2; i++){
				long tmp = neighbours[neighbours.size()-i-1];
				neighbours[neighbours.size()-i-1] = neighbours[i];
				neighbours[i] = tmp;
			}
			vertBound = firstVert;
			tris = getAttachedTriangleIDs(firstDirection, vert, vertBound);
			while(tris.size()==1) {
				ranTriangles++;
				vertBound = get3rdVertexID(tris[0], vert, vertBound);
				int size = neighbours.size();
				neighbours.reserve(size+1); neighbours.resize(size+1);
				neighbours[size] = vertBound;
				tris = getAttachedTriangleIDs(tris[0], vert, vertBound);
			};
			vertBound = -1;
		}
		vertProp[vert]->setNeighbourVertices(neighbours);
		if (tris.size()>1) moreThanTwo = true;
		if (vertBound==firstVert && !moreThanTwo){
			this->vertProp[vert]->setBoundCountClass(0);
		}else{
			this->vertProp[vert]->setBoundCountClass((moreThanTwo || ranTriangles!=vertProp[vert]->getTriangleCount()) ?2 :1);
			if (!moreThanTwo && ranTriangles>vertProp[vert]->getTriangleCount())vertProp[vert]->setBoundCountClass(-1);
		}
	}
}
int VertexFactory::getNeighbourVertexCount(long vertexID){	return vertProp[vertexID]->getNeighbourVertexCount();}
int VertexFactory::getBoundCountClass(long vertexID){		return vertProp[vertexID]->getBoundCountClass();}

} /* namespace std */
