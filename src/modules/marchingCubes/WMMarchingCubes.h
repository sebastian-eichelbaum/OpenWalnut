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

#include <osg/Node>
#include <osg/Geode>
#include <osg/Uniform>

#include "../../kernel/WModule.h"
#include "../../kernel/WModuleInputData.hpp"
#include "../../dataHandler/WGridRegular3D.h"
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
     * Connect the listener function of the module to the gui signals
     * this has to be called after full initialization fo the gui
     */
    void connectToGui();

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

    /**
     * Generate the triangles for the surface
     */
    template< typename T > void generateSurface( boost::shared_ptr< WGrid > inGrid, boost::shared_ptr< WValueSet< T > > vals, double isoValue );

    // TODO(wiebel): MC document this;
    void renderSurface();

    /**
     *  updates textures and shader parameters
     */
    void updateTextures();

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

private:
    boost::shared_ptr< WModuleInputData< boost::shared_ptr< WDataSet > > > m_input;  //!< Input connector required by this module.

    static const unsigned int m_edgeTable[256];  //!< Lookup table for edges used in the construction of the isosurface.
    static const int m_triTable[256][16];  //!< Lookup table for triangles used in the construction of the isosurface.

    /**
     * Calculates the intersection point of the isosurface with an
     * edge.
     */
    template< typename T > WPointXYZId calculateIntersection( boost::shared_ptr< WValueSet< T > > vals,
                                                              unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo );

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

    // TODO(wiebel): MC document this;
    void renderMesh( WTriangleMesh* mesh );

    /**
     * Store the mesh in legacy vtk file format.
     */
    bool save( std::string fileName, const WTriangleMesh& triMesh ) const;

    /**
     * Load meshes saved with WMMarchingCubes::save
     */
    WTriangleMesh load( std::string fileName );

    boost::shared_ptr< WGridRegular3D > m_grid; //!< pointer to grid, becaus we need to acces the grid for the dimensions of the texture.

    bool m_shaderUseTexture; //!< shall the shader use texturing?
    bool m_shaderUseLighting; //!< shall the shader use lighting?

    /**
     * Lock to prevent concurrent threads trying to update the osg node
     */
    boost::shared_mutex m_updateLock;

    osg::Geode* m_geode; //!< Pointer to geode. We need it to be able to update it when callback is invoked.

    std::vector< osg::ref_ptr< osg::Uniform > > m_typeUniforms; //!< uniforms for ...... ? for shader
    std::vector< osg::ref_ptr< osg::Uniform > > m_alphaUniforms; //!< uniforms for opacities of textures in shader
    std::vector< osg::ref_ptr< osg::Uniform > > m_thresholdUniforms; //!< uniforms for thresholds of textures in shader
    std::vector< osg::ref_ptr< osg::Uniform > > m_samplerUniforms; //!< uniforms for ids of textures in shader
};

class surfaceNodeCallback : public osg::NodeCallback
{
public:
    explicit surfaceNodeCallback( boost::shared_ptr< WMMarchingCubes > module )
        {
            m_module = module;
        }

    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
        {
            if ( m_module )
            {
                m_module->updateTextures();
            }
            traverse( node, nv );
        }
private:
    boost::shared_ptr< WMMarchingCubes > m_module;
};

#endif  // WMMARCHINGCUBES_H
