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
#include <vector>
#include <list>

#include <boost/shared_ptr.hpp>

#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture3D>

#include "boost/smart_ptr.hpp"

#include "WNavigationSliceModule.h"
#include "../../kernel/WKernel.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleConnector.h"
#include "../../kernel/WModuleInputData.hpp"

#include "../../dataHandler/WDataSet.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WValueSet.hpp"

#include "../../graphicsEngine/WShader.h"

WNavigationSliceModule::WNavigationSliceModule():
    WModule(),
    m_properties()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.

    // initialize members
    std::string shaderPath = WKernel::getRunningKernel()->getGraphicsEngine()->getShaderPath();
    m_shader = boost::shared_ptr< WShader > ( new WShader( "slice", shaderPath ) );

    m_properties.addBool( "textureAssigned", false );
    m_properties.addInt( "axialPos", 80 );
    m_properties.addInt( "coronalPos", 100 );
    m_properties.addInt( "sagittalPos", 80 );

    m_properties.addInt( "maxAxial", 160 );
    m_properties.addInt( "maxCoronal", 200 );
    m_properties.addInt( "maxSagittal", 160 );

    m_properties.addBool( "showAxial", true );
    m_properties.addBool( "showCoronal", true );
    m_properties.addBool( "showSagittal", true );
}

WNavigationSliceModule::~WNavigationSliceModule()
{
    // cleanup
    removeConnectors();
}

const std::string WNavigationSliceModule::getName() const
{
    return "Navigation Slice Module";
}

const std::string WNavigationSliceModule::getDescription() const
{
    return "This module shows 3 orthogonal navigation slices.";
}

void WNavigationSliceModule::connectors()
{
    // initialize connectors
    // XXX to add a new connector and to offer it, these simple steps need to be done
    // initialize it first
    m_Input= boost::shared_ptr<WModuleInputData<std::list<boost::shared_ptr<WDataSet> > > >(
            new WModuleInputData<std::list<boost::shared_ptr<WDataSet> > >( shared_from_this(),
                "in1", "List of datasets to show on the slices." )
    );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_Input );

    // call WModules initialization
    WModule::connectors();
}

void WNavigationSliceModule::notifyDataChange( boost::shared_ptr<WModuleConnector> input,
                                               boost::shared_ptr<WModuleConnector> output )
{
    WModule::notifyDataChange( input, output );

    // in this case input==m_Input
}

void WNavigationSliceModule::threadMain()
{
    createSlices();

    // Since the modules run in a separate thread: such loops are possible
    while ( !m_FinishRequested )
    {
        if ( WKernel::getRunningKernel()->getDataHandler()->getNumberOfSubjects() > 0 )
        {
            if ( WKernel::getRunningKernel()->getDataHandler()->getSubject(0)->getNumberOfDataSets() > 0 )
            {
                if ( !m_properties.getValue< bool >( "textureAssigned" ) )
                {
                    boost::shared_ptr< WDataSetSingle > ds = boost::shared_dynamic_cast< WDataSetSingle >(
                            WKernel::getRunningKernel()->getDataHandler()->getSubject( 0 )->getDataSet( 0 ) );
                    boost::shared_ptr< WValueSet< int8_t > > vs = boost::shared_dynamic_cast< WValueSet<
                            int8_t > >( ds->getValueSet() );
                    int8_t* source = const_cast< int8_t* > ( vs->rawData() );

                    osg::ref_ptr< osg::Image > ima = new osg::Image;
                    ima->allocateImage( 160, 200, 160, GL_LUMINANCE, GL_UNSIGNED_BYTE );

                    unsigned char* data = ima->data();

                    for ( unsigned int i = 0; i < 160* 200* 160 ; ++i )
                    {
                        data[i] = source[i];
                    }
                    osg::Texture3D* texture3D = new osg::Texture3D;
                    texture3D->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
                    texture3D->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
                    texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::REPEAT );
                    texture3D->setImage( ima );
                    texture3D->setResizeNonPowerOfTwoHint( false );

                    osg::StateSet* sliceState = m_sliceNode->getOrCreateStateSet();

                    sliceState->setTextureAttributeAndModes( 0, texture3D, osg::StateAttribute::ON );

                    m_properties.setValue( "textureAssigned", true );

                    WKernel::getRunningKernel()->getGui()->addDatasetToBrowser( ds->getFileName(), 0 );
                }
            }
        }
        // do fancy stuff
        sleep( 1 );
    }

    // clean up stuff
}

void WNavigationSliceModule::createSlices()
{
    float axialPos = ( float )( m_properties.getValue< int >( "axialPos" ) );
    float coronalPos = ( float )( m_properties.getValue< int >( "coronalPos" ) );
    float sagittalPos = ( float )( m_properties.getValue< int >( "sagittalPos" ) );

    float maxAxial = ( float )( m_properties.getValue<int>( "maxAxial") );
    float maxCoronal = ( float )( m_properties.getValue<int>( "maxCoronal") );
    float maxSagittal = ( float )( m_properties.getValue<int>( "maxSagittal") );

    float texAxial = axialPos / maxAxial;
    float texCoronal = coronalPos / maxCoronal;
    float texSagittal = sagittalPos / maxSagittal;

    m_sliceNode = new osg::Geode();

    osg::Geometry* sliceGeometry = new osg::Geometry();

    m_sliceNode->addDrawable( sliceGeometry );

    osg::Vec3Array* sliceVertices = new osg::Vec3Array;
    sliceVertices->push_back( osg::Vec3( 0, coronalPos, 0 ) );
    sliceVertices->push_back( osg::Vec3( 0, coronalPos, maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( maxAxial, coronalPos, maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( maxAxial, coronalPos, 0 ) );

    sliceVertices->push_back( osg::Vec3( axialPos, 0, 0 ) );
    sliceVertices->push_back( osg::Vec3( axialPos, 0, maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( axialPos, maxCoronal, maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( axialPos, maxCoronal, 0 ) );

    sliceVertices->push_back( osg::Vec3( 0, 0, sagittalPos ) );
    sliceVertices->push_back( osg::Vec3( 0, maxCoronal, sagittalPos ) );
    sliceVertices->push_back( osg::Vec3( maxAxial, maxCoronal, sagittalPos ) );
    sliceVertices->push_back( osg::Vec3( maxAxial, 0, sagittalPos ) );

    sliceGeometry->setVertexArray( sliceVertices );

    osg::Vec3Array* texCoords = new osg::Vec3Array;
    texCoords->push_back( osg::Vec3( 0.0, texCoronal, 0.0 ) );
    texCoords->push_back( osg::Vec3( 0.0, texCoronal, 1.0 ) );
    texCoords->push_back( osg::Vec3( 1.0, texCoronal, 1.0 ) );
    texCoords->push_back( osg::Vec3( 1.0, texCoronal, 0.0 ) );

    texCoords->push_back( osg::Vec3( texAxial, 0.0, 0.0 ) );
    texCoords->push_back( osg::Vec3( texAxial, 0.0, 1.0 ) );
    texCoords->push_back( osg::Vec3( texAxial, 1.0, 1.0 ) );
    texCoords->push_back( osg::Vec3( texAxial, 1.0, 0.0 ) );

    texCoords->push_back( osg::Vec3( 0.0, 0.0, texSagittal ) );
    texCoords->push_back( osg::Vec3( 0.0, 1.0, texSagittal ) );
    texCoords->push_back( osg::Vec3( 1.0, 1.0, texSagittal ) );
    texCoords->push_back( osg::Vec3( 1.0, 0.0, texSagittal ) );

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

    osg::StateSet* sliceState = m_sliceNode->getOrCreateStateSet();

    sliceState->setAttributeAndModes( m_shader->getProgramObject(), osg::StateAttribute::ON );
}

void WNavigationSliceModule::updateSlices()
{
    float axialPos = ( float )( m_properties.getValue< int >( "axialPos" ) );
    float coronalPos = ( float )( m_properties.getValue< int >( "coronalPos" ) );
    float sagittalPos = ( float )( m_properties.getValue< int >( "sagittalPos" ) );

    float maxAxial = ( float )( m_properties.getValue<int>( "maxAxial") );
    float maxCoronal = ( float )( m_properties.getValue<int>( "maxCoronal") );
    float maxSagittal = ( float )( m_properties.getValue<int>( "maxSagittal") );

    float texAxial = axialPos / maxAxial;
    float texCoronal = coronalPos / maxCoronal;
    float texSagittal = sagittalPos / maxSagittal;

    osg::Geometry* sliceGeometry = new osg::Geometry();

    osg::Vec3Array* sliceVertices = new osg::Vec3Array;
    sliceVertices->push_back( osg::Vec3( 0, coronalPos, 0 ) );
    sliceVertices->push_back( osg::Vec3( 0, coronalPos, maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( maxAxial, coronalPos, maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( maxAxial, coronalPos, 0 ) );

    sliceVertices->push_back( osg::Vec3( axialPos, 0, 0 ) );
    sliceVertices->push_back( osg::Vec3( axialPos, 0, maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( axialPos, maxCoronal, maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( axialPos, maxCoronal, 0 ) );

    sliceVertices->push_back( osg::Vec3( 0, 0, sagittalPos ) );
    sliceVertices->push_back( osg::Vec3( 0, maxCoronal, sagittalPos ) );
    sliceVertices->push_back( osg::Vec3( maxAxial, maxCoronal, sagittalPos ) );
    sliceVertices->push_back( osg::Vec3( maxAxial, 0, sagittalPos ) );

    sliceGeometry->setVertexArray( sliceVertices );

    osg::Vec3Array* texCoords = new osg::Vec3Array;
    texCoords->push_back( osg::Vec3( 0.0, texCoronal, 0.0 ) );
    texCoords->push_back( osg::Vec3( 0.0, texCoronal, 1.0 ) );
    texCoords->push_back( osg::Vec3( 1.0, texCoronal, 1.0 ) );
    texCoords->push_back( osg::Vec3( 1.0, texCoronal, 0.0 ) );

    texCoords->push_back( osg::Vec3( texAxial, 0.0, 0.0 ) );
    texCoords->push_back( osg::Vec3( texAxial, 0.0, 1.0 ) );
    texCoords->push_back( osg::Vec3( texAxial, 1.0, 1.0 ) );
    texCoords->push_back( osg::Vec3( texAxial, 1.0, 0.0 ) );

    texCoords->push_back( osg::Vec3( 0.0, 0.0, texSagittal ) );
    texCoords->push_back( osg::Vec3( 0.0, 1.0, texSagittal ) );
    texCoords->push_back( osg::Vec3( 1.0, 1.0, texSagittal ) );
    texCoords->push_back( osg::Vec3( 1.0, 0.0, texSagittal ) );

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

    if ( m_properties.getValue<bool>( "showAxial" ) )
    {
        sliceGeometry->addPrimitiveSet( slice2 );
    }

    if ( m_properties.getValue<bool>( "showCoronal" ) )
    {
        sliceGeometry->addPrimitiveSet( slice0 );
    }

    if ( m_properties.getValue<bool>( "showSagittal" ) )
    {
        sliceGeometry->addPrimitiveSet( slice1 );
    }

    osg::Drawable* old = m_sliceNode->getDrawable( 0 );

    m_sliceNode->replaceDrawable( old, sliceGeometry );

    osg::StateSet* sliceState = m_sliceNode->getOrCreateStateSet();
    sliceState->setAttributeAndModes( m_shader->getProgramObject(), osg::StateAttribute::ON );
}

void WNavigationSliceModule::connectToGui()
{
    WKernel::getRunningKernel()->getGui()->getAxialSliderSignal()->connect(
            boost::bind( &WNavigationSliceModule::sliderAxialMoved, this, _1 ) );
    WKernel::getRunningKernel()->getGui()->getCoronalSliderSignal()->connect(
            boost::bind( &WNavigationSliceModule::sliderCoronalMoved, this, _1 ) );
    WKernel::getRunningKernel()->getGui()->getSagittalSliderSignal()->connect(
            boost::bind( &WNavigationSliceModule::sliderSagittalMoved, this, _1 ) );

    WKernel::getRunningKernel()->getGui()->getAxialButtonSignal()->connect(
            boost::bind( &WNavigationSliceModule::buttonAxialChanged, this, _1 ) );
    WKernel::getRunningKernel()->getGui()->getCoronalButtonSignal()->connect(
            boost::bind( &WNavigationSliceModule::buttonCoronalChanged, this, _1 ) );
    WKernel::getRunningKernel()->getGui()->getSagittalButtonSignal()->connect(
            boost::bind( &WNavigationSliceModule::buttonSagittalChanged, this, _1 ) );
}

void WNavigationSliceModule::sliderAxialMoved( int value )
{
    m_properties.setValue( "axialPos", value );
    updateSlices();
}

void WNavigationSliceModule::sliderCoronalMoved( int value )
{
    m_properties.setValue( "coronalPos", value );
    updateSlices();
}

void WNavigationSliceModule::sliderSagittalMoved( int value )
{
    m_properties.setValue( "sagittalPos", value );
    updateSlices();
}

void WNavigationSliceModule::buttonAxialChanged( bool check )
{
    m_properties.setValue( "showAxial", check );
    updateSlices();
}

void WNavigationSliceModule::buttonCoronalChanged( bool check )
{
    m_properties.setValue( "showCoronal", check );
    updateSlices();
}

void WNavigationSliceModule::buttonSagittalChanged( bool check )
{
    m_properties.setValue( "showSagittal", check );
    updateSlices();
}

