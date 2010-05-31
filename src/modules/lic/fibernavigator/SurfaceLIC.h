// vim:nolist

/*
 * SurfaceLIC.h
 *
 *  Created on: 17.11.2008
 *      Author: ralph
 */

#ifndef SURFACELIC_H_
#define SURFACELIC_H_

#include "FStreamlineOnSurfaceEuler.h"
#include "../../../graphicsEngine/WTriangleMesh2.h"
#include "../../../dataHandler/WDataSetVector.h"
#include "triangleMesh.h"

class SurfaceLIC
{
public:
    SurfaceLIC( boost::shared_ptr< WDataSetVector > vectors, boost::shared_ptr< WTriangleMesh2 > mesh );
    virtual ~SurfaceLIC();
    virtual void execute();
    std::vector< std::vector<float> > testLines;
    /**
     * Converts old mesh into new WTriangleMesh2
     *
     * \return reference to the mesh
     */
    void updateMeshColor( boost::shared_ptr< WTriangleMesh2 > mesh ) const;

private:

    class MyLICStreamline: public FStreamlineOnSurfaceEuler
    {
    public:

        MyLICStreamline( boost::shared_ptr< WDataSetVector > vectors, TriangleMesh* mesh )
            : FStreamlineOnSurfaceEuler( vectors, mesh )
        {
        }

        ~MyLICStreamline( void )
        {
        }

        void setParams( const std::vector<positive> *the_hits, positive length, positive athreshold )
        {
            hits = the_hits;
            min_length = length;
            threshold = athreshold;
        }

        virtual bool proceed( void )
        {
            return ( visitedCells.size() < min_length || ( *hits)[currCell] < threshold );
        }

    private:
        const std::vector<positive> *hits;
        positive min_length;
        positive threshold;
    };

    boost::shared_ptr< WDataSetVector > m_vectors;
    TriangleMesh* m_mesh;
    int nbTriangles;

    // profile stuff
    double max_length;
    unsigned int nbFold;
    double offset;
    unsigned int modulo;
    bool optimizeShape, black;

    positive min_length;
    positive threshold;

    //  double normalx, normaly, normalz;
    void calculatePixelLuminance( const FIndex& cellId );

    MyLICStreamline *streamline;
    FArray start;
    std::vector<FIndex> ids, visitedFwd, visitedBwd;
    std::list<double> fifo;
    FArray pos[3];

    positive getId( positive i, const std::vector<FIndex>& bwd, const std::vector<FIndex>& fwd );

    std::vector<float> input_texture, output_texture;
    std::vector<positive> hit_texture;

    positive nbVisited;

    void displayTexture();
};

#endif /* SURFACELIC_H_ */
