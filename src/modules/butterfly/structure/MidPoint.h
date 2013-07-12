/*
 * MidPoint.h
 *
 *  Created on: 16.06.2013
 *      Author: renegade
 */

#ifndef MIDPOINT_H_
#define MIDPOINT_H_

namespace std {

class MidPoint {
public:
	MidPoint(long toID, long midID);
	virtual ~MidPoint();
	long getMidID();
	long getToID();
	long midExists();

private:
	long toID, midID;
};

} /* namespace std */
#endif /* MIDPOINT_H_ */
