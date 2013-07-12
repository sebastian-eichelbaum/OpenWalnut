/*
 * MidPoint.cpp
 *
 *  Created on: 16.06.2013
 *      Author: renegade
 */

#include "MidPoint.h"

namespace std {

MidPoint::MidPoint(long toID, long midID) {
	this->toID = toID; this->midID = midID;
}

MidPoint::~MidPoint() {
	// TODO Auto-generated destructor stub
}

long MidPoint::getMidID(){			return this->midID;}
long MidPoint::getToID(){			return this->toID;}

} /* namespace std */
