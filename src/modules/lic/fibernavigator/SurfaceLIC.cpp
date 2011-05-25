/*
 * SurfaceLIC.cpp
 *
 *  Created on: 17.11.2008
 *      Author: ralph
 */

#include <exception>

#include "SurfaceLIC.h"
#include "../fantom/FTensor.h"
#include "core/common/WLogger.h"

SurfaceLIC::SurfaceLIC( boost::shared_ptr< WDataSetVector > vectors, boost::shared_ptr< WTriangleMesh > mesh )
    : m_vectors( vectors )
{
    WAssert( boost::shared_dynamic_cast< WGridRegular3D >( vectors->getGrid() ), "Only vector field with regular grid are allowed... aborting" );
    m_mesh = new TriangleMesh( mesh, boost::shared_dynamic_cast< WGridRegular3D >( vectors->getGrid() ) );

    nbFold = 20;
    max_length = 10.;
    modulo = 20;

    black = true;

    min_length = 10;
    threshold = 10;

    nbTriangles = m_mesh->getNumTriangles();
}

SurfaceLIC::~SurfaceLIC()
{
    // TODO Auto-generated destructor stub
    delete m_mesh;
}

void SurfaceLIC::updateMeshColor( boost::shared_ptr< WTriangleMesh > mesh ) const
{
    WAssert( static_cast< int >( mesh->triangleSize() ) == m_mesh->getNumTriangles(), "Meshes have not same number of triangles" );
    for( int i = 0; i < m_mesh->getNumTriangles(); ++i )
    {
        mesh->setTriangleColor( i, m_mesh->getTriangleColor( i ) );
    }
}

//---------------------------------------------------------------------------

void SurfaceLIC::execute()
{
    try
    {
        nbVisited = 0;

        testLines.clear();

        // create textures
        output_texture.resize(m_mesh->getNumTriangles());
        input_texture.resize(m_mesh->getNumTriangles());
        hit_texture.resize(m_mesh->getNumTriangles());

        // setup all the textures
        // create a random input texture of luminance values
#ifdef _WIN32
        srand(time(0));
#else
        srand48(time(0));
#endif
        for(int i = 0; i < m_mesh->getNumTriangles(); ++i)
        {
#ifdef _WIN32
            input_texture[i] = (float) rand()/ (RAND_MAX + 1);
#else
            input_texture[i] = (float) drand48();
#endif
            output_texture[i] = 0.0;
            hit_texture[i] = 0;
        }
        // cell-based streamlines
        streamline = new MyLICStreamline(m_vectors, m_mesh);
        streamline->setParams(&hit_texture, min_length, threshold);

        wlog::debug( "SurfaceLIC" ) << "start calculating lic";
//        m_dh->printDebug(_T("start calculating lic"), 1);

        // iterate over all texture points
        positive m, q, n = 0;
        m = (m_mesh->getNumTriangles() - 1) / modulo;
        q = (m_mesh->getNumTriangles() - 1) % modulo;

        for(positive i = 0; i <= q; ++i)
            for(positive j = 0; j <= m; ++j, ++n)
            {
                calculatePixelLuminance(FIndex(modulo * j + i));
            }

        for(positive i = q + 1; i < modulo; ++i)
            for(positive j = 0; j < m; ++j, ++n)
            {
                calculatePixelLuminance(FIndex(modulo * j + i));
            }
        wlog::debug( "SurfaceLIC" ) << "lic done";
        //m_dh->printDebug(_T("lic done"), 1);

    } catch (FException& e)
    {
        if(streamline)
            delete streamline;

        e.addTraceMessage("void SurfaceLIC::execute ()");
        throw ;
    }

    displayTexture();

    if( streamline ) delete streamline;
}

//---------------------------------------------------------------------------

void SurfaceLIC::calculatePixelLuminance(const FIndex& cellId)
{
    //   cout << "calculatePixelLuminance: cell #" << cellId << " ";
    // already calculated ?
    if(hit_texture[cellId.getIndex()])
    {
        return;
    }

    // compute mean point
    m_mesh->getCellVerticesIndices(cellId, ids);
    for(positive j = 0; j < ids.size(); j++)
    {
        m_mesh->getPosition(pos[j], ids[j]);
    }
    start = 1. / 3. * (pos[0] + pos[1] + pos[2]);

    // integrate
    streamline->integrate(start, cellId, true, max_length);
    visitedFwd = streamline->getVisitedCells();
#ifdef __DRAW_STREAMLINES__
    std::vector<float>line;
    std::vector< FArray > steps = streamline->getIntermediateSteps();
    line.reserve(steps.size() *3);
    for( positive i=0; i<steps.size(); ++i )
    {
        line.push_back(steps[i](0));
        line.push_back(steps[i](1));
        line.push_back(steps[i](2));
    }
    testLines.push_back(line);
#endif
    streamline->integrate(start, cellId, false, max_length);
    visitedBwd = streamline->getVisitedCells();
#ifdef __DRAW_STREAMLINES__
    line.clear();
    steps = streamline->getIntermediateSteps();
    line.reserve(steps.size() *3);
    for( positive i=0; i<steps.size(); ++i )
    {
        line.push_back(steps[i](0));
        line.push_back(steps[i](1));
        line.push_back(steps[i](2));
    }
    testLines.push_back(line);
#endif

    positive total_sz = visitedFwd.size() + visitedBwd.size();

    // adjust kernel size to streamline length
    positive kernel_sz = nbFold;
    while(total_sz < 2 * kernel_sz)
        kernel_sz /= 2;
    if(kernel_sz < 2)
    {
        return;
    }
    double div = 1. / (2. * (double) kernel_sz);

    // compute convolution

    fifo.clear();
    double sum = 0.;
    // initialize convolution kernel
    for(positive i = 0; i < kernel_sz; i++)
    {
        fifo.push_back(input_texture[getId(i, visitedBwd, visitedFwd)]);
        sum += fifo.back();
    }
    // loop over streamline
    positive front = kernel_sz, curr = 0;
    double mult = div;
    for(; curr < total_sz; curr++, front++)
    {
        if(!black)
        {
            if(front < 2 * kernel_sz)
                mult = 1. / (double) front;
            else if(front < total_sz)
                mult = div;
            else
                mult = 1. / (double) (kernel_sz - curr + total_sz);
        }

        positive id = getId(curr, visitedBwd, visitedFwd);
        output_texture[id] += sum * mult;

        if(!hit_texture[id])
            ++nbVisited;

        ++hit_texture[id];
        if(front >= 2 * kernel_sz)
        {
            sum -= fifo.front();
            fifo.pop_front();
        }
        if(front < total_sz)
        {
            fifo.push_back(input_texture[getId(front, visitedBwd, visitedFwd)]);
            sum += fifo.back();
        }
    }
}

//---------------------------------------------------------------------------

positive SurfaceLIC::getId(positive i, const std::vector<FIndex>& bwd, const std::vector<FIndex>& fwd)
{
    //   if( i > bwd.size()+fwd.size() )
    //     return ( positive )-1;

    if(i < bwd.size())
        return bwd[bwd.size() - i - 1].getIndex();
    else
        return fwd[i - bwd.size()].getIndex();
}

//---------------------------------------------------------------------------

class sort_ratio
{
public:
    int operator()(std::pair<FIndex, double> p1, std::pair<FIndex, double> p2) const
    {
        return p1.second > p2.second;
    }
};

//---------------------------------------------------------------------------

void SurfaceLIC::displayTexture()
{
    double gray;

    for(int i = 0; i < nbTriangles; ++i)
    {
        if(hit_texture[i])
            gray = output_texture[i] / (double) hit_texture[i];
        else
        {
            printf("miss in hit texture\n");
            gray = input_texture[i];
            m_mesh->setTriangleAlpha(i, 0.0);
        }
       //gray = (m_mesh->getTriangleColor(i)).Alpha()/255.0;

        m_mesh->setTriangleRed(i, gray);
        m_mesh->setTriangleAlpha(i, input_texture[i]);
    }
}

//---------------------------------------------------------------------------

