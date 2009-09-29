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

#include "WTestModule.h"
#include "../../kernel/WKernel.h"

WTestModule::WTestModule():
    WModule()
{
    // initialize members
}

WTestModule::~WTestModule()
{
    // cleanup
}

WTestModule::WTestModule( const WTestModule& other )
    : WModule()
{
    *this = other;
}

const std::string WTestModule::getName() const
{
    return "Test Module";
}

const std::string WTestModule::getDescription() const
{
    return "This module is for testing and development";
}

void WTestModule::threadMain()
{
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

