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

#include <stdint.h>
#include <string>
#include <algorithm>
#include <vector>

#include "WMDistanceMap.h"
#include "WMDistanceMap.xpm"

#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleFactory.h"
#include "core/dataHandler/WSubject.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/common/WProgress.h"
#include "core/common/WAssert.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMDistanceMap )

WMDistanceMap::WMDistanceMap():
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // NOTE: Do not use the module factory inside this constructor. This will cause a dead lock as the module factory is locked
    // during construction of this instance and can then not be used to create another instance (Isosurface in this case).
}

WMDistanceMap::~WMDistanceMap()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMDistanceMap::factory() const
{
    return boost::shared_ptr< WModule >( new WMDistanceMap() );
}

const char** WMDistanceMap::getXPMIcon() const
{
    return distancemap_xpm;
}


const std::string WMDistanceMap::getName() const
{
    return "Distance Map";
}

const std::string WMDistanceMap::getDescription() const
{
    return "Computes a smoothed version of the dataset and a distance map on it.";
}

void WMDistanceMap::moduleMain()
{
    // use the m_input "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );

    // signal ready state
    ready();

    // loop until the module container requests the module to quit
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // woke up since the module is requested to finish?
        if( m_shutdownFlag() )
        {
            break;
        }

        // acquire data from the input connector
        m_dataSet = m_input->getData();
        if( !m_dataSet )
        {
            debugLog() << "Resetting output.";
            m_output->reset();
            continue;
        }

        // found some data
        debugLog() << "Data changed. Updating ...";
        boost::shared_ptr< WValueSet< float > > distanceMapValueSet = createOffset( m_dataSet );
        m_distanceMapDataSet = boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( distanceMapValueSet, m_dataSet->getGrid() ) );

        WLogger::getLogger()->addLogMessage( "Done!", "Distance Map", LL_INFO );

        // update the output
        m_output->updateData( m_distanceMapDataSet );
    }
}

void WMDistanceMap::connectors()
{
    // initialize connectors

    m_input = boost::shared_ptr<WModuleInputData< WDataSetScalar > >(
        new WModuleInputData< WDataSetScalar >( shared_from_this(),
                                                               "in", "Dataset to compute distance map for." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    m_output = boost::shared_ptr<WModuleOutputData< WDataSetScalar > >(
        new WModuleOutputData< WDataSetScalar >( shared_from_this(),
                                                               "out", "Distance map for the input data set." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_output );

    // call WModule's initialization
    WModule::connectors();
}

void WMDistanceMap::properties()
{
    WModule::properties();
}

template< typename T > boost::shared_ptr< WValueSet< float > > makeFloatValueSetHelper( boost::shared_ptr< WValueSet< T > > inSet )
{
    WAssert( inSet->dimension() == 1, "Works only for scalar data." );
    WAssert( inSet->order() == 0, "Works only for scalar data." );

    boost::shared_ptr< std::vector< float > > data = boost::shared_ptr< std::vector< float > >( new std::vector< float >( inSet->size() ) );
    for( unsigned int i = 0; i < inSet->size(); ++i )
    {
        ( *data )[i] = static_cast< float >( inSet->getScalar( i ) );
    }

    boost::shared_ptr< WValueSet< float > > outSet;
    outSet = boost::shared_ptr< WValueSet< float > >( new WValueSet< float >( ( *inSet ).order(), ( *inSet ).dimension(), data, W_DT_FLOAT ) );

    return outSet;
}

boost::shared_ptr< WValueSet< float > > makeFloatValueSet( boost::shared_ptr< WValueSetBase > inSet )
{
    WAssert( inSet->dimension() == 1, "Works only for scalar data." );
    WAssert( inSet->order() == 0, "Works only for scalar data."  );

    switch( inSet->getDataType() )
    {
        case W_DT_UNSIGNED_CHAR:
            return makeFloatValueSetHelper( boost::dynamic_pointer_cast< WValueSet< unsigned char > >( inSet ) );
            break;
        case W_DT_INT16:
            return makeFloatValueSetHelper( boost::dynamic_pointer_cast< WValueSet< int16_t > >( inSet ) );
            break;
        case W_DT_SIGNED_INT:
            return makeFloatValueSetHelper( boost::dynamic_pointer_cast< WValueSet< int32_t > >( inSet ) );
            break;
        case W_DT_FLOAT:
            return boost::dynamic_pointer_cast< WValueSet< float > >( inSet );
            break;
        default:
            WAssert( false, "Unknow data type in makeFloatDataSet" );
    }

    WAssert( false, "If this assertion is reached, the code above has to be fixed." );
    return boost::shared_ptr< WValueSet< float > >();
}

boost::shared_ptr< WValueSet< float > > WMDistanceMap::createOffset( boost::shared_ptr< const WDataSetScalar > dataSet )
{
    std::vector<float> floatDataset;

    // wiebel: I know that this is not the most speed and memory efficient way to deal with different data types.
    //         However, it seems the most feasible at the moment (2009-11-24).
    boost::shared_ptr< WValueSet< float > > valueSet = makeFloatValueSet( ( *dataSet ).getValueSet() );
    WAssert( valueSet, "Works only for float data sets." );

    boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( ( *dataSet ).getGrid() );
    WAssert( grid, "Works only for data on regular 3D grids."  );

    int b, r, c, bb, rr, r0, b0, c0;
    int i, istart, iend;
    int nbands, nrows, ncols, npixels;
    int d, d1, d2, cc1, cc2;
    float u, dmin, dmax;
    bool *srcpix;
    double g, *array;

    nbands = grid->getNbCoordsZ();
    nrows = grid->getNbCoordsY();
    ncols = grid->getNbCoordsX();

    npixels = std::max( nbands, nrows );

    array = new double[npixels];

    npixels = nbands * nrows * ncols;

    floatDataset.resize( npixels );
    for( int i = 0; i < npixels; ++i)
    {
        floatDataset[i] = 0.0;
    }

    bool* bitmask = new bool[npixels];
    for( int i = 0; i < npixels; ++i)
    {
        if( valueSet->getScalar(i) < 0.01 )
        {
            bitmask[i] = true;
        }
        else
        {
            bitmask[i] = false;
        }
    }

    dmax = 999999999.0;

    // first pass

    boost::shared_ptr< WProgress > progress1 = boost::shared_ptr< WProgress >(
            new WProgress( "Distance Map", nbands + nbands + nrows + nbands + nbands + nbands )
    );
    m_progress->addSubProgress( progress1 );
    for( b = 0; b < nbands; ++b)
    {
        ++*progress1;
        for( r = 0; r < nrows; ++r)
        {
            for( c = 0; c < ncols; ++c)
            {
                //if(VPixel(src,b,r,c,VBit) == 1)
                if( bitmask[b * nrows * ncols + r * ncols + c] )
                {
                    floatDataset[b * nrows * ncols + r * ncols + c] = 0;
                    continue;
                }

                srcpix = bitmask + b * nrows * ncols + r * ncols + c;
                cc1 = c;
                while(cc1 < ncols && *srcpix++ == 0)
                    cc1++;
                d1 = ( cc1 >= ncols ? ncols : ( cc1 - c ) );

                srcpix = bitmask + b * nrows * ncols + r * ncols + c;
                cc2 = c;
                while( cc2 >= 0 && *srcpix-- == 0 )
                    cc2--;
                d2 = ( cc2 <= 0 ? ncols : ( c - cc2 ) );

                if( d1 <= d2 )
                {
                    d = d1;
                    c0 = cc1;
                }
                else
                {
                    d = d2;
                    c0 = cc2;
                }
                floatDataset[b * nrows * ncols + r * ncols + c] = static_cast< float >( d * d );
            }
        }
    }

    // second pass
    for( b = 0; b < nbands; b++ )
    {
        ++*progress1;
        for( c = 0; c < ncols; c++ )
        {
            for( r = 0; r < nrows; r++ )
                array[r] = static_cast< double >( floatDataset[b * nrows * ncols + r * ncols + c] );

            for( r = 0; r < nrows; r++ )
            {
                if( bitmask[b * nrows * ncols + r * ncols + c] == 1 )
                    continue;

                dmin = dmax;
                r0 = r;
                g = sqrt( array[r] );
                istart = r - static_cast< int >( g );
                if( istart < 0 )
                    istart = 0;
                iend = r + static_cast< int >( g + 1 );
                if( iend >= nrows )
                    iend = nrows;

                for( rr = istart; rr < iend; rr++ )
                {
                    u = array[rr] + ( r - rr ) * ( r - rr );
                    if( u < dmin )
                    {
                        dmin = u;
                        r0 = rr;
                    }
                }
                floatDataset[b * nrows * ncols + r * ncols + c] = dmin;
            }
        }
    }

    // third pass
    for( r = 0; r < nrows; r++ )
    {
        ++*progress1;
        for( c = 0; c < ncols; c++ )
        {
            for( b = 0; b < nbands; b++ )
                array[b] = static_cast< double >( floatDataset[b * nrows * ncols + r * ncols + c]  );

            for( b = 0; b < nbands; b++ )
            {
                if(bitmask[b * nrows * ncols + r * ncols + c] == 1)
                    continue;

                dmin = dmax;
                b0 = b;

                g = sqrt( array[b] );
                istart = b - static_cast< int >( g - 1 );
                if( istart < 0 )
                    istart = 0;
                iend = b + static_cast< int >( g + 1 );
                if( iend >= nbands )
                    iend = nbands;

                for( bb = istart; bb < iend; bb++ )
                {
                    u = array[bb] + ( b - bb ) * ( b - bb );
                    if( u < dmin )
                    {
                        dmin = u;
                        b0 = bb;
                    }
                }
                floatDataset[b * nrows * ncols + r * ncols + c] = dmin;
            }
        }
    }

    delete[] array;

    float max = 0;
    for( i = 0; i < npixels; ++i)
    {
        floatDataset[i] = sqrt( static_cast< double >( floatDataset[i] ) );
        if(floatDataset[i] > max)
            max = floatDataset[i];
    }
    for( i = 0; i < npixels; ++i)
    {
        floatDataset[i] = floatDataset[i] / max;
    }

    // filter with gauss
    // create the filter kernel
    double sigma = 4;

    int dim = static_cast< int >( ( 3.0 * sigma + 1 ) );
    int n = 2* dim + 1;
    double step = 1;

    float* kernel = new float[n];

    double sum = 0;
    double x = -static_cast< float >( dim );

    double uu;
    for( int i = 0; i < n; ++i)
    {
        uu = xxgauss( x, sigma );
        sum += uu;
        kernel[i] = uu;
        x += step;
    }

    /* normalize */
    for( int i = 0; i < n; ++i)
    {
        uu = kernel[i];
        uu /= sum;
        kernel[i] = uu;
    }

    d = n / 2;
    float* float_pp;
    std::vector<float> tmp( npixels );
    int c1, cc;

    for( int i = 0; i < npixels; ++i )
    {
        tmp[i] = 0.0;
    }

    for( b = 0; b < nbands; ++b )
    {
        ++*progress1;
        for( r = 0; r < nrows; ++r )
        {
            for( c = d; c < ncols - d; ++c )
            {
                float_pp = kernel;
                sum = 0;
                c0 = c - d;
                c1 = c + d;
                for( cc = c0; cc <= c1; cc++ )
                {
                    x = floatDataset[b * nrows * ncols + r * ncols + cc];
                    sum += x * ( *float_pp++ );
                }
                tmp[b * nrows * ncols + r * ncols + c] = sum;
            }
        }
    }
    int r1;
    for( b = 0; b < nbands; ++b )
    {
        ++*progress1;
        for( r = d; r < nrows - d; ++r )
        {
            for( c = 0; c < ncols; ++c )
            {
                float_pp = kernel;
                sum = 0;
                r0 = r - d;
                r1 = r + d;
                for( rr = r0; rr <= r1; rr++ )
                {
                    x = tmp[b * nrows * ncols + rr * ncols + c];
                    sum += x * ( *float_pp++ );
                }
                floatDataset[b * nrows * ncols + r * ncols + c] = sum;
            }
        }
    }
    int b1;
    for( b = d; b < nbands - d; ++b )
    {
        ++*progress1;
        for( r = 0; r < nrows; ++r )
        {
            for( c = 0; c < ncols; ++c )
            {
                float_pp = kernel;
                sum = 0;
                b0 = b - d;
                b1 = b + d;
                for( bb = b0; bb <= b1; bb++ )
                {
                    x = floatDataset[bb * nrows * ncols + r * ncols + c];
                    sum += x * ( *float_pp++ );
                }
                tmp[b * nrows * ncols + r * ncols + c] = sum;
            }
        }
    }
    delete[] bitmask;
    delete[] kernel;

    floatDataset = tmp;
    boost::shared_ptr< WValueSet< float > > resultValueSet;
    resultValueSet = boost::shared_ptr< WValueSet< float > >(
        new WValueSet< float >( valueSet->order(), valueSet->dimension(),
                                boost::shared_ptr< std::vector< float > >( new std::vector< float >( floatDataset ) ), W_DT_FLOAT ) );

    progress1->finish();

    return resultValueSet;
}

double WMDistanceMap::xxgauss( double x, double sigma )
{
    double y, z, a = 2.506628273;
    z = x / sigma;
    y = exp( -z * z * 0.5 ) / ( sigma * a );
    return y;
}
