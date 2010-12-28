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

#include <osg/Array>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LineWidth>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osg/Vec3>

#include "../common/math/WPosition.h"
#include "../common/WPathHelper.h"
#include "WGESubdividedPlane.h"
#include "WGEGeodeUtils.h"
#include "WGEGeometryUtils.h"
#include "WGEUtils.h"


osg::ref_ptr< osg::Geode > wge::generateBoundingBoxGeode( const wmath::WPosition& pos1, const wmath::WPosition& pos2, const WColor& color )
{
    WAssert( pos1[0] <= pos2[0] && pos1[1] <= pos2[1] && pos1[2] <= pos2[2], "pos1 does not seem to be the frontLowerLeft corner of the BB!" );
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry > geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    vertices->push_back( osg::Vec3( pos1[0], pos1[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos1[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos2[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos2[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos1[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos1[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos1[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos2[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos2[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos1[1], pos2[2] ) );

    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, vertices->size() ) );

    vertices->push_back( osg::Vec3( pos1[0], pos2[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos1[0], pos2[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos2[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos2[1], pos2[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos1[1], pos1[2] ) );
    vertices->push_back( osg::Vec3( pos2[0], pos1[1], pos2[2] ) );

    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, vertices->size() - 6, 6 ) );

    geometry->setVertexArray( vertices );
    colors->push_back( wge::osgColor( color ) );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry );

    // disable light for this geode as lines can't be lit properly
    osg::StateSet* state = geode->getOrCreateStateSet();
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    return geode;
}

osg::ref_ptr< osg::Geometry > wge::createUnitCube( const WColor& color )
{
    // create the unit cube manually as the ShapeDrawable and osg::Box does not provide 3D texture coordinates
    osg::ref_ptr< osg::Geometry > cube = new osg::Geometry();
    osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec3Array > normals = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec4Array > colors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array );

    // front face
    vertices->push_back( osg::Vec3( 0.0, 0.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 1.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );
    normals->push_back( osg::Vec3( 0.0, 0.0, -1.0 ) );

    // back face
    vertices->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 0.0, 1.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 1.0, 1.0 ) );
    vertices->push_back( osg::Vec3( 0.0, 1.0, 1.0 ) );
    normals->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );

    // left
    vertices->push_back( osg::Vec3( 0.0, 0.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 0.0, 1.0, 1.0 ) );
    vertices->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );
    normals->push_back( osg::Vec3( -1.0, 0.0, 0.0 ) );

    // right
    vertices->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 1.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 1.0, 1.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 0.0, 1.0 ) );
    normals->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );

    // bottom
    vertices->push_back( osg::Vec3( 0.0, 0.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 0.0, 1.0 ) );
    vertices->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );
    normals->push_back( osg::Vec3( 0.0, -1.0, 0.0 ) );

    // top
    vertices->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 1.0, 0.0 ) );
    vertices->push_back( osg::Vec3( 1.0, 1.0, 1.0 ) );
    vertices->push_back( osg::Vec3( 0.0, 1.0, 1.0 ) );
    normals->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );

    // set it up and set arrays
    cube->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, vertices->size() ) );
    cube->setVertexArray( vertices );

    // set 3D texture coordinates here.
    cube->setTexCoordArray( 0, vertices );

    // set normals
    cube->setNormalArray( normals );
    cube->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );

    // finally, the colors
    colors->push_back( wge::osgColor( color ) );
    cube->setColorArray( colors );
    cube->setColorBinding( osg::Geometry::BIND_OVERALL );

    return cube;
}

osg::ref_ptr< osg::Node > wge::generateSolidBoundingBoxNode( const wmath::WPosition& pos1, const wmath::WPosition& pos2, const WColor& color,
        bool threeDTexCoords )
{
    WAssert( pos1[0] <= pos2[0] && pos1[1] <= pos2[1] && pos1[2] <= pos2[2], "pos1 doesn't seem to be the frontLowerLeft corner of the BB!" );

    // create a uni cube
    osg::ref_ptr< osg::Geode > cube = new osg::Geode();
    cube->setName( "Solid Bounding Box" );
    if ( threeDTexCoords )
    {
        cube->addDrawable( createUnitCube( color ) );
    }
    else
    {
        osg::ref_ptr< osg::ShapeDrawable > cubeDrawable = new osg::ShapeDrawable( new osg::Box( osg::Vec3( 0.5, 0.5, 0.5 ), 1.0 ) );
        cubeDrawable->setColor( wge::osgColor( color ) );
        cube->addDrawable( cubeDrawable );
    }

    // transform the cube to match the bbox
    osg::Matrixd transformM;
    osg::Matrixd scaleM;
    transformM.makeTranslate( pos1 );
    scaleM.makeScale( pos2 - pos1 );

    // apply transformation to bbox
    osg::ref_ptr< osg::MatrixTransform > transform = new osg::MatrixTransform();
    transform->setMatrix( scaleM * transformM );
    transform->addChild( cube );

    // we do not need light
    osg::StateSet* state = cube->getOrCreateStateSet();
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    return transform;
}

osg::ref_ptr< osg::Geometry > wge::convertToOsgGeometry( WTriangleMesh* mesh, bool includeNormals )
{
    osg::ref_ptr< osg::Vec3Array > vertices = mesh->getVertexArray();

    osg::DrawElementsUInt* triangles = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES );
    triangles->reserve( 3 * mesh->triangleSize() );
    for( size_t triangleID = 0; triangleID < mesh->triangleSize(); ++triangleID )
    {
        triangles->push_back( mesh->getTriVertId0( triangleID ) );
        triangles->push_back( mesh->getTriVertId1( triangleID ) );
        triangles->push_back( mesh->getTriVertId2( triangleID ) );
    }

    osg::ref_ptr< osg::Geometry> geometry( new osg::Geometry );
    geometry->setVertexArray( vertices );
    geometry->addPrimitiveSet( triangles );

    if( includeNormals )
    {
        geometry->setNormalArray( mesh->getVertexNormalArray( true ) );
        geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
    }

    return geometry;
}

osg::ref_ptr< osg::Geode > wge::generateLineStripGeode( const wmath::WLine& line, const float thickness, const WColor& color )
{
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors   = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry >  geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    for( size_t i = 1; i < line.size(); ++i )
    {
        vertices->push_back( osg::Vec3( line[i-1][0], line[i-1][1], line[i-1][2] ) );
        colors->push_back( wge::osgColor( wge::getRGBAColorFromDirection( line[i-1], line[i] ) ) );
    }
    vertices->push_back( osg::Vec3( line.back()[0], line.back()[1], line.back()[2] ) );
    colors->push_back( colors->back() );

    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, line.size() ) );
    geometry->setVertexArray( vertices );

    if( color != WColor( 0, 0, 0, 0 ) )
    {
        colors->clear();
        colors->push_back( wge::osgColor( color ) );
        geometry->setColorArray( colors );
        geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    }
    else
    {
        geometry->setColorArray( colors );
        geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    }

    // line width
    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth( thickness );
    stateset->setAttributeAndModes( linewidth, osg::StateAttribute::ON );
    stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    geometry->setStateSet( stateset );

    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry );
    return geode;
}

osg::ref_ptr< osg::PositionAttitudeTransform > wge::addLabel( osg::Vec3 position, std::string text )
{
    osg::ref_ptr< osgText::Text > label = osg::ref_ptr< osgText::Text >( new osgText::Text() );
    osg::ref_ptr< osg::Geode > labelGeode = osg::ref_ptr< osg::Geode >( new osg::Geode() );

    labelGeode->addDrawable( label );

    // setup font
    label->setFont( WPathHelper::getAllFonts().Default.file_string() );
    label->setBackdropType( osgText::Text::OUTLINE );
    label->setCharacterSize( 6 );

    label->setText( text );
    label->setAxisAlignment( osgText::Text::SCREEN );
    label->setDrawMode( osgText::Text::TEXT );
    label->setAlignment( osgText::Text::CENTER_TOP );
    label->setPosition( osg::Vec3( 0.0, 0.0, 0.0 ) );
    label->setColor( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

    osg::ref_ptr< osg::PositionAttitudeTransform > labelXform =
        osg::ref_ptr< osg::PositionAttitudeTransform >( new osg::PositionAttitudeTransform() );
    labelXform->setPosition( position );

    labelXform->addChild( labelGeode );

    return labelXform;
}

osg::ref_ptr< osg::PositionAttitudeTransform > wge::vector2label( osg::Vec3 position )
{
    std::string label = "(" + boost::lexical_cast< std::string >( position[0] ) + "," +
    boost::lexical_cast< std::string >( position[1] ) + "," + boost::lexical_cast< std::string >( position[2] ) + ")";
    return ( addLabel( position, label ) );
}

osg::ref_ptr< osg::Geode > wge::genFinitePlane( double xSize, double ySize, const WPlane& p, const WColor& color, bool border )
{
    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    ref_ptr< osg::Vec4Array > colors   = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
    ref_ptr< osg::Geometry >  geometry = ref_ptr< osg::Geometry >( new osg::Geometry );

    colors->push_back( wge::osgColor( color ) );

    vertices->push_back( p.getPointInPlane(  xSize,  ySize ) );
    vertices->push_back( p.getPointInPlane( -xSize,  ySize ) );
    vertices->push_back( p.getPointInPlane( -xSize, -ySize ) );
    vertices->push_back( p.getPointInPlane(  xSize, -ySize ) );

    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, 4 ) );
    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    osg::StateSet* stateset = new osg::StateSet;
    stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    geometry->setStateSet( stateset );

    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( geometry );

    if( border )
    {
        vertices->push_back( vertices->front() );
        ref_ptr< osg::Geometry >  borderGeom = ref_ptr< osg::Geometry >( new osg::Geometry );
        borderGeom->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, 4 ) );
        borderGeom->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 3, 2 ) );
        ref_ptr< osg::Vec4Array > colors   = ref_ptr< osg::Vec4Array >( new osg::Vec4Array );
        WColor borderColor = color;
        borderColor.inverse();
        colors->push_back( wge::osgColor( borderColor ) );
        borderGeom->setColorArray( colors );
        borderGeom->setColorBinding( osg::Geometry::BIND_OVERALL );
        borderGeom->setVertexArray( vertices );
        geode->addDrawable( borderGeom );
    }
    return geode;
}

osg::ref_ptr< osg::Geode > wge::genFinitePlane( osg::Vec3 const& base, osg::Vec3 const& a, osg::Vec3 const& b )
{
    // the stuff needed by the OSG to create a geometry instance
    osg::ref_ptr< osg::Vec3Array > vertices = new osg::Vec3Array;
    osg::ref_ptr< osg::Vec3Array > texcoords0 = new osg::Vec3Array;
    osg::ref_ptr< osg::Vec3Array > normals = new osg::Vec3Array;
    osg::ref_ptr< osg::Vec4Array > colors = new osg::Vec4Array;

    osg::Vec3 aPlusB = a + b;

    vertices->push_back( base );
    vertices->push_back( base + a );
    vertices->push_back( base + aPlusB );
    vertices->push_back( base + b );

    osg::Vec3 aCrossB = a ^ b;
    aCrossB.normalize();
    osg::Vec3 aNorm = a;
    aNorm.normalize();
    osg::Vec3 bNorm = b;
    bNorm.normalize();

    normals->push_back( aCrossB );
    colors->push_back( osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
    texcoords0->push_back( osg::Vec3( 0.0, 0.0, 0.0 ) );
    texcoords0->push_back( aNorm );
    texcoords0->push_back( aNorm + bNorm );
    texcoords0->push_back( bNorm );

    // put it all together
    osg::ref_ptr< osg::Geometry > geometry = new osg::Geometry();
    geometry->setVertexArray( vertices );
    geometry->setTexCoordArray( 0, texcoords0 );
    geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
    geometry->setNormalArray( normals );
    geometry->setColorArray( colors );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, 4 ) );

    osg::ref_ptr< osg::Geode > geode = new osg::Geode();
    geode->addDrawable( geometry );
    return geode;
}

osg::ref_ptr< WGESubdividedPlane > wge::genUnitSubdividedPlane( size_t resX, size_t resY, double spacing )
{
    WAssert( resX > 0 && resY > 0, "A Plane with no quad is not supported, use another datatype for that!" );
    double dx = ( resX > 1 ? 1.0 / ( resX - 1 ) : 1.0 );
    double dy = ( resY > 1 ? 1.0 / ( resY - 1 ) : 1.0 );

    size_t numQuads = resX * resY;

    using osg::ref_ptr;
    ref_ptr< osg::Vec3Array > vertices = ref_ptr< osg::Vec3Array >( new osg::Vec3Array( numQuads * 4 ) );
    ref_ptr< osg::Vec3Array > centers = ref_ptr< osg::Vec3Array >( new osg::Vec3Array( numQuads ) );
    ref_ptr< osg::Vec4Array > colors   = ref_ptr< osg::Vec4Array >( new osg::Vec4Array( numQuads ) );

    for( size_t yQuad = 0; yQuad < resY; ++yQuad )
    {
        for( size_t xQuad = 0; xQuad < resX; ++xQuad )
        {
            size_t qIndex = yQuad * resX + xQuad;
            size_t vIndex = qIndex * 4; // since there are 4 corners
            vertices->at( vIndex     ) = osg::Vec3( xQuad * dx + spacing,      yQuad * dy + spacing,      0.0 );
            vertices->at( vIndex + 1 ) = osg::Vec3( xQuad * dx + dx - spacing, yQuad * dy + spacing,      0.0 );
            vertices->at( vIndex + 2 ) = osg::Vec3( xQuad * dx + dx - spacing, yQuad * dy + dy - spacing, 0.0 );
            vertices->at( vIndex + 3 ) = osg::Vec3( xQuad * dx + spacing,      yQuad * dy + dy - spacing, 0.0 );
            centers->at( qIndex ) = osg::Vec3( xQuad * dx + dx / 2.0, yQuad * dy + dy / 2.0, 0.0 );
            colors->at( qIndex ) = osg::Vec4( 0.1 +  static_cast< double >( qIndex ) / numQuads * 0.6,
                                              0.1 +  static_cast< double >( qIndex ) / numQuads * 0.6,
                                              1.0, 1.0 );
        }
    }

    ref_ptr< osg::Geometry >  geometry = ref_ptr< osg::Geometry >( new osg::Geometry );
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, vertices->size() ) );
    geometry->setVertexArray( vertices );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_PER_PRIMITIVE );

    ref_ptr< osg::Vec3Array > normals = ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    normals->push_back( osg::Vec3( 0.0, 0.0, 1.0 ) );
    geometry->setNormalArray( normals );
    geometry->setNormalBinding( osg::Geometry::BIND_OVERALL );
    osg::ref_ptr< WGESubdividedPlane > geode = osg::ref_ptr< WGESubdividedPlane >( new WGESubdividedPlane );
    geode->addDrawable( geometry );
    geode->setCenterArray( centers );

    // we need to disable light, since the order of the vertices may be wrong and with lighting you won't see anything but black surfaces
    osg::StateSet* state = geode->getOrCreateStateSet();
    state->setMode( GL_BLEND, osg::StateAttribute::ON );
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    return geode;
}

