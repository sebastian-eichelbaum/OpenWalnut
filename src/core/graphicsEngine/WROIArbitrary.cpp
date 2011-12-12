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

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <osg/LineWidth>
#include <osg/LightModel>
#include <osg/Geometry>

#include "algorithms/WMarchingLegoAlgorithm.h"

#include "callbacks/WGEFunctorCallback.h"
#include "WGraphicsEngine.h"

#include "WROIArbitrary.h"

WROIArbitrary::WROIArbitrary( size_t nbCoordsX, size_t nbCoordsY, size_t nbCoordsZ,
                              const WMatrix< double >& mat,
                              const std::vector< float >& vals,
                              boost::shared_ptr< WTriangleMesh > triMesh,
                              float threshold,
                              float maxThreshold,
                              WColor color ) :
    WROI(),
    m_nbCoordsVec( 3 ),
    m_matrix( mat ),
    m_vals( vals ),
    m_triMesh( triMesh ),
    m_color( color )
{
    m_nbCoordsVec[0] = nbCoordsX;
    m_nbCoordsVec[1] = nbCoordsY;
    m_nbCoordsVec[2] = nbCoordsZ;

    properties();

    m_threshold->set( threshold );
    m_threshold->setMax( maxThreshold );

    updateGFX();

    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( this );
    addUpdateCallback( new WGEFunctorCallback< osg::Node >( boost::bind( &WROIArbitrary::updateGFX, this ) ) );

    setDirty();
}

WROIArbitrary::WROIArbitrary( size_t nbCoordsX, size_t nbCoordsY, size_t nbCoordsZ,
        const WMatrix< double >& mat,
        const std::vector< float >& vals,
        float maxThreshold,
        WColor color ) :
    WROI(),
    m_nbCoordsVec( 3 ),
    m_matrix( mat ),
    m_vals( vals ),
    m_color( color )
{
    m_nbCoordsVec[0] = nbCoordsX;
    m_nbCoordsVec[1] = nbCoordsY;
    m_nbCoordsVec[2] = nbCoordsZ;

    properties();

    m_threshold->set( 0.01 );
    m_threshold->setMax( maxThreshold );

    updateGFX();

    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( this );
    addUpdateCallback( new WGEFunctorCallback< osg::Node >( boost::bind( &WROIArbitrary::updateGFX, this ) ) );

    setDirty();
}

WROIArbitrary::~WROIArbitrary()
{
//    std::cout << "destructor called" << std::endl;
//    std::cout << "ref count geode: " << m_geode->referenceCount() << std::endl;
//
//    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( m_geode );
}

void WROIArbitrary::properties()
{
    m_threshold = m_properties->addProperty( "Threshold", "description", 0. , boost::bind( &WROIArbitrary::propertyChanged, this ) );
}

void WROIArbitrary::propertyChanged()
{
    setDirty();
}

void WROIArbitrary::setThreshold( double threshold )
{
    m_threshold->set( threshold );
    setDirty();
}

double WROIArbitrary::getThreshold()
{
    return m_threshold->get();
}

std::vector< size_t > WROIArbitrary::getCoordDimensions()
{
    return m_nbCoordsVec;
}

std::vector< double > WROIArbitrary::getCoordOffsets()
{
    std::vector< double > vec( 3 );
    vec[0] = m_matrix( 0, 0 );
    vec[1] = m_matrix( 1, 1 );
    vec[2] = m_matrix( 2, 2 );
    return vec;
}

float WROIArbitrary::getValue( size_t i )
{
    return m_vals[i];
}

void WROIArbitrary::updateGFX()
{
    if( m_dirty->get() )
    {
        WMarchingLegoAlgorithm mlAlgo;
        m_triMesh = mlAlgo.generateSurface( m_nbCoordsVec[0], m_nbCoordsVec[1], m_nbCoordsVec[2],
                                            m_matrix,
                                            &m_vals,
                                            m_threshold->get() );

        osg::Geometry* surfaceGeometry = new osg::Geometry();
        setName( "roi" );

        surfaceGeometry->setVertexArray( m_triMesh->getVertexArray() );

        // ------------------------------------------------
        // normals
        surfaceGeometry->setNormalArray( m_triMesh->getTriangleNormalArray() );
        surfaceGeometry->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );

        // ------------------------------------------------
        // colors
        osg::Vec4Array* colors = new osg::Vec4Array;
        colors->push_back( m_color );
        surfaceGeometry->setColorArray( colors );
        surfaceGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

        osg::DrawElementsUInt* surfaceElement = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );

        std::vector< size_t >tris = m_triMesh->getTriangles();
        surfaceElement->reserve( tris.size() );

        for( unsigned int vertId = 0; vertId < tris.size(); ++vertId )
        {
            surfaceElement->push_back( tris[vertId] );
        }
        surfaceGeometry->addPrimitiveSet( surfaceElement );
        removeDrawables( 0 );
        addDrawable( surfaceGeometry );

        osg::StateSet* state = getOrCreateStateSet();
        osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel();
        lightModel->setTwoSided( true );
        state->setAttributeAndModes( lightModel.get(), osg::StateAttribute::ON );

        state->setMode(  GL_BLEND, osg::StateAttribute::ON );

    //    {
    //        osg::ref_ptr< osg::Material > material = new osg::Material();
    //        material->setDiffuse(   osg::Material::FRONT, osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
    //        material->setSpecular(  osg::Material::FRONT, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
    //        material->setAmbient(   osg::Material::FRONT, osg::Vec4( 0.1, 0.1, 0.1, 1.0 ) );
    //        material->setEmission(  osg::Material::FRONT, osg::Vec4( 0.0, 0.0, 0.0, 1.0 ) );
    //        material->setShininess( osg::Material::FRONT, 25.0 );
    //        state->setAttribute( material );
    //    }

        m_dirty->set( false );
    }
}
