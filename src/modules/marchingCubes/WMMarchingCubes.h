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

#ifndef WMMARCHINGCUBES_H
#define WMMARCHINGCUBES_H

#include <map>
#include <string>
#include <vector>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.hpp"
#include "WTriangleMesh.h"

struct WPointXYZId
{
    unsigned int newID;
    double x, y, z;
};

typedef std::map< unsigned int, WPointXYZId > ID2WPointXYZId;

struct WMCTriangle
{
    unsigned int pointID[3];
};

typedef std::vector<WMCTriangle> WMCTriangleVECTOR;

// -------------------------------------------------------
//
// Numbering of edges (0..B) and vertices (0..7) per cube.
//
//      5--5--6
//     /|    /|
//    4 |   6 |    A=10
//   /  9  /  A
//  4--7--7   |
//  |   | |   |
//  |   1-|1--2
//  8  /  B  /
//  | 0   | 2      B=11
//  |/    |/
//  0--3--3
//
//  |  /
//  z y
//  |/
//  0--x--
//
// -------------------------------------------------------

/**
 * Module implementing the marching cubes algorithm with consistent triangulation for data
 * given on rgualr grids with axis-aligned cells.
 */
class WMMarchingCubes : public WModule
{
/**
 * Only UnitTests may be friends.
 */
friend class WMMarchingCubesTest;

public:
    /**
     * Standard constructor.
     */
    WMMarchingCubes();

    /**
     * Destructor.
     */
    ~WMMarchingCubes();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description of module.
     */
    virtual const std::string getDescription() const;

    /**
     * Determine what to do if a property was changed.
     */
    void slotPropertyChanged( std::string propertyName );

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     * 
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void threadMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

private:
    boost::shared_ptr< WModuleInputData< boost::shared_ptr< WDataSet > > > m_input;  //!< Input connector required by this module.

    static const unsigned int m_edgeTable[256];  //!< Lookup table for edges used in the construction of the isosurface.
    static const int m_triTable[256][16];  //!< Lookup table for triangles used in the construction of the isosurface.

    boost::shared_ptr< const WDataSetSingle > m_dataSet;  //!< Convenience pointer to the data set we are dealing with
    boost::shared_ptr< WValueSet< unsigned char > > m_vals;  //!< Convenience pointer to the values set we are dealing with

    /**
     * Generate the triangles for the surface
     */
    void generateSurface( double isoValue );

    /**
     * Calculates the intersection point of the isosurface with an
     * edge.
     */
    WPointXYZId calculateIntersection( unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo );

    /**
     * Interpolates between two grid points to produce the point at which
     * the isosurface intersects an edge.
     */
    WPointXYZId interpolate( double fX1, double fY1, double fZ1, double fX2, double fY2, double fZ2, double tVal1, double tVal2 );

    /**
     * Returns the edge ID.
     */
    int getEdgeID( unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo );

    /**
     * Returns the vertex ID.
     */
    unsigned int getVertexID( unsigned int nX, unsigned int nY, unsigned int nZ );

    unsigned int m_nCellsX;  //!< No. of cells in x direction.
    unsigned int m_nCellsY;  //!< No. of cells in y direction.
    unsigned int m_nCellsZ;  //!< No. of cells in z direction.

    double m_fCellLengthX;  //!< Cell length in x direction.
    double m_fCellLengthY;  //!< Cell length in y direction.
    double m_fCellLengthZ;  //!< Cell length in z direction.

    double m_tIsoLevel;  //!< The isovalue.

    ID2WPointXYZId m_idToVertices;  //!< List of WPointXYZIds which form the isosurface.
    WMCTriangleVECTOR m_trivecTriangles;  //!< List of WMCTriangleS which form the triangulation of the isosurface.

    // TODO(wiebel): MC comment this;
    void renderSurface();
    void renderMesh( const WTriangleMesh& mesh );

    /**
     * Store the mesh in legacy vtk file format.
     */
    bool save( std::string fileName, const WTriangleMesh& triMesh ) const;

    /**
     * Load meshes saved with WMMarchingCubes::save
     */
    WTriangleMesh load( std::string fileName );
};

#endif  // WMMARCHINGCUBES_H
