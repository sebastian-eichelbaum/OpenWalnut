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

#include <string>
#include <algorithm>
#include <vector>

#include "WMDistanceMap.h"

#include "../../kernel/WKernel.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../marchingCubes/WMMarchingCubes.h"

WMDistanceMap::WMDistanceMap():
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
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

const std::string WMDistanceMap::getName() const
{
    return "Distance Map";
}

const std::string WMDistanceMap::getDescription() const
{
    return "This description has to be improved when the module is completed."
        " By now lets say the following: Computes a smoothed version of the dataset"
        " and a distance map on it. Finally it renders this distance map using MarchinCubes";
}



void WMDistanceMap::moduleMain()
{
    // TODO(wiebel): MC fix this hack when possible by using an input connector.
    while ( !WKernel::getRunningKernel() )
    {
        sleep( 1 );
    }
    while ( !WKernel::getRunningKernel()->getDataHandler() )
    {
        sleep( 1 );
    }
    while ( !WKernel::getRunningKernel()->getDataHandler()->getNumberOfSubjects() )
    {
        sleep( 1 );
    }
    boost::shared_ptr< WDataHandler > dh = WKernel::getRunningKernel()->getDataHandler();
    boost::shared_ptr< WSubject > subject = (*dh)[0];
    boost::shared_ptr< const WDataSetSingle > dataSet;

    dataSet = boost::shared_dynamic_cast< const WDataSetSingle >( (*subject)[0] );

    boost::shared_ptr< const WDataSetSingle > distanceMapDataSet = createOffset( dataSet );

    WMMarchingCubes mc;
    mc.generateSurface( distanceMapDataSet, .1);

    WLogger::getLogger()->addLogMessage( "Rendering surface ...", "Distance Map", LL_INFO );

    mc.renderSurface();

    WLogger::getLogger()->addLogMessage( "Done!", "Distance Map", LL_INFO );

}

void WMDistanceMap::connectors()
{
    // initialize connectors

    m_input = boost::shared_ptr<WModuleInputData< boost::shared_ptr< WDataSet > > >(
        new WModuleInputData< boost::shared_ptr< WDataSet > >( shared_from_this(),
                                                               "in", "Dataset to compute isosurface for." )
        );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMDistanceMap::properties()
{
//     ( m_properties->addDouble( "isoValue", 80 ) )->connect( boost::bind( &WMMarchingCubes::slotPropertyChanged, this, _1 ) );
}

boost::shared_ptr< WDataSetSingle > WMDistanceMap::createOffset( boost::shared_ptr< const WDataSetSingle > dataSet )
{
    std::vector<float> floatDataset;

    // TODO(wiebel): we should be able to do all this without the value vector.
    boost::shared_ptr< WValueSet< float > > valueSet = boost::shared_dynamic_cast< WValueSet< float > >( ( *dataSet ).getValueSet() );
    assert( valueSet );
    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( ( *dataSet ).getValueSet() );
    assert( grid );
    const std::vector< float >* source = valueSet->rawDataVectorPointer();


    int b, r, c, bb, rr, r0, b0, c0;
    int i, istart, iend;
    int nbands, nrows, ncols, npixels;
    int d, d1, d2, cc1, cc2;
    float u, dmin, dmax;
    bool *srcpix;
    double g, *array;

//     nbands = m_frame;
//     nrows = m_rows;
//     ncols = m_columns;
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
        if (source->at(i) < 0.01)
            bitmask[i] = true;
        else
            bitmask[i] = false;
    }

    dmax = 999999999.0;

    // first pass
    for( b = 0; b < nbands; ++b)
    {
        for( r = 0; r < nrows; ++r)
        {
            for( c = 0; c < ncols; ++c)
            {
                //if (VPixel(src,b,r,c,VBit) == 1)
                if( bitmask[b * nrows * ncols + r * ncols + c] )
                {
                    floatDataset[b * nrows * ncols + r * ncols + c] = 0;
                    continue;
                }

                srcpix = bitmask + b * nrows * ncols + r * ncols + c;
                cc1 = c;
                while (cc1 < ncols && *srcpix++ == 0)
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
        for( c = 0; c < ncols; c++ )
        {
            for( b = 0; b < nbands; b++ )
                array[b] = static_cast< double >( floatDataset[b * nrows * ncols + r * ncols + c]  );

            for( b = 0; b < nbands; b++ )
            {
                if (bitmask[b * nrows * ncols + r * ncols + c] == 1)
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
        if (floatDataset[i] > max)
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
    boost::shared_ptr< WValueSet< float > > vset;
    vset = boost::shared_ptr< WValueSet< float > >( new WValueSet< float >( valueSet->order(), valueSet->dimension(), floatDataset, W_DT_FLOAT ) );

    boost::shared_ptr< WDataSetSingle > newDataSet;
    newDataSet = boost::shared_ptr< WDataSetSingle >( new WDataSetSingle( vset, grid ) );
    return newDataSet;
}

double WMDistanceMap::xxgauss( double x, double sigma )
{
    double y, z, a = 2.506628273;
    z = x / sigma;
    y = exp( ( double ) -z * z * 0.5 ) / ( sigma * a );
    return y;
}
