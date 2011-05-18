// vim:nolist
/*
 * TensorField.h
 *
 *  Created on: 16.11.2008
 *      Author: ralph
 */

#ifndef TENSORFIELD_H_
#define TENSORFIELD_H_

#include <boost/shared_ptr.hpp>

#include "../fantom/FTensor.h"
#include "core/dataHandler/WDataSetVector.h"

class TensorField
{
public:
    explicit TensorField( boost::shared_ptr< WDataSetVector > vectors );
	virtual ~TensorField();

	int getCells() 	{ return m_cells; };
	int getOrder() 	{ return m_order; };
	int getPosDim()	{ return m_posDim; };

	FTensor getTensorAtIndex(int index) {return theField[index];};
	FTensor getInterpolatedVector(const float x, const float y, const float z);

private:
	std::vector<FTensor> theField;

	FMatrix createMatrix(FTensor lhs, FTensor rhs);

	int m_cells;
	int m_order;
	int m_posDim;
    boost::shared_ptr< WDataSetVector > m_vectors;
};

#endif /* TENSORFIELD_H_ */
