//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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
#include <osg/Geometry>

#include "WNavigationSliceModule.h"
#include "../../kernel/WKernel.h"

WNavigationSliceModule::WNavigationSliceModule():
    WModule()
{
    // initialize members
}

WNavigationSliceModule::~WNavigationSliceModule()
{
    // cleanup
}

WNavigationSliceModule::WNavigationSliceModule( const WNavigationSliceModule& other )
    : WModule()
{
    *this = other;
}

const std::string WNavigationSliceModule::getName() const
{
    return "Navigation Slice Module";
}

const std::string WNavigationSliceModule::getDescription() const
{
    return "This module shows 3 orthogonal navigation slices.";
}

void WNavigationSliceModule::threadMain()
{
    createSlices();

    // Since the modules run in a separate thread: such loops are possible
    while ( !m_FinishRequested )
    {
        // do fancy stuff
        sleep( 1 );
    }

    // clean up stuff
}

void WNavigationSliceModule::createSlices()
{
    osg::Geode* m_sliceNode = new osg::Geode();

    osg::Geometry* sliceGeometry = new osg::Geometry();
    m_sliceNode->addDrawable( sliceGeometry );

    osg::Vec3Array* sliceVertices = new osg::Vec3Array;
    sliceVertices->push_back( osg::Vec3( 0, 50, 0 ) );
    sliceVertices->push_back( osg::Vec3( 0, 50, 100 ) );
    sliceVertices->push_back( osg::Vec3( 100, 50, 100 ) );
    sliceVertices->push_back( osg::Vec3( 100, 50, 0 ) );

    sliceVertices->push_back( osg::Vec3( 50, 0, 0 ) );
    sliceVertices->push_back( osg::Vec3( 50, 0, 100 ) );
    sliceVertices->push_back( osg::Vec3( 50, 100, 100 ) );
    sliceVertices->push_back( osg::Vec3( 50, 100, 0 ) );

    sliceVertices->push_back( osg::Vec3( 0, 0, 50 ) );
    sliceVertices->push_back( osg::Vec3( 0, 100, 50 ) );
    sliceVertices->push_back( osg::Vec3( 100, 100, 50 ) );
    sliceVertices->push_back( osg::Vec3( 100, 0, 50 ) );

    sliceGeometry->setVertexArray( sliceVertices );

    osg::Vec3Array* texCoords = new osg::Vec3Array;
    texCoords->push_back( osg::Vec3( 0.0, 0.5, 0.0 ) );
    texCoords->push_back( osg::Vec3( 0.0, 0.5, 1.0 ) );
    texCoords->push_back( osg::Vec3( 1.0, 0.5, 1.0 ) );
    texCoords->push_back( osg::Vec3( 1.0, 0.5, 0.0 ) );

    texCoords->push_back( osg::Vec3( 0.5, 0.0, 0.0 ) );
    texCoords->push_back( osg::Vec3( 0.5, 0.0, 1.0 ) );
    texCoords->push_back( osg::Vec3( 0.5, 1.0, 1.0 ) );
    texCoords->push_back( osg::Vec3( 0.5, 1.0, 0.0 ) );

    texCoords->push_back( osg::Vec3( 0.0, 0.0, 0.5 ) );
    texCoords->push_back( osg::Vec3( 0.0, 1.0, 0.5 ) );
    texCoords->push_back( osg::Vec3( 1.0, 1.0, 0.5 ) );
    texCoords->push_back( osg::Vec3( 1.0, 0.0, 0.5 ) );

    sliceGeometry->setTexCoordArray( 0, texCoords );

    osg::DrawElementsUInt* slice0 = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    slice0->push_back( 3 );
    slice0->push_back( 2 );
    slice0->push_back( 1 );
    slice0->push_back( 0 );

    osg::DrawElementsUInt* slice1 = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    slice1->push_back( 7 );
    slice1->push_back( 6 );
    slice1->push_back( 5 );
    slice1->push_back( 4 );

    osg::DrawElementsUInt* slice2 = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    slice2->push_back( 11 );
    slice2->push_back( 10 );
    slice2->push_back( 9 );
    slice2->push_back( 8 );

    sliceGeometry->addPrimitiveSet( slice0 );
    sliceGeometry->addPrimitiveSet( slice1 );
    sliceGeometry->addPrimitiveSet( slice2 );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( m_sliceNode );


    // FIXME (schurade)
    // code taken from http://www.linuxquestions.org/questions/programming-9/get-full-path-of-a-command-in-c-117965/
    // move it to somewhere else and execute it on startup and store the app path somewhere central

    int length;
    char appPath[255];

    length = readlink( "/proc/self/exe", appPath, sizeof( appPath ) );

    // Catch some errors
    if ( length < 0 )
    {
        fprintf( stderr, "Error resolving symlink /proc/self/exe.\n" );
        exit( EXIT_FAILURE );
    }
    if ( length >= 255 )
    {
        fprintf( stderr, "Path too long. Truncated.\n" );
        exit( EXIT_FAILURE );
    }

    // I don't know why, but the string this readlink() function
    // returns is appended with a '@'.

    appPath[length] = '\0';

    // strip off the executable name
    while ( appPath[length] != '/' )
    {
        appPath[length] = '\0';
        --length;
    }

    std::string shaderPath( appPath );
    shaderPath += "shaders/";

    std::cout << "Full path is: " << shaderPath << std::endl;

    osg::StateSet* sliceState = m_sliceNode->getOrCreateStateSet();

    osg::Program* sliceProgramObject = new osg::Program;
    osg::Shader* sliceVertexObject = osg::Shader::readShaderFile( osg::Shader::VERTEX, shaderPath + "slice.vs" );
    osg::Shader* sliceFragmentObject = osg::Shader::readShaderFile( osg::Shader::FRAGMENT, shaderPath + "slice.fs" );
    sliceProgramObject->addShader( sliceFragmentObject );
    sliceProgramObject->addShader( sliceVertexObject );

    sliceState->setAttributeAndModes( sliceProgramObject, osg::StateAttribute::ON );
}
