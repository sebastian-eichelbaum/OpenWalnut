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

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>

#include "WMTextureList.h"
#include "../../kernel/WKernel.h"

WMTextureList::WMTextureList():
    WModule()
{
    // initialize members
}

WMTextureList::~WMTextureList()
{
    // cleanup
}

boost::shared_ptr< WModule > WMTextureList::factory() const
{
    return boost::shared_ptr< WModule >( new WMTextureList() );
}

const std::string WMTextureList::getName() const
{
    return "Texture List";
}

const std::string WMTextureList::getDescription() const
{
    return "Module for managing lists of loaded textures.";
}

void WMTextureList::moduleMain()
{
    // TODO(seralph): write me
    // load the sample scene.
    osg::ref_ptr<osg::Geode> sceneDataGeode = new osg::Geode();

    // 20 units into the screen
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Box( osg::Vec3( -6, 5, -20 ), 1.0 ) ) );
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Sphere( osg::Vec3( -3, 5, -20 ), 1.0 ) ) );
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Cone( osg::Vec3(  0, 5, -20 ), 1.0, 1.0 ) ) );
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Cylinder( osg::Vec3(  3, 5, -20 ), 1.0, 1.0 ) ) );
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Capsule( osg::Vec3(  6, 5, -20 ), 1.0, 1.0 ) ) );

    // 25 units into the screen
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Box( osg::Vec3( -6, 0, -25 ), 1.0 ) ) );
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Sphere( osg::Vec3( -3, 0, -25 ), 1.0 ) ) );
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Cone( osg::Vec3(  0, 0, -25 ), 1.0, 1.0 ) ) );
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Cylinder( osg::Vec3(  3, 0, -25 ), 1.0, 1.0 ) ) );
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Capsule( osg::Vec3(  6, 0, -25 ), 1.0, 1.0 ) ) );

    // 30 units into the screen
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Box( osg::Vec3( -6, -5, -30 ), 1.0 ) ) );
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Sphere( osg::Vec3( -3, -5, -30 ), 1.0 ) ) );
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Cone( osg::Vec3(  0, -5, -30 ), 1.0, 1.0 ) ) );
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Cylinder( osg::Vec3(  3, -5, -30 ), 1.0, 1.0 ) ) );
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Capsule( osg::Vec3(  6, -5, -30 ), 1.0, 1.0 ) ) );

    // 35 units into the screen
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Box( osg::Vec3( -6, -10, -35 ), 1.0 ) ) );
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Sphere( osg::Vec3( -3, -10, -35 ), 1.0 ) ) );
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Cone( osg::Vec3(  0, -10, -35 ), 1.0, 1.0 ) ) );
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Cylinder( osg::Vec3(  3, -10, -35 ), 1.0, 1.0 ) ) );
    sceneDataGeode->addDrawable(
            new osg::ShapeDrawable( new osg::Capsule( osg::Vec3(  6, -10, -35 ), 1.0, 1.0 ) ) );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( sceneDataGeode );

    // Since the modules run in a separate thread: such loops are possible
    while ( !m_FinishRequested )
    {
        // do fancy stuff
        sleep( 1 );
    }

    // clean up stuff
}

