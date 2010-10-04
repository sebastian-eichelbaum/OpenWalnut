//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
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

#ifndef WSPHERICALHARMONICSCOEFFICIENTSTHREAD_H
#define WSPHERICALHARMONICSCOEFFICIENTSTHREAD_H

#include <map>
#include <string>
#include <vector>


#include "../../common/WThreadedRunner.h"
#include "../../common/math/WMatrix.h"
#include "../../dataHandler/WDataSetRawHARDI.h"
#include "../../dataHandler/WDataSetSphericalHarmonics.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"
#include "../../dataHandler/WGridRegular3D.h"


/**
 * Module for the creation of a spherical harmonic data set from raw HARDI data.
 * \ingroup modules
 */
class WSphericalHarmonicsCoefficientsThread : public WThreadedRunner
{
public:
  /**
   * This structure is a collection of parameter and pointer to input and output data needed by each thread.
   */
  struct WThreadParameter
  {
      /**
        * Pointer to the HARDI measurements
        */
      boost::shared_ptr< WValueSet< int16_t > > m_valueSet;

      /**
        * Indices of nonzero gradients
        */
      std::vector< size_t > m_validIndices;

      /**
        * Indices of zero gradients (this should be normal T2 measurement)
        */
      std::vector< size_t > m_S0Indexes;

      /**
        * Output data, the spherical harmonics coefficients
        */
      boost::shared_ptr< std::vector<double> > m_data;

      /**
        * The order of the calculated spherical harmonics
        */
      int m_order;

      /**
        * Transformation-Matrix for conversion from HARDI measurements to spherical harmonics coefficients
        * (see Descoteaux dissertation)
        */
      boost::shared_ptr< wmath::WMatrix< double > > m_TransformMatrix;

      /**
        * Gradients of all measurements (including )
        */
      std::vector< wmath::WVector3D > m_gradients;

      /**
        * Pointer to progess indicator
        */
      boost::shared_ptr< WProgress > m_progress;

      /**
        * Indicate if the is error calculation is done.
        */
      bool m_doErrorCalculation;

      /**
        * Indicate if the Funk-Radon-Transformation is done.
        */
      bool m_doFunkRadonTransformation;

      /**
        * Indicate if the residuals will be calculated.
        * The residuals are the reprojection error. This means the 
        */
      bool m_doResidualCalculation;

      /**
        * The stored residuals.
        */
      boost::shared_ptr< std::vector<double> > m_dataResiduals;

      /**
        * The b-value used during the creation of the HARDI-data.
        */
      double m_bDiffusionWeightingFactor;

      /**
        * Indicate if the normalisation from 0 to 1 is done.
        */
      bool m_normalize;
  };

  /**
   * The constructor.
   * \param parameter collection of parameter and so on (description in more detail above)
   * \param range the range (start and end) of input data this thread should use
   */
  WSphericalHarmonicsCoefficientsThread( WThreadParameter parameter, std::pair< size_t, size_t > range );

  /**
   * The main function of the thread. Here the calculation of the spherical harmonics coefficients is done.
   */
  void threadMain();

  /**
   * Returns the average error of the thread.
   */
  double getError() const;

private:
  double m_overallError;                 //!< accumulated error
  unsigned int m_errorCount;             //!< number of accumulated errors, necessary for average error calculation
  WThreadParameter m_parameter;          //!< collection of parameter and so on (description in more detail above)
  std::pair< size_t, size_t > m_range;   //!< the range (start and end) of input data this thread use
};
#endif  // WSPHERICALHARMONICSCOEFFICIENTSTHREAD_H
