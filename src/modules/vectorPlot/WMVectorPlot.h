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

#ifndef WMVECTORPLOT_H
#define WMVECTORPLOT_H

#include <string>
#include <vector>

#include <osg/Geode>
#include <osg/Geometry>

#include "core/dataHandler/WDataSetVector.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

#include "core/common/math/linearAlgebra/WLinearAlgebra.h"

/**
 * Show an glyph plot (in this case the glyphs are arrows) of a vector data set.
 * \ingroup modules
 */
class WMVectorPlot: public WModule
{
public:

    /**
     * Default constructor.
     */
    WMVectorPlot();

    /**
     * Destructor.
     */
    virtual ~WMVectorPlot();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     * \return The icon.
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
     * The root node used for this modules graphics. For OSG nodes, always use osg::ref_ptr to ensure proper resource management.
     */
    osg::ref_ptr<osg::Geode> m_rootNode;

    /**
     * Callback for m_active. Overwrite this in your modules to handle m_active changes separately.
     */
    virtual void activate();

private:
    /**
     * Creates a vector plot on the currently selected slices.
     *
     * \return geometry containing the slices
     * \tparam ValueType the real type of the valueset
     */
    template< typename ValueType >
    osg::ref_ptr<osg::Geometry> buildPlotSlices();

    /**
     * Creates a vector plot on the currently selected slices. Handles valuesets of different types properly.
     *
     * \return geometry containing the slices
     */
    osg::ref_ptr<osg::Geometry> buildPlotSlices();

    /**
     * The update callback that is called for the osg node of this module.
     */
    void updateCallback();

    /**
     * Transforms the given vertices according to m_matrix
     * \param verts These vertices will be transformed.
     */
    void transformVerts( osg::ref_ptr< osg::Vec3Array > verts );


    /**
     * An input connector used to get datasets from other modules. The connection management between connectors must not be handled by the module.
     */
    boost::shared_ptr< WModuleInputData< WDataSetVector > > m_input;

    /**
     * This is a pointer to the dataset the module is currently working on.
     */
    boost::shared_ptr< WDataSetVector > m_dataSet;

    /**
     * list of positions to plot vectors on, this will come from a selection tool class
     */
    boost::shared_ptr< std::vector< WPosition > > m_positions;

    WPropBool     m_projectOnSlice; //!< projects the vectors on the slice.

    WPropBool     m_coloringMode; //!< indicates a set color or direction color coding.

    WPropDouble      m_xSlice; //!< x position of the slice

    WPropDouble      m_ySlice; //!< y position of the slice

    WPropDouble      m_zSlice; //!< z position of the slice

    WPropBool     m_showOnSagittal; //!< indicates whether the vector should be shown on sagittal slice

    WPropBool     m_showOnCoronal; //!< indicates whether the vector should be shown on coronal slice

    WPropBool     m_showOnAxial; //!< indicates whether the vector should be shown on axial slice


    WPropColor    m_aColor; //!< color

    osg::ref_ptr< WGEShader > m_shader; //!< the shader object for this module

    WPosition m_oldPos; //!< The previous position of the slides.
};

template< typename ValueType >
osg::ref_ptr<osg::Geometry> WMVectorPlot::buildPlotSlices()
{
    boost::shared_ptr< WProgress > progress = boost::shared_ptr< WProgress >( new WProgress( "Vector Plot", 4 ) );
    m_progress->addSubProgress( progress );

    WPosition current = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();

    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
    boost::shared_ptr< WValueSet< ValueType > > vals = boost::shared_dynamic_cast< WValueSet< ValueType > >( m_dataSet->getValueSet() );

    int maxX = grid->getNbCoordsX() - 1;
    int maxY = grid->getNbCoordsY() - 1;
    int maxZ = grid->getNbCoordsZ() - 1;
    int nbX = maxX + 1;
    int nbY = maxY + 1;
    int nbZ = maxZ + 1;

    m_xSlice->setMax( grid->getNbCoordsX() - 1 );
    m_ySlice->setMax( grid->getNbCoordsY() - 1 );
    m_zSlice->setMax( grid->getNbCoordsZ() - 1 );

    WPosition texPos = grid->worldCoordToTexCoord( current );
    double xSlice = texPos[0] * grid->getNbCoordsX() - 0.5;
    double ySlice = texPos[1] * grid->getNbCoordsY() - 0.5;
    double zSlice = texPos[2] * grid->getNbCoordsZ() - 0.5;

    m_xSlice->set( xSlice );
    m_ySlice->set( ySlice );
    m_zSlice->set( zSlice );

    // When working with the OpenSceneGraph, always use ref_ptr to store pointers to OSG objects. This allows OpenSceneGraph to manage
    // its resources automatically.

    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    WColor color = m_aColor->get( true );

    if( ( ( *m_dataSet ).getValueSet()->order() == 1 ) && ( ( *m_dataSet ).getValueSet()->dimension() == 3 ) )
    {
        if( m_showOnAxial->get( true ) )
        {
            for( int x = 0; x < nbX; ++x )
            {
                for( int y = 0; y < nbY; ++y )
                {
                    float vecCompX = vals->getScalar( ( x + y * nbX + static_cast< int >( zSlice ) * nbX * nbY ) * 3 ) / 2.;
                    float vecCompY = vals->getScalar( ( x + y * nbX + static_cast< int >( zSlice ) * nbX * nbY ) * 3 + 1 ) / 2.;
                    float vecCompZ = vals->getScalar( ( x + y * nbX + static_cast< int >( zSlice ) * nbX * nbY ) * 3 + 2 ) / 2.;

                    if( !m_projectOnSlice->get( true ) )
                    {
                        vertices->push_back( osg::Vec3( x - vecCompX, y - vecCompY, zSlice - vecCompZ ) );
                        vertices->push_back( osg::Vec3( x + vecCompX, y + vecCompY, zSlice + vecCompZ ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( color );
                            colors->push_back( color );
                        }
                    }
                    else
                    {
                        vertices->push_back( osg::Vec3( x - vecCompX, y - vecCompY, zSlice - 0.01f ) );
                        vertices->push_back( osg::Vec3( x + vecCompX, y + vecCompY, zSlice - 0.01f ) );
                        vertices->push_back( osg::Vec3( x - vecCompX, y - vecCompY, zSlice + 0.01f ) );
                        vertices->push_back( osg::Vec3( x + vecCompX, y + vecCompY, zSlice + 0.01f ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( color );
                            colors->push_back( color );
                            colors->push_back( color );
                            colors->push_back( color );
                        }
                    }
                }
            }
        }

        ++*progress;

        if( m_showOnCoronal->get( true ) )
        {
            for( int x = 0; x < nbX; ++x )
            {
                for( int z = 0; z < nbZ; ++z )
                {
                    float vecCompX = vals->getScalar( ( x + static_cast< int >( ySlice ) * nbX + z * nbX * nbY ) * 3 ) / 2.;
                    float vecCompY = vals->getScalar( ( x + static_cast< int >( ySlice ) * nbX + z * nbX * nbY ) * 3 + 1 ) / 2.;
                    float vecCompZ = vals->getScalar( ( x + static_cast< int >( ySlice ) * nbX + z * nbX * nbY ) * 3 + 2 ) / 2.;

                    if( !m_projectOnSlice->get( true ) )
                    {
                        vertices->push_back( osg::Vec3( x - vecCompX, ySlice - vecCompY, z - vecCompZ ) );
                        vertices->push_back( osg::Vec3( x + vecCompX, ySlice + vecCompY, z + vecCompZ ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( color );
                            colors->push_back( color );
                        }
                    }
                    else
                    {
                        vertices->push_back( osg::Vec3( x - vecCompX, ySlice - 0.01f, z - vecCompZ ) );
                        vertices->push_back( osg::Vec3( x + vecCompX, ySlice - 0.01f, z + vecCompZ ) );
                        vertices->push_back( osg::Vec3( x - vecCompX, ySlice + 0.01f, z - vecCompZ ) );
                        vertices->push_back( osg::Vec3( x + vecCompX, ySlice + 0.01f, z + vecCompZ ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( color );
                            colors->push_back( color );
                            colors->push_back( color );
                            colors->push_back( color );
                        }
                    }
                }
            }
        }

        ++*progress;

        if( m_showOnSagittal->get( true ) )
        {
            for( int y = 0; y < nbY; ++y )
            {
                for( int z = 0; z < nbZ; ++z )
                {
                    float vecCompX = vals->getScalar( ( static_cast< int >( xSlice ) + y * nbX + z * nbX * nbY ) * 3 ) / 2.;
                    float vecCompY = vals->getScalar( ( static_cast< int >( xSlice ) + y * nbX + z * nbX * nbY ) * 3 + 1 ) / 2.;
                    float vecCompZ = vals->getScalar( ( static_cast< int >( xSlice ) + y * nbX + z * nbX * nbY ) * 3 + 2 ) / 2.;

                    if( !m_projectOnSlice->get( true ) )
                    {
                        vertices->push_back( osg::Vec3( xSlice + vecCompX, y + vecCompY, z + vecCompZ ) );
                        vertices->push_back( osg::Vec3( xSlice - vecCompX, y - vecCompY, z - vecCompZ ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( color );
                            colors->push_back( color );
                        }
                    }
                    else
                    {
                        vertices->push_back( osg::Vec3( xSlice - 0.01f, y + vecCompY, z + vecCompZ ) );
                        vertices->push_back( osg::Vec3( xSlice - 0.01f, y - vecCompY, z - vecCompZ ) );
                        vertices->push_back( osg::Vec3( xSlice + 0.01f, y + vecCompY, z + vecCompZ ) );
                        vertices->push_back( osg::Vec3( xSlice + 0.01f, y - vecCompY, z - vecCompZ ) );
                        if( m_coloringMode->get( true ) )
                        {
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                            colors->push_back( osg::Vec4( fabs( vecCompX ), fabs( vecCompY ), fabs( vecCompZ ), 1.0 ) );
                        }
                        else
                        {
                            colors->push_back( color );
                            colors->push_back( color );
                            colors->push_back( color );
                            colors->push_back( color );
                        }
                    }
                }
            }
        }

        ++*progress;

        // Transform the vertices according to the matrix given in the grid.
        transformVerts( vertices );

        for( size_t i = 0; i < vertices->size(); ++i )
        {
            osg::DrawElementsUInt* line = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );
            line->push_back( i );
            line->push_back( i + 1 );
            ++i;
            geometry->addPrimitiveSet( line );
        }
    }

    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

    progress->finish();

    return geometry;
}


#endif  // WMVECTORPLOT_H
