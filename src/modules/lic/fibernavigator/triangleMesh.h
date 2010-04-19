#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H
/*****************************************************************
*
* TriangleMesh.h
*
* Author:    Russ Stimpson
* Date:        2/7/03
*
*****************************************************************/

#include "Vector.h"
#include <vector>

#include "../fantom/FIndex.h"
#include "../fantom/FArray.h"
#include "../../../graphicsEngine/WTriangleMesh2.h"
#include "../../../common/WColor.h"
#include "../../../dataHandler/WGridRegular3D.h"

#define PI 3.14159

struct Triangle {
    unsigned int pointID[3];
};

class TriangleMesh {

    // Attributes
    private:
        std::vector<Vector> vertices;
        std::vector<Vector> vertNormals;
        std::vector<WColor> vertColors;
        std::vector < std::vector<unsigned int> >vIsInTriangle;

        std::vector< Triangle > triangles;
        std::vector<Vector> triNormals;
        std::vector< int >triangleTensor;
        std::vector<WColor>triangleColor;
        std::vector< std::vector<int> > neighbors;

        int    numVerts;
        int    numTris;

        // we don't delete vertices yet, so can do a cleanup only once
        bool isCleaned;

        bool m_vertNormalsCalculated;
        bool m_neighborsCalculated;
        bool m_triangleTensorsCalculated;

        WColor defaultColor;
        boost::shared_ptr< WTriangleMesh2 > m_mesh;
        boost::shared_ptr< WGridRegular3D > m_grid;

    // Construction
    public:
        TriangleMesh ( boost::shared_ptr< WTriangleMesh2 > mesh, boost::shared_ptr< WGridRegular3D > grid );
        ~TriangleMesh ();

    // Operations
    public:
        void addVert(const Vector newVert);
        void addVert(const float x, const float y, const float z);
        void fastAddVert(const Vector newVert);

        void addTriangle(const int vertA, const int vertB, const int vertC);
        void addTriangle(const int vertA, const int vertB, const int vertC, const int tensorIndex);
        void fastAddTriangle(const int vertA, const int vertB, const int vertC);

        void clearMesh();

        void reserveVerts(const int size);
        void reserveTriangles(const int size);
        void resizeVerts(const int size);
        void resizeTriangles(const int size);
        int getNumVertices();
        int getNumTriangles();

        Vector getVertex (const int vertNum);
        Vector getVertex (const int triNum, int pos);
        Vector getNormal(const int triNum);
        Vector getVertNormal(const int vertNum);
        WColor getVertColor (const int vertNum);
        Triangle getTriangle(const int triNum);
        WColor getTriangleColor(const int triNum);
        std::vector<unsigned int> getStar(const int vertNum);
        std::vector<Vector> getVerts();

        int getTriangleTensor(const int triNum);
        Vector getTriangleCenter(int triNum) ;

        void setVertex(const unsigned int vertNum, const Vector nPos);
        void setVertColor(const int vertNum, const WColor color);

        void eraseTriFromVert( const unsigned int triNum, const unsigned int vertNum);
        void setTriangle(const unsigned int triNum, const unsigned int vertA, const unsigned int vertB, const unsigned int vertC);
        void setTriangleColor(const unsigned int triNum, const float r, const float g, const float b, const float a);
        void setTriangleColor(const unsigned int triNum, const float r, const float g, const float b);
        void setTriangleAlpha(const unsigned int triNum, const float a);
        void setTriangleRed(const unsigned int triNum, const float r);
        void setTriangleGreen(const unsigned int triNum, const float g);
        void setTriangleBlue(const unsigned int triNum, const float b);

        bool isInTriangle(const unsigned int vertNum, const unsigned int triangleNum);

        bool hasEdge(const unsigned int coVert1, const unsigned int coVert2, const unsigned int triangleNum);
        int getThirdVert(const unsigned int coVert1, const unsigned int coVert2, const unsigned int triangleNum);

        int getNextVertex(const unsigned int triNum, const unsigned int vertNum);

        void cleanUp();
        void doLoopSubD();

        void getCellVerticesIndices( const FIndex& cellId, std::vector< FIndex >& vertices );
        void getPosition( FPosition& resultPos, const FIndex& pIndex );
        void getEdgeNeighbor( const FIndex& cellId, int pos, std::vector< FIndex >& neigh );
        void getNeighbors( const FIndex& vertId, std::vector< FIndex >& neighs );
        int getNeighbor(const unsigned int coVert1, const unsigned int coVert2, const unsigned int triangleNum);

        void printInfo();

    private:
        Vector calcTriangleNormal(const Triangle);
        Vector calcTriangleNormal(const int triNum);
        Vector calcVertNormal(const int vertNum);
        void calcTriangleTensors();
        void calcNeighbors();
        void calcVertNormals();

        void flipNormals();


        int calcTriangleTensor(const int triNum);
        void calcNeighbor(const int triangleNum);
};
#endif
