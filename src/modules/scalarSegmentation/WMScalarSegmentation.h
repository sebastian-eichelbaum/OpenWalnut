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

#ifndef WMSCALARSEGMENTATION_H
#define WMSCALARSEGMENTATION_H

#include <string>
#include <vector>

#include "../../common/WItemSelection.h"
#include "../../common/WItemSelector.h"

#include "../../dataHandler/WITKImageConversion.h"
#include "../../dataHandler/WDataHandler.h"

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.h"
#include "../../kernel/WModuleOutputData.h"
#include "../../dataHandler/WDataSetScalar.h"

#ifdef OW_USE_ITK

#include "itkWatershedImageFilter.h"
#include "itkOtsuThresholdImageFilter.h"
#include "itkConfidenceConnectedImageFilter.h"
#include "itkCannySegmentationLevelSetImageFilter.h"

// smoothing filters
#include "itkGradientAnisotropicDiffusionImageFilter.h"
// other
#include "itkCastImageFilter.h"
#include "itkGradientMagnitudeImageFilter.h"

#endif  // OW_USE_ITK

/**
 * First version of a module that implements 3D-image segmentation algorithms.
 *
 * \ingroup modules
 */
class WMScalarSegmentation: public WModule
{
public:

    /**
     * Default constructor.
     */
    WMScalarSegmentation();

    /**
     * Destructor.
     */
    virtual ~WMScalarSegmentation();

    /**
     * Return the name of this module.
     *
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Return a description of this module.
     *
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Return a new instance of this module.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     *
     * \return the icon.
     */
    virtual const char** getXPMIcon() const;

protected:

    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

    /**
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

private:

    /**
     * Do a segmentation depending on the current module property values.
     *
     * \return the resulting segmented dataset.
     */
    void doSegmentation();

    /**
     * An input connector used to get datasets from other modules.
     */
    boost::shared_ptr< WModuleInputData< WDataSetScalar > > m_input;

    /**
     * The output connector used to provide the calculated data to other modules.
     */
    boost::shared_ptr< WModuleOutputData< WDataSetScalar > > m_output;

    /**
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetScalar > m_dataSet;

    /**
     * This is a pointer to the segmented dataset.
     */
    boost::shared_ptr< WDataSetScalar > m_result;

    /**
     * A condition used to notify about changes in several properties.
     */
    boost::shared_ptr< WCondition > m_propCondition;

    /**
     * The number of the currently selected segmentation method.
     */
    std::size_t m_algoIndex;

    /**
     * A list of possible segmentation algorithms.
     */
    boost::shared_ptr< WItemSelection > m_algos;

    /**
     * A selection box for segmentation algorithms.
     */
    WPropSelection m_algoType;

    /**
     * The threshold property for the simple threshold segmentation.
     */
    WPropDouble m_threshold;

    //! The watershed level.
    WPropDouble m_level;

    //! A given variance.
    WPropDouble m_variance;

    //! A function object that implements a simple threshold segmentation.
    class SimpleSegmentation : public boost::static_visitor< boost::shared_ptr< WValueSetBase > >
    {
    public:
        /**
         * Constructor.
         * \param t The threshold for segmentation.
         */
        SimpleSegmentation( double t ) // NOLINT
            : m_threshold( t )
        {
        }

        /**
         * Do the segmentation.
         *
         * \tparam T The integral data type.
         * \param valueset The valueset to segment.
         * \return A pointer to a new valueset.
         */
        template< typename T >
        boost::shared_ptr< WValueSetBase > operator() ( WValueSet< T > const* valueset ) const;

    private:
        //! the threshold
        double m_threshold;
    };

#ifdef OW_USE_ITK
    //! A class that implements a watershed segmentation algorithm.
    class WatershedSegmentation : public boost::static_visitor< boost::shared_ptr< WValueSetBase > >
    {
    public:
        /**
         * Constructor.
         * \param level The water level.
         * \param t The threshold for segmentation.
         * \param ds The dataset.
         */
        WatershedSegmentation( double level, double t, boost::shared_ptr< WDataSetScalar > ds )
            : m_level( level ),
              m_threshold( t ),
              m_dataset( ds )
        {
        }

        /**
         * Do the segmentation.
         *
         * \tparam T The integral data type.
         * \return A pointer to a new valueset.
         */
        template< typename T >
        boost::shared_ptr< WValueSetBase > operator() ( WValueSet< T > const* ) const;

    private:
        //! the level
        double m_level;
        //! the threshold
        double m_threshold;
        //! the dataset
        boost::shared_ptr< WDataSetScalar > m_dataset;
    };

    //! A segmentation using otsu's thresholding method.
    class OtsuSegmentation : public boost::static_visitor< boost::shared_ptr< WValueSetBase > >
    {
    public:
        /**
         * Constructor.
         * \param ds The dataset.
         */
        OtsuSegmentation( boost::shared_ptr< WDataSetScalar > ds ) // NOLINT
            : m_dataset( ds )
        {
        }

        /**
         * Do the segmentation.
         *
         * \tparam T The integral data type.
         * \param valueset The valueset to segment.
         * \return A pointer to a new valueset.
         */
        template< typename T >
        boost::shared_ptr< WValueSetBase > operator() ( WValueSet< T > const* v ) const;

    private:
        //! the dataset
        boost::shared_ptr< WDataSetScalar > m_dataset;
    };

    //! Segmentation via confidence connected region growing.
    class RegionGrowingSegmentation : public boost::static_visitor< boost::shared_ptr< WValueSetBase > >
    {
    public:
        /**
         * Constructor.
         * \param ds The dataset.
         */
        RegionGrowingSegmentation( boost::shared_ptr< WDataSetScalar > ds ) // NOLINT
            : m_dataset( ds )
        {
        }

        /**
         * Do the segmentation.
         *
         * \tparam T The integral data type.
         * \param valueset The valueset to segment.
         * \return A pointer to a new valueset.
         */
        template< typename T >
        boost::shared_ptr< WValueSetBase > operator() ( WValueSet< T > const* ) const;

    private:
        //! the dataset
        boost::shared_ptr< WDataSetScalar > m_dataset;
    };

    //! Segmentation via level sets.
    class LevelSetSegmentation1 : public boost::static_visitor< boost::shared_ptr< WValueSetBase > >
    {
    public:
        /**
         * Constructor.
         * \param ds The dataset.
         */
        LevelSetSegmentation1( double level, double t, double var, boost::shared_ptr< WDataSetScalar > ds )
            : m_level( level ),
              m_threshold( t ),
              m_variance( var ),
              m_dataset( ds )
        {
        }

        /**
         * Do the segmentation.
         *
         * \tparam T The integral data type.
         * \param valueset The valueset to segment.
         * \return A pointer to a new valueset.
         */
        template< typename T >
        boost::shared_ptr< WValueSetBase > operator() ( WValueSet< T > const* ) const;

    private:
        //! The level.
        double m_level;

        //! threshold
        double m_threshold;

        //! the variance
        double m_variance;

        //! the dataset
        boost::shared_ptr< WDataSetScalar > m_dataset;
    };
#endif  // OW_USE_ITK
};

template< typename T >
boost::shared_ptr< WValueSetBase > WMScalarSegmentation::SimpleSegmentation::operator() ( WValueSet< T > const* valueset ) const
{
    std::vector< T > values( valueset->size() );
    for( std::size_t k = 0; k < valueset->size(); ++k )
    {
        values[ k ] = ( static_cast< double >( valueset->getScalar( k ) ) <
                        m_threshold * ( valueset->getMaximumValue() - valueset->getMinimumValue() ) / 100.0 + valueset->getMinimumValue()
                        ? static_cast< T >( 0 ) : static_cast< T >( 100 ) );
    }
    return boost::shared_ptr< WValueSet< T > >( new WValueSet< T >( 0, 1, values, DataType< T >::type ) );
}

#ifdef OW_USE_ITK

template< typename T >
boost::shared_ptr< WValueSetBase > WMScalarSegmentation::WatershedSegmentation::operator() ( WValueSet< T > const* ) const
{
    typedef itk::Image< T, 3 > ImgType;
    typedef itk::Image< double, 3 > RealType;
    typedef itk::Image< uint64_t, 3 > LabelType; // this might be a problem on 32-bit systems
    typedef itk::Image< float, 3 > FinalType;

    typedef itk::GradientAnisotropicDiffusionImageFilter< ImgType, RealType > SmoothingType;
    typedef itk::CastImageFilter< LabelType, FinalType > CastFilter;
    typedef itk::GradientMagnitudeImageFilter< RealType, RealType > GradFilter;
    typedef itk::WatershedImageFilter< RealType > WaterFilter;

    typename ImgType::Pointer img = makeImageFromDataSet< T >( m_dataset );
    typename SmoothingType::Pointer sm = SmoothingType::New();
    typename CastFilter::Pointer cf = CastFilter::New();
    typename GradFilter::Pointer gf = GradFilter::New();
    typename WaterFilter::Pointer ws = WaterFilter::New();

    sm->SetNumberOfIterations( 10 );
    sm->SetTimeStep( 0.0625 );
    sm->SetConductanceParameter( 2.0 );
    sm->SetInput( img );
    gf->SetInput( sm->GetOutput() );
    ws->SetInput( gf->GetOutput() );
    ws->SetLevel( m_level / 100.0 );
    ws->SetThreshold( m_threshold / 100.0 );
    cf->SetInput( ws->GetOutput() );
    cf->Update();
    boost::shared_ptr< WDataSetScalar > res = makeDataSetFromImage< float >( cf->GetOutput() );
    return res->getValueSet();
}

template< typename T >
boost::shared_ptr< WValueSetBase > WMScalarSegmentation::OtsuSegmentation::operator() ( WValueSet< T > const* v ) const
{
    typedef itk::Image< T, 3 > ImgType;
    typedef itk::OtsuThresholdImageFilter< ImgType, ImgType > Otsu;

    typename ImgType::Pointer img = makeImageFromDataSet< T >( m_dataset );
    typename Otsu::Pointer o = Otsu::New();
    o->SetInput( img );
    o->SetOutsideValue( v->getMaximumValue() );
    o->SetInsideValue( v->getMinimumValue() );
    o->Update();
    boost::shared_ptr< WDataSetScalar > res = makeDataSetFromImage< T >( o->GetOutput() );
    return res->getValueSet();
}

template< typename T >
boost::shared_ptr< WValueSetBase > WMScalarSegmentation::RegionGrowingSegmentation::operator() ( WValueSet< T > const* ) const
{
    typedef itk::Image< T, 3 > ImgType;
    typedef itk::Image< double, 3 > RealType;
    typedef itk::Image< uint64_t, 3 > LabelType; // this might be a problem on 32-bit systems
    typedef itk::Image< float, 3 > FinalType;

    typedef itk::GradientAnisotropicDiffusionImageFilter< ImgType, RealType > SmoothingType;
    typedef itk::CastImageFilter< LabelType, FinalType > CastFilter;
    typedef itk::ConfidenceConnectedImageFilter< RealType, LabelType > RegionGrowingFilter;

    typename ImgType::Pointer img = makeImageFromDataSet< T >( m_dataset );
    typename SmoothingType::Pointer sm = SmoothingType::New();
    typename CastFilter::Pointer cf = CastFilter::New();
    typename RegionGrowingFilter::Pointer rg = RegionGrowingFilter::New();

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
        i[ 0 ] = static_cast< int32_t >( box->getMinPos()[ 0 ] + box->getMaxPos()[ 0 ] ) / 2;
        i[ 1 ] = static_cast< int32_t >( box->getMinPos()[ 1 ] + box->getMaxPos()[ 1 ] ) / 2;
        i[ 2 ] = static_cast< int32_t >( box->getMinPos()[ 2 ] + box->getMaxPos()[ 2 ] ) / 2;

        sm->SetNumberOfIterations( 5 );
        sm->SetTimeStep( 0.0625 );
        sm->SetConductanceParameter( 2.0 );
        sm->SetInput( img );
        rg->SetInput( sm->GetOutput() );
        rg->SetMultiplier( 2.5 );
        rg->SetNumberOfIterations( 5 );
        rg->SetReplaceValue( 255.0f );
        rg->SetInitialNeighborhoodRadius( 2 );
        rg->SetSeed( i );
        cf->SetInput( rg->GetOutput() );
        try
        {
            cf->Update();
        }
        catch( ... )
        {
            throw WException( "Problem in Region Growing Segmentation" );
        }
        boost::shared_ptr< WDataSetScalar > res = makeDataSetFromImage< float >( cf->GetOutput() );
        return res->getValueSet();
    }
    else
    {
        return m_dataset->getValueSet();
    }
}

template< typename T >
boost::shared_ptr< WValueSetBase > WMScalarSegmentation::LevelSetSegmentation1::operator() ( WValueSet< T > const* ) const
{
    typedef itk::Image< T, 3 > ImgType;
    typedef itk::Image< float, 3 > RealType;

    typedef itk::GradientAnisotropicDiffusionImageFilter< ImgType, RealType > SmoothingType;
    typedef itk::CannySegmentationLevelSetImageFilter< RealType, RealType > CannyLSFilter;
    typedef itk::CastImageFilter< ImgType, RealType > CastFilter;

    typename ImgType::Pointer img = makeImageFromDataSet< T >( m_dataset );
    typename SmoothingType::Pointer sm = SmoothingType::New();
    typename CannyLSFilter::Pointer ls = CannyLSFilter::New();
    typename CastFilter::Pointer cf = CastFilter::New();

    sm->SetNumberOfIterations( 5 );
    sm->SetTimeStep( 0.0625 );
    sm->SetConductanceParameter( 2.0 );
    sm->SetInput( img );

    cf->SetInput( img );

    ls->SetAdvectionScaling( 1.0 );
    ls->SetCurvatureScaling( 1.0 );
    ls->SetPropagationScaling( 0.0 );
    ls->SetMaximumRMSError( 0.01 );
    ls->SetNumberOfIterations( 10 );
    ls->SetThreshold( m_threshold );
    ls->SetVariance( m_variance );
    ls->SetIsoSurfaceValue( m_level );
    ls->SetInput( cf->GetOutput() );
    ls->SetFeatureImage( sm->GetOutput() );

    try
    {
        ls->Update();
    }
    catch( ... )
    {
        throw WException( "Problem in Level Set Segmentation 1" );
    }
    boost::shared_ptr< WDataSetScalar > res = makeDataSetFromImage< float >( ls->GetOutput() );
    return res->getValueSet();
}

#endif  // OW_USE_ITK

#endif  // WMSCALARSEGMENTATION_H
