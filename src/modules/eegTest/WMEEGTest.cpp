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
#include <osg/Geometry>
#include <osgText/Font>
#include <osgText/Text>

#include "WMEEGTest.h"
#include "../../kernel/WKernel.h"
#include "../../dataHandler/WEEG.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WSubject.h"

WMEEGTest::WMEEGTest():
    WModule()
{
    // initialize members
}

WMEEGTest::~WMEEGTest()
{
    // cleanup
}

boost::shared_ptr< WModule > WMEEGTest::factory() const
{
    return boost::shared_ptr< WModule >( new WMEEGTest() );
}

const std::string WMEEGTest::getName() const
{
    return "Eeg Module";
}

const std::string WMEEGTest::getDescription() const
{
    return "This module is for testing and development";
}

void drawChannel( boost::shared_ptr< const WEEG > eegData, size_t channelId, osg::Geode* sceneDataGeode )
{
    const wmath::WPosition basePos( 0., 0., 0. );
    // create Geometry object to store all the vetices and lines primtive.
    osg::Geometry* linesGeom = new osg::Geometry();

    const size_t segmentId = 0;
    const size_t nbSamples = eegData->getNumberOfSamples( segmentId );
    const double scaleX = 1;
    const double scaleY = 1;
    const double scaleZ = .1;

    osg::Vec3Array* vertices = new osg::Vec3Array( nbSamples );
    osg::Vec3Array::iterator vitr = vertices->begin();

    for( unsigned int i = 0; i < nbSamples; ++i )
    {
        (vitr++)->set( basePos[0] + i * scaleX, basePos[1] + 0 * scaleY, basePos[2] + (*eegData)( 0, channelId, i ) * scaleZ );
    }

    linesGeom->setVertexArray( vertices );

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back( osg::Vec4( 1.0f, 1.0f, 0.0f, 1.0f ) );
    linesGeom->setColorArray( colors );
    linesGeom->setColorBinding( osg::Geometry::BIND_OVERALL );

    linesGeom->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, nbSamples ) );
    sceneDataGeode ->addDrawable( linesGeom );


    wmath::WPosition textPos( basePos[0] - 20 * scaleX, basePos[1], basePos[2] + (*eegData)( 0, channelId, 0 ) * scaleZ );
    osgText::Text* textOne = new osgText::Text();
    textOne->setCharacterSize( 25 );
    textOne->setText( eegData->getChannelLabel( channelId ) );
    textOne->setAxisAlignment( osgText::Text::SCREEN );
    textOne->setPosition( osg::Vec3( textPos[0], textPos[1], textPos[2] ) );
    sceneDataGeode->addDrawable( textOne );
}

void WMEEGTest::threadMain()
{
    // load the sample scene.
    //osg::Geode* sceneDataGeode = new osg::Geode();

    // TODO(all): fix this.
    //std::string fileName = "dataHandler/fixtures/eeg_testData.asc";
    ////    std::string fileName = "/dargb/bv_data/Medical/MPI-CBS/EEG-Test/Alex_Segment_1.edf";
    //std::cout << "Test loading of " << fileName << "." << std::endl;
    //boost::shared_ptr< WDataHandler > dataHandler =
        //boost::shared_ptr< WDataHandler >( new WDataHandler() );
    //WLoaderManager testLoaderManager;
    //testLoaderManager.load( fileName, dataHandler );
    //std::cout << "Number of DS: " << dataHandler->getNumberOfSubjects() << std::endl;
    //sleep( 4 );  // we need this to allow the thread to terminate

    //std::cout << "Number of DS: " << dataHandler->getNumberOfSubjects() << std::endl;
    //boost::shared_ptr< const WEEG > eegData;
    //eegData = boost::shared_dynamic_cast< const WEEG >( dataHandler->getSubject( 0 )->getDataSet( 0 ) );

    //for( unsigned int channelId = 0; channelId < eegData->getNumberOfChannels() ; ++channelId )
    //{
        //std::cout << "Draw channel " << channelId << std::endl;
        //drawChannel( eegData, channelId, sceneDataGeode );
    //}

    //WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( sceneDataGeode );

    //std::cout << "Number of DS: " << dataHandler->getNumberOfSubjects() << std::endl;

    // Since the modules run in a separate thread: such loops are possible
    while ( !m_FinishRequested )
    {
        // do fancy stuff
        sleep( 1 );
    }
    //std::cout << "Number of DS: " << dataHandler->getNumberOfSubjects() << std::endl;

    // clean up stuff
}

