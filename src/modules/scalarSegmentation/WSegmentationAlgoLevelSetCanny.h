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

#ifndef WSEGMENTATIONALGOLEVELSETCANNY_H
#define WSEGMENTATIONALGOLEVELSETCANNY_H

#ifdef OW_USE_ITK

#include <string>

#include "itkImage.h"
#include "itkCannySegmentationLevelSetImageFilter.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkCastImageFilter.h"

#include "core/dataHandler/WITKImageConversion.h"

#include "WSegmentationAlgo.h"

/**
 * Canny edge levelset segmentation.
 * \class WSegmentationAlgoLevelSetCanny
 * \note Work in progress.
 */
class WSegmentationAlgoLevelSetCanny : public WSegmentationAlgo
{
public:
    /**
     * Standard constructor.
     */
    WSegmentationAlgoLevelSetCanny();

    /**
     * Destructor.
     */
    virtual ~WSegmentationAlgoLevelSetCanny();

    /**
     * Return the name of this algorithm.
     * \return The name.
     */
    virtual std::string getName();

    /**
     * Return a description of this algorithm.
     * \return A description.
     */
    virtual std::string getDescription();

    /**
     * Checks if any properties were changed.
     * \return True, iff any properties were changed.
     */
    virtual bool propChanged();

    /**
     * Implements the operation.
     *
     * \tparam The type of the values in the dataset's valueset.
     * \param valueset The dataset's valueset.
     * \return The resulting dataset.
     */
    template< typename T >
    DataSetPtr operator() ( WValueSet< T > const* valueset ) const;

private:
    /**
     * Initializes the algorithm's properties.
     */
    virtual void properties();

    /**
     * A virtual function that calls the correct segmentation operation.
     * \return The resulting dataset.
     */
    virtual DataSetPtr applyOperation();

    //! The number of iteration in the smoothing filter.
    WPropInt m_smoothingIter;

    //! The conductance parameter for the anisotropic smoothing.
    WPropDouble m_conductance;

    //! The levelset value.
    WPropDouble m_level;

    //! The variance.
    WPropDouble m_variance;

    //! A threshold.
    WPropDouble m_threshold;
};

template< typename T >
WSegmentationAlgo::DataSetPtr WSegmentationAlgoLevelSetCanny::operator() ( WValueSet< T > const* ) const
{
    typedef itk::Image< T, 3 > ImgType;
    typedef itk::Image< float, 3 > RealType;

    typedef itk::GradientAnisotropicDiffusionImageFilter< ImgType, RealType > SmoothingType;
    typedef itk::CannySegmentationLevelSetImageFilter< RealType, RealType > CannyLSFilter;
    typedef itk::CastImageFilter< ImgType, RealType > CastFilter;

    typename ImgType::Pointer image = makeImageFromDataSet< T >( m_dataSet );
    typename SmoothingType::Pointer smoothing = SmoothingType::New();
    typename CannyLSFilter::Pointer levelset = CannyLSFilter::New();
    typename CastFilter::Pointer cast = CastFilter::New();

    smoothing->SetNumberOfIterations( m_smoothingIter->get( true ) );
    smoothing->SetTimeStep( 0.0625 );
    smoothing->SetConductanceParameter( m_conductance->get( true ) );
    smoothing->SetInput( image );

    cast->SetInput( image );

    levelset->SetAdvectionScaling( 1.0 );
    levelset->SetCurvatureScaling( 1.0 );
    levelset->SetPropagationScaling( 0.0 );
    levelset->SetMaximumRMSError( 0.01 );
    levelset->SetNumberOfIterations( 10 );
    levelset->SetThreshold( m_threshold->get( true ) / 100.0 );
    levelset->SetVariance( m_variance->get( true ) );
    levelset->SetIsoSurfaceValue( m_level->get( true ) );
    levelset->SetInput( cast->GetOutput() );
    levelset->SetFeatureImage( smoothing->GetOutput() );

    try
    {
        levelset->Update();
    }
    catch( ... )
    {
        throw WException( "Problem in Level Set Segmentation 1" );
    }
    return makeDataSetFromImage< float >( levelset->GetOutput() );
}

#endif  // OW_USE_ITK

#endif  // WSEGMENTATIONALGOLEVELSETCANNY_H
