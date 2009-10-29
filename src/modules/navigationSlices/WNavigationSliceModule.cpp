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
#include "../../dataHandler/WGridRegular3D.h"
#include "../data/WDataModule.hpp"

#include "../../graphicsEngine/WShader.h"

WNavigationSliceModule::WNavigationSliceModule():
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.

    // initialize members
    std::string shaderPath = WKernel::getRunningKernel()->getGraphicsEngine()->getShaderPath();
    m_shader = boost::shared_ptr< WShader > ( new WShader( "slice", shaderPath ) );

    properties();
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

void WNavigationSliceModule::properties()
{
    ( m_properties->addBool( "textureChanged", false ) );

    ( m_properties->addInt( "axialPos", 80 ) );
    ( m_properties->addInt( "coronalPos", 100 ) );
    ( m_properties->addInt( "sagittalPos", 80 ) );

    m_properties->addInt( "maxAxial", 160 );
    m_properties->addInt( "maxCoronal", 200 );
    m_properties->addInt( "maxSagittal", 160 );

    ( m_properties->addBool( "showAxial", true ) );
    ( m_properties->addBool( "showCoronal", true ) );
    ( m_properties->addBool( "showSagittal", true ) );
}

void WNavigationSliceModule::notifyDataChange( boost::shared_ptr<WModuleConnector> input,
                                               boost::shared_ptr<WModuleConnector> output )
{
    WModule::notifyDataChange( input, output );

    // in this case input==m_Input
}

void WNavigationSliceModule::threadMain()
{
    createGeometry();

    // Since the modules run in a separate thread: such loops are possible
    while ( !m_FinishRequested )
    {
        // do fancy stuff
        sleep( 1 );
    }

    // clean up stuff
}

void WNavigationSliceModule::createGeometry()
{
    float axialPos = ( float )( m_properties->getValue< int >( "axialPos" ) );
    float coronalPos = ( float )( m_properties->getValue< int >( "coronalPos" ) );
    float sagittalPos = ( float )( m_properties->getValue< int >( "sagittalPos" ) );

    float maxAxial = ( float )( m_properties->getValue<int>( "maxAxial") );
    float maxCoronal = ( float )( m_properties->getValue<int>( "maxCoronal") );
    float maxSagittal = ( float )( m_properties->getValue<int>( "maxSagittal") );

    float texAxial = axialPos / maxAxial;
    float texCoronal = coronalPos / maxCoronal;
    float texSagittal = sagittalPos / maxSagittal;

    m_sliceNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );

    osg::ref_ptr<osg::Geometry> sliceGeometry = osg::ref_ptr<osg::Geometry>( new osg::Geometry() );

    m_sliceNode->addDrawable( sliceGeometry );

    osg::Vec3Array* sliceVertices = new osg::Vec3Array;
    sliceVertices->push_back( osg::Vec3( 0, coronalPos, 0 ) );
    sliceVertices->push_back( osg::Vec3( 0, coronalPos, maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( maxAxial, coronalPos, maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( maxAxial, coronalPos, 0 ) );

    sliceVertices->push_back( osg::Vec3( sagittalPos, 0, 0 ) );
    sliceVertices->push_back( osg::Vec3( sagittalPos, 0, maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( sagittalPos, maxCoronal, maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( sagittalPos, maxCoronal, 0 ) );

    sliceVertices->push_back( osg::Vec3( 0, 0, axialPos ) );
    sliceVertices->push_back( osg::Vec3( 0, maxCoronal, axialPos ) );
    sliceVertices->push_back( osg::Vec3( maxAxial, maxCoronal, axialPos ) );
    sliceVertices->push_back( osg::Vec3( maxAxial, 0, axialPos ) );

    sliceGeometry->setVertexArray( sliceVertices );

    osg::Vec3Array* texCoords = new osg::Vec3Array;
    texCoords->push_back( osg::Vec3( 0.0, texCoronal, 0.0 ) );
    texCoords->push_back( osg::Vec3( 0.0, texCoronal, 1.0 ) );
    texCoords->push_back( osg::Vec3( 1.0, texCoronal, 1.0 ) );
    texCoords->push_back( osg::Vec3( 1.0, texCoronal, 0.0 ) );

    texCoords->push_back( osg::Vec3( texSagittal, 0.0, 0.0 ) );
    texCoords->push_back( osg::Vec3( texSagittal, 0.0, 1.0 ) );
    texCoords->push_back( osg::Vec3( texSagittal, 1.0, 1.0 ) );
    texCoords->push_back( osg::Vec3( texSagittal, 1.0, 0.0 ) );

    texCoords->push_back( osg::Vec3( 0.0, 0.0, texAxial ) );
    texCoords->push_back( osg::Vec3( 0.0, 1.0, texAxial ) );
    texCoords->push_back( osg::Vec3( 1.0, 1.0, texAxial ) );
    texCoords->push_back( osg::Vec3( 1.0, 0.0, texAxial ) );

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

    initUniforms( sliceState );

    sliceState->setAttributeAndModes( m_shader->getProgramObject(), osg::StateAttribute::ON );

    m_sliceNode->setUpdateCallback( new sliceNodeCallback( boost::shared_dynamic_cast<WNavigationSliceModule>( shared_from_this() ) ) );
}

void WNavigationSliceModule::updateGeometry()
{
    boost::shared_lock<boost::shared_mutex> slock;
    slock = boost::shared_lock<boost::shared_mutex>( m_updateLock );

//    std::vector< boost::shared_ptr< WModule > > datasetList = WKernel::getRunningKernel()->getGui()->getDataSetList( 0 );
//    if ( datasetList.size() > 0 )
//    {
//        boost::shared_ptr< WDataSetSingle > ds = boost::shared_dynamic_cast< WDataSetSingle >( datasetList[0] );
//        boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( ds->getGrid() );
//
//        float mx = grid->getNbCoordsX() * grid->getOffsetX();
//        float my = grid->getNbCoordsY() * grid->getOffsetY();
//        float mz = grid->getNbCoordsZ() * grid->getOffsetZ();
//
//        m_properties->setValue( "maxAxial", mx );
//        m_properties->setValue( "maxCoronal", my );
//        m_properties->setValue( "maxSagittal", mz );
//    }

    float axialPos = ( float )( m_properties->getValue< int >( "axialPos" ) );
    float coronalPos = ( float )( m_properties->getValue< int >( "coronalPos" ) );
    float sagittalPos = ( float )( m_properties->getValue< int >( "sagittalPos" ) );

    float maxAxial = ( float )( m_properties->getValue<int>( "maxAxial") );
    float maxCoronal = ( float )( m_properties->getValue<int>( "maxCoronal") );
    float maxSagittal = ( float )( m_properties->getValue<int>( "maxSagittal") );

    float texAxial = axialPos / maxAxial;
    float texCoronal = coronalPos / maxCoronal;
    float texSagittal = sagittalPos / maxSagittal;

    osg::ref_ptr<osg::Geometry> sliceGeometry = osg::ref_ptr<osg::Geometry>( new osg::Geometry() );

    osg::Vec3Array* sliceVertices = new osg::Vec3Array;
    sliceVertices->push_back( osg::Vec3( 0, coronalPos, 0 ) );
    sliceVertices->push_back( osg::Vec3( 0, coronalPos, maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( maxAxial, coronalPos, maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( maxAxial, coronalPos, 0 ) );

    sliceVertices->push_back( osg::Vec3( sagittalPos, 0, 0 ) );
    sliceVertices->push_back( osg::Vec3( sagittalPos, 0, maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( sagittalPos, maxCoronal, maxSagittal ) );
    sliceVertices->push_back( osg::Vec3( sagittalPos, maxCoronal, 0 ) );

    sliceVertices->push_back( osg::Vec3( 0, 0, axialPos ) );
    sliceVertices->push_back( osg::Vec3( 0, maxCoronal, axialPos ) );
    sliceVertices->push_back( osg::Vec3( maxAxial, maxCoronal, axialPos ) );
    sliceVertices->push_back( osg::Vec3( maxAxial, 0, axialPos ) );

    sliceGeometry->setVertexArray( sliceVertices );

    osg::Vec3Array* texCoords = new osg::Vec3Array;
    texCoords->push_back( osg::Vec3( 0.0, texCoronal, 0.0 ) );
    texCoords->push_back( osg::Vec3( 0.0, texCoronal, 1.0 ) );
    texCoords->push_back( osg::Vec3( 1.0, texCoronal, 1.0 ) );
    texCoords->push_back( osg::Vec3( 1.0, texCoronal, 0.0 ) );

    texCoords->push_back( osg::Vec3( texSagittal, 0.0, 0.0 ) );
    texCoords->push_back( osg::Vec3( texSagittal, 0.0, 1.0 ) );
    texCoords->push_back( osg::Vec3( texSagittal, 1.0, 1.0 ) );
    texCoords->push_back( osg::Vec3( texSagittal, 1.0, 0.0 ) );

    texCoords->push_back( osg::Vec3( 0.0, 0.0, texAxial ) );
    texCoords->push_back( osg::Vec3( 0.0, 1.0, texAxial ) );
    texCoords->push_back( osg::Vec3( 1.0, 1.0, texAxial ) );
    texCoords->push_back( osg::Vec3( 1.0, 0.0, texAxial ) );

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

    if ( m_properties->getValue<bool>( "showAxial" ) )
    {
        sliceGeometry->addPrimitiveSet( slice2 );
    }

    if ( m_properties->getValue<bool>( "showCoronal" ) )
    {
        sliceGeometry->addPrimitiveSet( slice0 );
    }

    if ( m_properties->getValue<bool>( "showSagittal" ) )
    {
        sliceGeometry->addPrimitiveSet( slice1 );
    }

    osg::Drawable* old = m_sliceNode->getDrawable( 0 );

    m_sliceNode->replaceDrawable( old, sliceGeometry );

    osg::StateSet* sliceState = m_sliceNode->getOrCreateStateSet();
    sliceState->setAttributeAndModes( m_shader->getProgramObject(), osg::StateAttribute::ON );

    slock.unlock();
}


void WNavigationSliceModule::updateTextures()
{
    boost::shared_lock<boost::shared_mutex> slock;
    slock = boost::shared_lock<boost::shared_mutex>( m_updateLock );

    if ( m_properties->getValue< bool >( "textureChanged" ) && WKernel::getRunningKernel()->getGui()->isInitalized() )
    {
        m_properties->setValue( "textureChanged", false );
        std::vector< boost::shared_ptr< WModule > > datasetList = WKernel::getRunningKernel()->getGui()->getDataSetList( 0 );

        if ( datasetList.size() > 0 )
        {
            for ( int i = 0; i < 10; ++i )
            {
                m_typeUniforms[i]->set( 0 );
            }

            osg::StateSet* sliceState = m_sliceNode->getOrCreateStateSet();
            int c = 0;
            for ( size_t i = 0; i < datasetList.size(); ++i )
            {
                if ( datasetList[i]->getProperties()->getValue<bool>( "active" ) &&
                        boost::shared_dynamic_cast<WDataModule<int> >( datasetList[i] )->getTexture3D() )
                {
                    osg::ref_ptr<osg::Texture3D> texture3D = boost::shared_dynamic_cast<WDataModule<int> >( datasetList[i] )->getTexture3D();

                    sliceState->setTextureAttributeAndModes( c, texture3D, osg::StateAttribute::ON );

                    float t = ( float ) ( datasetList[i]->getProperties()->getValue<int>( "threshold" ) ) / 100.0;
                    float a = ( float ) ( datasetList[i]->getProperties()->getValue<int>( "alpha" ) ) / 100.0;

                    m_typeUniforms[c]->set( boost::shared_dynamic_cast<WDataSetSingle>(
                            boost::shared_dynamic_cast<WDataModule<int> >( datasetList[i] )->getDataSet() )->getValueSet()->getDataType() );
                    m_thresholdUniforms[c]->set( t );
                    m_alphaUniforms[c]->set( a );

                    ++c;
                }
            }

            m_properties->setValue( "textureChanged", false );
        }
    }
    slock.unlock();
}


void WNavigationSliceModule::connectToGui()
{
    WKernel::getRunningKernel()->getGui()->connectProperties( m_properties );
}


void WNavigationSliceModule::initUniforms( osg::StateSet* sliceState )
{
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type0", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type1", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type2", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type3", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type4", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type5", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type6", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type7", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type8", 0 ) ) );
    m_typeUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "type9", 0 ) ) );

    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha0", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha1", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha2", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha3", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha4", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha5", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha6", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha7", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha8", 1.0f ) ) );
    m_alphaUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "alpha9", 1.0f ) ) );

    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold0", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold1", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold2", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold3", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold4", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold5", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold6", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold7", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold8", 0.0f ) ) );
    m_thresholdUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "threshold9", 0.0f ) ) );

    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex0", 0 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex1", 1 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex2", 2 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex3", 3 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex4", 4 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex5", 5 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex6", 6 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex7", 7 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex8", 8 ) ) );
    m_samplerUniforms.push_back( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "tex9", 9 ) ) );

    for ( int i = 0; i < 10; ++i )
    {
        sliceState->addUniform( m_typeUniforms[i] );
        sliceState->addUniform( m_thresholdUniforms[i] );
        sliceState->addUniform( m_alphaUniforms[i] );
        sliceState->addUniform( m_samplerUniforms[i] );
    }
}
