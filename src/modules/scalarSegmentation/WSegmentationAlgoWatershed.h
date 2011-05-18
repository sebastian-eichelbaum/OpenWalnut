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

#ifndef WSEGMENTATIONALGOWATERSHED_H
#define WSEGMENTATIONALGOWATERSHED_H

#ifdef OW_USE_ITK

#include <string>

#include "itkImage.h"
#include "itkWatershedImageFilter.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkGradientMagnitudeImageFilter.h"

#include "core/dataHandler/WITKImageConversion.h"

#include "WSegmentationAlgo.h"

/**
 * Watershed segmentation. Fills a gradient magnitude image with water up to
 * a certain level. The 'basins' are the segmented regions.
 *
 * \class WSegmentationAlgoWatershed
 */
class WSegmentationAlgoWatershed : public WSegmentationAlgo
{
public:
    /**
     * Standard constructor.
     */
    WSegmentationAlgoWatershed();

    /**
     * Destructor.
     */
    virtual ~WSegmentationAlgoWatershed();

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

    //! The threshold in %.
    WPropDouble m_threshold;

    //! The water level in %.
    WPropDouble m_level;

    //! The number of iteration in the smoothing filter.
    WPropInt m_iter;

    //! The conductance parameter for the anisotropic smoothing.
    WPropDouble m_conductance;
};

template< typename T >
WSegmentationAlgo::DataSetPtr WSegmentationAlgoWatershed::operator() ( WValueSet< T > const* ) const
{
    typedef itk::Image< T, 3 > ImgType;
    typedef itk::Image< double, 3 > RealType;
    typedef itk::Image< uint64_t, 3 > LabelType; // this might be a problem on 32-bit systems
    typedef itk::Image< float, 3 > FinalType;

    typedef itk::GradientAnisotropicDiffusionImageFilter< ImgType, RealType > SmoothingType;
    typedef itk::CastImageFilter< LabelType, FinalType > CastFilter;
    typedef itk::GradientMagnitudeImageFilter< RealType, RealType > GradFilter;
    typedef itk::WatershedImageFilter< RealType > WaterFilter;

    typename ImgType::Pointer image = makeImageFromDataSet< T >( m_dataSet );
    typename SmoothingType::Pointer smoothing = SmoothingType::New();
    typename CastFilter::Pointer cast = CastFilter::New();
    typename GradFilter::Pointer gradientMagnitude = GradFilter::New();
    typename WaterFilter::Pointer watershed = WaterFilter::New();

    smoothing->SetNumberOfIterations( m_iter->get( true ) );
    smoothing->SetTimeStep( 0.0625 );
    smoothing->SetConductanceParameter( m_conductance->get( true ) );
    smoothing->SetInput( image );
    gradientMagnitude->SetInput( smoothing->GetOutput() );
    watershed->SetInput( gradientMagnitude->GetOutput() );
    watershed->SetLevel( m_level->get( true ) / 100.0 );
    watershed->SetThreshold( m_threshold->get( true ) / 100.0 );
    cast->SetInput( watershed->GetOutput() );
    cast->Update();
    return makeDataSetFromImage< float >( cast->GetOutput() );
}

#endif  // OW_USE_ITK

#endif  // WSEGMENTATIONALGOWATERSHED_H
