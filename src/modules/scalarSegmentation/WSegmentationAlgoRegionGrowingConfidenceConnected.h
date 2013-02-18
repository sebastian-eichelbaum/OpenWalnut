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

#ifndef WSEGMENTATIONALGOREGIONGROWINGCONFIDENCECONNECTED_H
#define WSEGMENTATIONALGOREGIONGROWINGCONFIDENCECONNECTED_H

#ifdef OW_USE_ITK

#include <string>
#include <vector>

#include "itkImage.h"
#include "itkConfidenceConnectedImageFilter.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkCastImageFilter.h"

#include "core/kernel/WKernel.h"
#include "core/kernel/WROIManager.h"

#include "core/graphicsEngine/WROIBox.h"

#include "core/dataHandler/WITKImageConversion.h"

#include "WSegmentationAlgo.h"

/**
 * Confidence connected region growing segmentation.
 * \class WSegmentationAlgoRegionGrowingConfidenceConnected
 */
class WSegmentationAlgoRegionGrowingConfidenceConnected : public WSegmentationAlgo
{
public:
    /**
     * Standard constructor.
     */
    WSegmentationAlgoRegionGrowingConfidenceConnected();

    /**
     * Destructor.
     */
    virtual ~WSegmentationAlgoRegionGrowingConfidenceConnected();

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

    //! The number of iteration in region growing.
    WPropInt m_regionGrowingIterations;

    //! The initial neighborhood radius.
    WPropInt m_neighborhoodRadius;

    //! The multiplier for the variance.
    WPropDouble m_multiplier;
};

template< typename T >
WSegmentationAlgo::DataSetPtr WSegmentationAlgoRegionGrowingConfidenceConnected::operator() ( WValueSet< T > const* ) const
{
    typedef itk::Image< T, 3 > ImgType;
    typedef itk::Image< double, 3 > RealType;
    typedef itk::Image< uint64_t, 3 > LabelType; // this might be a problem on 32-bit systems
    typedef itk::Image< float, 3 > FinalType;

    typedef itk::GradientAnisotropicDiffusionImageFilter< ImgType, RealType > SmoothingType;
    typedef itk::CastImageFilter< LabelType, FinalType > CastFilter;
    typedef itk::ConfidenceConnectedImageFilter< RealType, LabelType > RegionGrowingFilter;

    typename ImgType::Pointer image = makeImageFromDataSet< T >( m_dataSet );
    typename SmoothingType::Pointer smoothing = SmoothingType::New();
    typename CastFilter::Pointer cast = CastFilter::New();
    typename RegionGrowingFilter::Pointer regionGrowing = RegionGrowingFilter::New();

    std::vector< osg::ref_ptr< WROI > > rois = WKernel::getRunningKernel()->getRoiManager()->getRois();
    itk::Index< 3 > i;

    WROIBox* box = NULL;

    unsigned int k = 0;
    while( !box && k < rois.size() )
    {
        box = dynamic_cast< WROIBox* >( rois[ k ].get() );
        ++k;
    }

    if( box )
    {
        boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( m_dataSet->getGrid() );
        WVector3d v = 0.5 * ( box->getMinPos() + box->getMaxPos() );
        WValue< int > voxel = grid->getVoxelCoord( v );

        i[ 0 ] = static_cast< int32_t >( voxel[ 0 ] );
        i[ 1 ] = static_cast< int32_t >( voxel[ 1 ] );
        i[ 2 ] = static_cast< int32_t >( voxel[ 2 ] );

        smoothing->SetNumberOfIterations( m_smoothingIter->get( true ) );
        smoothing->SetTimeStep( 0.0625 );
        smoothing->SetConductanceParameter( m_conductance->get( true ) );
        smoothing->SetInput( image );
        regionGrowing->SetInput( smoothing->GetOutput() );
        regionGrowing->SetMultiplier( m_multiplier->get( true ) );
        regionGrowing->SetNumberOfIterations( m_regionGrowingIterations->get( true ) );
        regionGrowing->SetReplaceValue( 255.0f );
        regionGrowing->SetInitialNeighborhoodRadius( m_neighborhoodRadius->get( true ) );
        regionGrowing->SetSeed( i );
        cast->SetInput( regionGrowing->GetOutput() );
        try
        {
            cast->Update();
        }
        catch( ... )
        {
            throw WException( "Problem in Region Growing Segmentation" );
        }
        return makeDataSetFromImage< float >( cast->GetOutput() );
    }
    else
    {
        return DataSetPtr();
    }
}

#endif  // OW_USE_ITK

#endif  // WSEGMENTATIONALGOREGIONGROWINGCONFIDENCECONNECTED_H
