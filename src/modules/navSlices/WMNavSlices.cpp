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
#include <list>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>

#include "../../dataHandler/WDataSet.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../dataHandler/WSubject.h"
#include "../../dataHandler/WValueSet.h"
#include "../../graphicsEngine/WShader.h"
#include "../../kernel/WKernel.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleConnector.h"
#include "../../kernel/WModuleInputData.h"
#include "../data/WMData.h"
#include "WMNavSlices.h"

WMNavSlices::WMNavSlices():
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.

    // initialize members
    std::string shaderPath = WKernel::getRunningKernel()->getGraphicsEngine()->getShaderPath();
    m_shader = boost::shared_ptr< WShader > ( new WShader( "slice", shaderPath ) );
}

WMNavSlices::~WMNavSlices()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMNavSlices::factory() const
{
    return boost::shared_ptr< WModule >( new WMNavSlices() );
}

const std::string WMNavSlices::getName() const
{
    return "Navigation Slice Module";
}

const std::string WMNavSlices::getDescription() const
{
    return "This module shows 3 orthogonal navigation slices.";
}

void WMNavSlices::connectors()
{
    // initialize connectors
    // XXX to add a new connector and to offer it, these simple steps need to be done
    // initialize it first
    m_input= boost::shared_ptr<WModuleInputData< WDataSetSingle > >(
            new WModuleInputData< WDataSetSingle >( shared_from_this(),
                "in1", "List of datasets to show on the slices." )
    );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMNavSlices::properties()
{
    m_properties->addBool( "textureChanged", false, true );
    //( m_properties->addBool( "active", true, true ) )->connect( boost::bind( &WMNavSlices::slotPropertyChanged, this, _1 ) );
    m_properties->addBool( "active", true, true );
    m_properties->addInt( "axialPos", 80 );
    m_properties->addInt( "coronalPos", 100 );
    m_properties->addInt( "sagittalPos", 80 );

    m_properties->addInt( "maxAxial", 160, true );
    m_properties->addInt( "maxCoronal", 200, true );
    m_properties->addInt( "maxSagittal", 160, true );


    m_properties->addBool( "showAxial", true );
    m_properties->addBool( "showCoronal", true );
    m_properties->addBool( "showSagittal", true );
}

void WMNavSlices::notifyDataChange( boost::shared_ptr<WModuleConnector> input,
                                               boost::shared_ptr<WModuleConnector> output )
{
    WModule::notifyDataChange( input, output );

    // in this case input==m_input
}

void WMNavSlices::moduleMain()
{
    // signal ready state
    ready();

    create();

    // Since the modules run in a separate thread: wait
    waitForStop();

    // clean up stuff
    // NOTE: ALLAWAYS remove your osg nodes!
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_rootNode );
}

void WMNavSlices::create()
{
    m_rootNode = osg::ref_ptr<osg::Group>( new osg::Group() );

    m_xSliceNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_xSliceNode->setName( "X-Slice" );
    m_ySliceNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_ySliceNode->setName( "Y-Slice" );
    m_zSliceNode = osg::ref_ptr<osg::Geode>( new osg::Geode() );
    m_zSliceNode->setName( "Z-Slice" );

    m_xSliceNode->addDrawable( createGeometry( 0 ) );
    m_ySliceNode->addDrawable( createGeometry( 1 ) );
    m_zSliceNode->addDrawable( createGeometry( 2 ) );

    m_rootNode->addChild( m_xSliceNode );
    m_rootNode->addChild( m_ySliceNode );
    m_rootNode->addChild( m_zSliceNode );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( m_rootNode );
    osg::StateSet* rootState = m_rootNode->getOrCreateStateSet();
    initUniforms( rootState );
    rootState->setAttributeAndModes( m_shader->getProgramObject(), osg::StateAttribute::ON );

    m_rootNode->setUserData( this );
    m_rootNode->setUpdateCallback( new sliceNodeCallback );
}

osg::ref_ptr<osg::Geometry> WMNavSlices::createGeometry( int slice )
{
    float maxDim = 255.0;

    float xSlice = ( float )( m_properties->getValue< int >( "sagittalPos" ) );
    float ySlice = ( float )( m_properties->getValue< int >( "coronalPos" ) );
    float zSlice = ( float )( m_properties->getValue< int >( "axialPos" ) );

    float xPos = xSlice + 0.5f;
    float yPos = ySlice + 0.5f;
    float zPos = zSlice + 0.5f;

    osg::ref_ptr<osg::Geometry> sliceGeometry = osg::ref_ptr<osg::Geometry>( new osg::Geometry() );

    osg::Vec3Array* sliceVertices = new osg::Vec3Array;

    std::vector< boost::shared_ptr< WDataSet > > dsl = WKernel::getRunningKernel()->getGui()->getDataSetList( 0, true );

    if ( dsl.size() > 0 )
    {
        switch ( slice )
        {
            case 0:
            {
                sliceVertices->push_back( osg::Vec3( 0,      yPos, 0      ) );
                sliceVertices->push_back( osg::Vec3( 0,      yPos, maxDim ) );
                sliceVertices->push_back( osg::Vec3( maxDim, yPos, maxDim ) );
                sliceVertices->push_back( osg::Vec3( maxDim, yPos, 0      ) );
                sliceGeometry->setVertexArray( sliceVertices );

                int c = 0;
                for ( size_t i = 0; i < dsl.size(); ++i )
                {
                    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >(
                            boost::shared_dynamic_cast< WDataSetSingle >( dsl[i] )->getGrid() );

                    float maxX = ( float )( grid->getNbCoordsX() );
                    float maxY = ( float )( grid->getNbCoordsY() );
                    float maxZ = ( float )( grid->getNbCoordsZ() );

                    //float texX = xSlice / maxX;
                    float texY = ySlice / maxY;
                    //float texZ = zSlice / maxZ;

                    float texXOff = 255.0 / maxX;
                    //float texYOff = 255.0 / maxY;
                    float texZOff = 255.0 / maxZ;

                    osg::Vec3Array* texCoords = new osg::Vec3Array;
                    texCoords->push_back( grid->transformTexCoord( osg::Vec3( 0.0,     texY, 0.0     ) ) );
                    texCoords->push_back( grid->transformTexCoord( osg::Vec3( 0.0,     texY, texZOff ) ) );
                    texCoords->push_back( grid->transformTexCoord( osg::Vec3( texXOff, texY, texZOff ) ) );
                    texCoords->push_back( grid->transformTexCoord( osg::Vec3( texXOff, texY, 0.0     ) ) );
                    sliceGeometry->setTexCoordArray( c, texCoords );
                    ++c;
                }
                break;
            }
            case 1:
            {
                sliceVertices->push_back( osg::Vec3( xPos, 0,      0      ) );
                sliceVertices->push_back( osg::Vec3( xPos, 0,      maxDim ) );
                sliceVertices->push_back( osg::Vec3( xPos, maxDim, maxDim ) );
                sliceVertices->push_back( osg::Vec3( xPos, maxDim, 0      ) );
                sliceGeometry->setVertexArray( sliceVertices );

                int c = 0;
                for ( size_t i = 0; i < dsl.size(); ++i )
                {
                    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >(
                            boost::shared_dynamic_cast< WDataSetSingle >( dsl[i] )->getGrid() );

                    float maxX = ( float )( grid->getNbCoordsX() );
                    float maxY = ( float )( grid->getNbCoordsY() );
                    float maxZ = ( float )( grid->getNbCoordsZ() );

                    float texX = xSlice / maxX;
                    //float texY = ySlice / maxY;
                    //float texZ = zSlice / maxZ;

                    //float texXOff = 255.0 / maxX;
                    float texYOff = 255.0 / maxY;
                    float texZOff = 255.0 / maxZ;

                    osg::Vec3Array* texCoords = new osg::Vec3Array;
                    texCoords->push_back( grid->transformTexCoord( osg::Vec3( texX, 0.0,     0.0     ) ) );
                    texCoords->push_back( grid->transformTexCoord( osg::Vec3( texX, 0.0,     texZOff ) ) );
                    texCoords->push_back( grid->transformTexCoord( osg::Vec3( texX, texYOff, texZOff ) ) );
                    texCoords->push_back( grid->transformTexCoord( osg::Vec3( texX, texYOff, 0.0     ) ) );
                    sliceGeometry->setTexCoordArray( c, texCoords );
                    ++c;
                }
                break;
            }
            case 2:
            {
                sliceVertices->push_back( osg::Vec3( 0,      0,      zPos ) );
                sliceVertices->push_back( osg::Vec3( 0,      maxDim, zPos ) );
                sliceVertices->push_back( osg::Vec3( maxDim, maxDim, zPos ) );
                sliceVertices->push_back( osg::Vec3( maxDim, 0,      zPos ) );
                sliceGeometry->setVertexArray( sliceVertices );
                int c = 0;
                for ( size_t i = 0; i < dsl.size(); ++i )
                {
                    boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >(
                            boost::shared_dynamic_cast< WDataSetSingle >( dsl[i] )->getGrid() );

                    float maxX = ( float )( grid->getNbCoordsX() );
                    float maxY = ( float )( grid->getNbCoordsY() );
                    float maxZ = ( float )( grid->getNbCoordsZ() );

                    //float texX = xSlice / maxX;
                    //float texY = ySlice / maxY;
                    float texZ = zSlice / maxZ;

                    float texXOff = 255.0 / maxX;
                    float texYOff = 255.0 / maxY;
                    //float texZOff = 255.0 / maxZ;

                    osg::Vec3Array* texCoords = new osg::Vec3Array;

                    texCoords->push_back( grid->transformTexCoord( osg::Vec3( 0.0,     0.0,     texZ ) ) );
                    texCoords->push_back( grid->transformTexCoord( osg::Vec3( 0.0,     texYOff, texZ ) ) );
                    texCoords->push_back( grid->transformTexCoord( osg::Vec3( texXOff, texYOff, texZ ) ) );
                    texCoords->push_back( grid->transformTexCoord( osg::Vec3( texXOff, 0.0,     texZ ) ) );
                    sliceGeometry->setTexCoordArray( c, texCoords );
                    ++c;
                }
                break;
            }
        }
    }
    else
    {
        float maxX = ( float )( m_properties->getValue<int>( "maxSagittal") );
        float maxY = ( float )( m_properties->getValue<int>( "maxCoronal") );
        float maxZ = ( float )( m_properties->getValue<int>( "maxAxial") );

        m_properties->setMax( "sagittalPos", maxX );
        m_properties->setMax( "coronalPos", maxY );
        m_properties->setMax( "axialPos", maxZ );

        float texX = xSlice / maxX;
        float texY = ySlice / maxY;
        float texZ = zSlice / maxZ;

        osg::Vec3Array* texCoords = new osg::Vec3Array;
        switch ( slice )
        {
            case 0:
                sliceVertices->push_back( osg::Vec3( 0,       yPos, 0      ) );
                sliceVertices->push_back( osg::Vec3( 0,       yPos, maxDim ) );
                sliceVertices->push_back( osg::Vec3( maxDim,  yPos, maxDim ) );
                sliceVertices->push_back( osg::Vec3( maxDim,  yPos, 0      ) );
                sliceGeometry->setVertexArray( sliceVertices );
                texCoords->push_back( osg::Vec3( 0.0, texY, 0.0 ) );
                texCoords->push_back( osg::Vec3( 0.0, texY, 1.0 ) );
                texCoords->push_back( osg::Vec3( 1.0, texY, 1.0 ) );
                texCoords->push_back( osg::Vec3( 1.0, texY, 0.0 ) );
                sliceGeometry->setTexCoordArray( 0, texCoords );
                break;
            case 1:
                sliceVertices->push_back( osg::Vec3( xPos, 0,      0      ) );
                sliceVertices->push_back( osg::Vec3( xPos, 0,      maxDim ) );
                sliceVertices->push_back( osg::Vec3( xPos, maxDim, maxDim ) );
                sliceVertices->push_back( osg::Vec3( xPos, maxDim, 0      ) );
                sliceGeometry->setVertexArray( sliceVertices );
                texCoords->push_back( osg::Vec3( texX, 0.0, 0.0 ) );
                texCoords->push_back( osg::Vec3( texX, 0.0, 1.0 ) );
                texCoords->push_back( osg::Vec3( texX, 1.0, 1.0 ) );
                texCoords->push_back( osg::Vec3( texX, 1.0, 0.0 ) );
                sliceGeometry->setTexCoordArray( 0, texCoords );
                break;
            case 2:
                sliceVertices->push_back( osg::Vec3( 0,      0,      zPos ) );
                sliceVertices->push_back( osg::Vec3( 0,      maxDim, zPos ) );
                sliceVertices->push_back( osg::Vec3( maxDim, maxDim, zPos ) );
                sliceVertices->push_back( osg::Vec3( maxDim, 0,      zPos ) );
                sliceGeometry->setVertexArray( sliceVertices );
                texCoords->push_back( osg::Vec3( 0.0, 0.0, texZ ) );
                texCoords->push_back( osg::Vec3( 0.0, 1.0, texZ ) );
                texCoords->push_back( osg::Vec3( 1.0, 1.0, texZ ) );
                texCoords->push_back( osg::Vec3( 1.0, 0.0, texZ ) );
                sliceGeometry->setTexCoordArray( 0, texCoords );
                break;
        }
    }

    osg::DrawElementsUInt* quad = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    quad->push_back( 3 );
    quad->push_back( 2 );
    quad->push_back( 1 );
    quad->push_back( 0 );
    sliceGeometry->addPrimitiveSet( quad );

    return sliceGeometry;
}

void WMNavSlices::updateGeometry()
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
    osg::ref_ptr<osg::Geometry> xSliceGeometry = createGeometry( 0 );
    osg::ref_ptr<osg::Geometry> ySliceGeometry = createGeometry( 1 );
    osg::ref_ptr<osg::Geometry> zSliceGeometry = createGeometry( 2 );

    osg::ref_ptr<osg::Drawable> oldx = osg::ref_ptr<osg::Drawable>( m_xSliceNode->getDrawable( 0 ) );
    m_xSliceNode->replaceDrawable( oldx, xSliceGeometry );
    osg::ref_ptr<osg::Drawable> oldy = osg::ref_ptr<osg::Drawable>( m_ySliceNode->getDrawable( 0 ) );
    m_ySliceNode->replaceDrawable( oldy, ySliceGeometry );
    osg::ref_ptr<osg::Drawable> oldz = osg::ref_ptr<osg::Drawable>( m_zSliceNode->getDrawable( 0 ) );
    m_zSliceNode->replaceDrawable( oldz, zSliceGeometry );

    if ( m_properties->getValue<bool>( "showAxial" ) )
    {
        m_zSliceNode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_zSliceNode->setNodeMask( 0x0 );
    }

    if ( m_properties->getValue<bool>( "showCoronal" ) )
    {
        m_xSliceNode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_xSliceNode->setNodeMask( 0x0 );
    }

    if ( m_properties->getValue<bool>( "showSagittal" ) )
    {
        m_ySliceNode->setNodeMask( 0xFFFFFFFF );
    }
    else
    {
        m_ySliceNode->setNodeMask( 0x0 );
    }

    slock.unlock();
}


void WMNavSlices::updateTextures()
{
    boost::shared_lock<boost::shared_mutex> slock;
    slock = boost::shared_lock<boost::shared_mutex>( m_updateLock );

    if ( m_properties->getValue< bool >( "textureChanged" ) && WKernel::getRunningKernel()->getGui()->isInitialized()() )
    {
        m_properties->setValue( "textureChanged", false );
        std::vector< boost::shared_ptr< WDataSet > > dsl = WKernel::getRunningKernel()->getGui()->getDataSetList( 0, true );

        if ( dsl.size() > 0 )
        {
            for ( int i = 0; i < 10; ++i )
            {
                m_typeUniforms[i]->set( 0 );
            }

            osg::StateSet* rootState = m_rootNode->getOrCreateStateSet();
            int c = 0;
            for ( size_t i = 0; i < dsl.size(); ++i )
            {
                osg::ref_ptr<osg::Texture3D> texture3D = dsl[i]->getTexture()->getTexture();

                rootState->setTextureAttributeAndModes( c, texture3D, osg::StateAttribute::ON );

                float t = dsl[i]->getTexture()->getThreshold() / 255.0;
                float a = dsl[i]->getTexture()->getAlpha();

                m_typeUniforms[c]->set( boost::shared_dynamic_cast<WDataSetSingle>( dsl[i] )->getValueSet()->getDataType() );
                m_thresholdUniforms[c]->set( t );
                m_alphaUniforms[c]->set( a );

                ++c;
            }
        }
    }
    slock.unlock();
}

void WMNavSlices::initUniforms( osg::StateSet* rootState )
{
    boost::shared_lock<boost::shared_mutex> slock;
    slock = boost::shared_lock<boost::shared_mutex>( m_updateLock );

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
        rootState->addUniform( m_typeUniforms[i] );
        rootState->addUniform( m_thresholdUniforms[i] );
        rootState->addUniform( m_alphaUniforms[i] );
        rootState->addUniform( m_samplerUniforms[i] );
    }
    slock.unlock();
}
