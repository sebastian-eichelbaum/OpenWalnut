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

#ifndef WSEGMENTATIONALGOOTSU_H
#define WSEGMENTATIONALGOOTSU_H

#ifdef OW_USE_ITK

#include <string>

#include "itkImage.h"
#include "itkOtsuThresholdImageFilter.h"

#include "core/dataHandler/WITKImageConversion.h"

#include "WSegmentationAlgo.h"

/**
 * Otsu segmentation. Chooses a threshold from the datasets histogram.
 * \class WSegmentationAlgoOtsu
 */
class WSegmentationAlgoOtsu : public WSegmentationAlgo
{
public:
    /**
     * Standard constructor.
     */
    WSegmentationAlgoOtsu();

    /**
     * Destructor.
     */
    virtual ~WSegmentationAlgoOtsu();

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
};

template< typename T >
WSegmentationAlgo::DataSetPtr WSegmentationAlgoOtsu::operator() ( WValueSet< T > const* valueset ) const
{
    typedef itk::Image< T, 3 > ImgType;
    typedef itk::OtsuThresholdImageFilter< ImgType, ImgType > Otsu;

    typename ImgType::Pointer image = makeImageFromDataSet< T >( m_dataSet );
    typename Otsu::Pointer otsu = Otsu::New();
    otsu->SetInput( image );
    otsu->SetOutsideValue( valueset->getMaximumValue() );
    otsu->SetInsideValue( valueset->getMinimumValue() );
    otsu->Update();
    return makeDataSetFromImage< T >( otsu->GetOutput() );
}

#endif  // OW_USE_ITK

#endif  // WSEGMENTATIONALGOOTSU_H

