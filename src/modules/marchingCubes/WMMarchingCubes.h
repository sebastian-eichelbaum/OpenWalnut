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
#include "../../kernel/WModuleInputData.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../graphicsEngine/WShader.h"
#include "../../dataHandler/WTriangleMesh.h"

/**
 * A point consisting of its coordinates and ID
 */
struct WPointXYZId
{
    unsigned int newID; //!< ID of the point
    double x; //!< x coordinates of the point.
    double y; //!< y coordinates of the point.
    double z; //!< z coordinates of the point.
};

typedef std::map< unsigned int, WPointXYZId > ID2WPointXYZId;

/**
 * Encapsulated ids representing a triangle.
 */
struct WMCTriangle
{
    unsigned int pointID[3]; //!< The IDs of the vertices of the triangle.
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
 * given on regular grids with axis-aligned cells.
 * \ingroup modules
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
     * \param propertyName Name of the property.
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
     * Get the icon for this module in XPM format.
     */
    virtual const char** getXPMIcon() const;

    /**
     * Generate the triangles for the surface on the given dataSet (inGrid, vals).
     * \param inGrid The grid of the data set
     * \param vals the value set of the data set
     * \param isoValue The surface will run through all positions with this value.
     */
    template< typename T > void generateSurface( boost::shared_ptr< WGrid > inGrid, boost::shared_ptr< WValueSet< T > > vals, double isoValue );

    /**
     * Activate the rendering of the computed surface.
     * This converts the surface to a WTriangleMesh and calls renderMesh afterwards
     */
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

    /**
     * Used as callback which simply sets m_textureChanged to true. Called by WSubject whenever the datasets change.
     */
    void notifyTextureChange();

    /**
     * True when textures haven changed.
     */
    bool m_textureChanged;

    /**
     * This condition denotes whether the properties have changed.
     */
    boost::shared_ptr< WCondition > m_propertiesChanged;

    /**
     * Prepares and commits everything for rendering with the OSG
     * \param mesh The mesh that will be rendered.
     */
    void renderMesh( WTriangleMesh* mesh );

    boost::shared_ptr< WModuleInputData< WDataSetSingle > > m_input;  //!< Input connector required by this module.

    static const unsigned int m_edgeTable[256];  //!< Lookup table for edges used in the construction of the isosurface.
    static const int m_triTable[256][16];  //!< Lookup table for triangles used in the construction of the isosurface.

    /**
     * Calculates the intersection point id of the isosurface with an
     * edge.
     * \param vals the value set that determines the values at the vertices
     * \param nX id of cell in x direction
     * \param nY id of cell in y direction
     * \param nZ id of cell in z direction
     * \param nEdgeNo id of the edge the point that will be interpolates lies on
     */
    template< typename T > WPointXYZId calculateIntersection( boost::shared_ptr< WValueSet< T > > vals,
                                                              unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo );

    /**
     * Interpolates between two grid points to produce the point at which
     * the isosurface intersects an edge.
     * \param fX1 x coordinate of first position
     * \param fY1 y coordinate of first position
     * \param fZ1 z coordinate of first position
     * \param fX2 x coordinate of second position
     * \param fY2 y coordinate of first position
     * \param fZ2 z coordinate of first position
     * \param tVal1 scalar value at first position
     * \param tVal2 scalar value at second position
     */
    WPointXYZId interpolate( double fX1, double fY1, double fZ1, double fX2, double fY2, double fZ2, double tVal1, double tVal2 );

    /**
     * Returns the edge ID.
     * \param nX ID of desired cell along x axis
     * \param nY ID of desired cell along y axis
     * \param nZ ID of desired cell along z axis
     * \param nEdgeNo id of edge inside cell
     * \return The id of the edge in the large array.
     */
    int getEdgeID( unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo );

    /**
     * Returns the ID of the vertex given by by the IDs along the axis
     * \param nX ID of desired vertex along x axis
     * \param nY ID of desired vertex along y axis
     * \param nZ ID of desired vertex along z axis
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

    /**
     * Store the mesh in legacy vtk file format.
     * \param fileName the file where the triMesh will be written to
     * \param triMesh this mesh will be stored.
     */
    bool save( std::string fileName, const WTriangleMesh& triMesh ) const;

    /**
     * Load meshes saved with WMMarchingCubes::save
     * \param fileName the mesh will be loaded from this file
     */
    WTriangleMesh load( std::string fileName );

    /**
     * Kind of a convenience function for generate surface.
     * It performs the conversions of the value sets of different data types.
     * \param isoValue The surface will represent this value.
     */
    void generateSurfacePre( double isoValue );

    boost::shared_ptr< const WDataSetSingle > m_dataSet; //!< pointer to dataSet to be able to access it throughout the whole module.
    boost::shared_ptr< WGridRegular3D > m_grid; //!< pointer to grid, because we need to access the grid for the dimensions of the texture.


    bool m_shaderUseTexture; //!< shall the shader use texturing?
    bool m_shaderUseLighting; //!< shall the shader use lighting?
    bool m_shaderUseTransparency; //!< shall the shader use transparency?

    /**
     * Lock to prevent concurrent threads trying to update the osg node
     */
    boost::shared_mutex m_updateLock;

    osg::ref_ptr< osg::Geode > m_geode; //!< Pointer to geode. We need it to be able to update it when callback is invoked.

    /**
     * The shader used for the iso surface in m_geode
     */
    osg::ref_ptr< WShader > m_shader;

    std::vector< osg::ref_ptr< osg::Uniform > > m_typeUniforms; //!< uniforms for ...... ? for shader
    std::vector< osg::ref_ptr< osg::Uniform > > m_alphaUniforms; //!< uniforms for opacities of textures in shader
    std::vector< osg::ref_ptr< osg::Uniform > > m_thresholdUniforms; //!< uniforms for thresholds of textures in shader
    std::vector< osg::ref_ptr< osg::Uniform > > m_samplerUniforms; //!< uniforms for ids of textures in shader
};

/**
 * Adapter object for realizing callbacks of the node representing the isosurface in the osg
 */
class SurfaceNodeCallback : public osg::NodeCallback
{
public:
    /**
     * Constructor of the callback adapter.
     * \param module A function of this module will be called
     */
    explicit SurfaceNodeCallback( boost::shared_ptr< WMMarchingCubes > module );

    /**
     * Function that is called by the osg and that call the function in the module.
     * \param node The node we are called.
     * \param nv the visitor calling us.
     */
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

private:
    boost::shared_ptr< WMMarchingCubes > m_module; //!< Pointer to the module to which the function that is called belongs to.
};

inline SurfaceNodeCallback::SurfaceNodeCallback( boost::shared_ptr< WMMarchingCubes > module )
    : m_module( module )
{
}

inline void SurfaceNodeCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    if ( m_module )
    {
        m_module->updateTextures();
    }
    traverse( node, nv );
}

#endif  // WMMARCHINGCUBES_H
