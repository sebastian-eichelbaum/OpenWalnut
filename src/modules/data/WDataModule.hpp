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

#ifndef WDATAMODULE_H
#define WDATAMODULE_H

#include <string>

#include <boost/shared_ptr.hpp>

#include <osg/Texture3D>

#include "../../kernel/WKernel.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleConnector.h"
#include "../../kernel/WModuleOutputData.hpp"


/**
 * Module for encapsulating WDataSets. It can encapsulate almost everything, but is intended to be used with WDataSets and its
 * inherited classes. This class builds a "source".
 */
template < typename T >
class WDataModule: public WModule
{
public:

    /**
     * \par Description
     * Default constructor.
     */
    // WDataModule();

    /**
     * \par Description
     * constructor with dataset
     */
    explicit WDataModule( boost::shared_ptr< WDataSet > dataSet );

    /**
     * \par Description
     * Destructor.
     */
    virtual ~WDataModule();

    /**
     * \par Description
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * \par Description
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     *  setter for the asociated dataset
     */
    virtual void setDataSet( boost::shared_ptr< WDataSet > dataSet );

    /**
     * getter for the dataset
     */
    virtual boost::shared_ptr< WDataSet > getDataSet();

    /**
     * getter for the 3d texture, which will be created on demand
     */
    virtual osg::Texture3D* getTexture3D();

protected:

    /**
     * \par Description
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void threadMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Receive DATA_CHANGE notifications.
     *
     * \param input the input connector that got the change signal. Typically it is one of the input connectors from this module.
     * \param output the output connector that sent the signal. Not part of this module instance.
     */
    virtual void notifyDataChange( boost::shared_ptr<WModuleConnector> input,
                                   boost::shared_ptr<WModuleConnector> output );

    /**
     * Creates a 3d texture from a dataset. This function will be overloaded for the
     * various data types. A template function is not recommended due to the different commands
     * in the image creation.
     *
     * TODO(schurade): create other functions once dataset meta data is available again
     *
     * \param source Pointer to the raw data of a dataset
     * \param components Number of values used in a Voxel, usually 1, 3 or 4
     * \return Pointer to a new texture3D
     */
    osg::Texture3D* createTexture3D( unsigned char* source, int components = 1 );
    osg::Texture3D* createTexture3D( int16_t* source, int components = 1 );
    osg::Texture3D* createTexture3D( float* source, int components = 1 );


private:

    /**
     * The only output of this data module.
     */
    boost::shared_ptr<WModuleOutputData<T> > m_output;

    /**
     * pointer to the dataset
     */
    boost::shared_ptr< WDataSet > m_dataSet;

    /**
     * pointer to the 3d texture
     */
    osg::Texture3D* m_texture3D;
};

// TODO(schurade, ebaum): do we still need/want that constructor?
// template < typename T >
// WDataModule<T>::WDataModule():
//    WModule()
// {
//    // WARNING: initializing connectors inside the constructor will lead to an exception.
//    // Implement WModule::initializeConnectors instead.
//
//    // initialize members
//    m_properties.addString( "name", "not initialized" );
// }

template < typename T >
WDataModule<T>::WDataModule( boost::shared_ptr< WDataSet > dataSet ):
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.

    // initialize members
    m_dataSet = dataSet;
    m_texture3D = 0;
    m_properties.addString( "name", "not initialized" );
}

template < typename T >
WDataModule<T>::~WDataModule()
{
    // cleanup
}

template < typename T >
const std::string WDataModule<T>::getName() const
{
    return "Data Module";
}

template < typename T >
const std::string WDataModule<T>::getDescription() const
{
    return "This module can encapsulate data.";
}

template < typename T >
void WDataModule<T>::connectors()
{
    // initialize connectors
    m_output= boost::shared_ptr<WModuleOutputData<T> >(
            new WModuleOutputData<T>( shared_from_this(),
                "out1", "A loaded dataset." )
    );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_output );

    // call WModules initialization
    WModule::connectors();
}

template < typename T >
void WDataModule<T>::notifyDataChange( boost::shared_ptr<WModuleConnector> input,
                                               boost::shared_ptr<WModuleConnector> output )
{
    WModule::notifyDataChange( input, output );
}

template < typename T >
void WDataModule<T>::threadMain()
{
    // Since the modules run in a separate thread: such loops are possible
    while ( !m_FinishRequested )
    {
        // do fancy stuff
        sleep( 1 );
    }

    // clean up stuff
}

template < typename T >
void WDataModule<T>::setDataSet( boost::shared_ptr< WDataSet > dataSet )
{
    m_dataSet = dataSet;
}

template < typename T >
boost::shared_ptr< WDataSet > WDataModule<T>::getDataSet()
{
    return m_dataSet;
}

template < typename T >
osg::Texture3D* WDataModule<T>::getTexture3D()
{
    if ( !m_texture3D )
    {
        boost::shared_ptr< WDataSetSingle > ds = boost::shared_dynamic_cast< WDataSetSingle >( m_dataSet );
        std::cout << "type:" << ds->getValueSet()->getDataType() << std::endl;

        if ( ds->getValueSet()->getDataType() == 2 )
        {
            boost::shared_ptr< WValueSet< unsigned char > > vs = boost::shared_dynamic_cast< WValueSet< unsigned char > >( ds->getValueSet() );
            unsigned char* source = const_cast< unsigned char* > ( vs->rawData() );
            m_texture3D = createTexture3D( source, ds->getValueSet()->dimension() );
        }

        else if ( ds->getValueSet()->getDataType() == 4 )
        {
            boost::shared_ptr< WValueSet< int16_t > > vs = boost::shared_dynamic_cast< WValueSet< int16_t > >( ds->getValueSet() );
            int16_t* source = const_cast< int16_t* > ( vs->rawData() );
            m_texture3D = createTexture3D( source, ds->getValueSet()->dimension() );
        }

        else if ( ds->getValueSet()->getDataType() == 16 )
        {
            boost::shared_ptr< WValueSet< float > > vs = boost::shared_dynamic_cast< WValueSet< float > >( ds->getValueSet() );
            float* source = const_cast< float* > ( vs->rawData() );
            m_texture3D = createTexture3D( source, ds->getValueSet()->dimension() );
        }
    }
    return m_texture3D;
}

template < typename T >
osg::Texture3D* WDataModule<T>::createTexture3D( unsigned char* source, int components )
{
    if ( components == 1 )
    {
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

        return texture3D;
    }
    else if ( components == 3 )
    {
        osg::ref_ptr< osg::Image > ima = new osg::Image;
        ima->allocateImage( 160, 200, 160, GL_RGB, GL_UNSIGNED_BYTE );

        unsigned char* data = ima->data();

        for ( unsigned int i = 0; i < 160 * 200 * 160 * 3; ++i )
        {
            data[i] = source[i];
        }
        osg::Texture3D* texture3D = new osg::Texture3D;
        texture3D->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
        texture3D->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
        texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::REPEAT );
        texture3D->setImage( ima );
        texture3D->setResizeNonPowerOfTwoHint( false );

        return texture3D;
    }

    return 0;
}

template < typename T >
osg::Texture3D* WDataModule<T>::createTexture3D( int16_t* source, int components )
{
    if ( components == 1)
    {
        osg::ref_ptr< osg::Image > ima = new osg::Image;
        ima->allocateImage( 160, 200, 160, GL_LUMINANCE, GL_SHORT );

        unsigned char* data = ima->data();

        unsigned char* charSource = ( unsigned char* )source;

        for ( unsigned int i = 0; i < 160* 200* 160* 2 ; ++i )
        {
            data[i] = charSource[i];
        }
        osg::Texture3D* texture3D = new osg::Texture3D;
        texture3D->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
        texture3D->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
        texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::REPEAT );
        texture3D->setImage( ima );
        texture3D->setResizeNonPowerOfTwoHint( false );

        return texture3D;
    }
    return 0;
}

template < typename T >
osg::Texture3D* WDataModule<T>::createTexture3D( float* source, int components )
{
    if ( components == 1)
    {
        osg::ref_ptr< osg::Image > ima = new osg::Image;
        ima->allocateImage( 160, 200, 160, GL_LUMINANCE, GL_FLOAT );

        unsigned char* data = ima->data();

        unsigned char* charSource = ( unsigned char* )source;

        for ( unsigned int i = 0; i < 160* 200* 160* 4 ; ++i )
        {
            data[i] = charSource[i];
        }
        osg::Texture3D* texture3D = new osg::Texture3D;
        texture3D->setFilter( osg::Texture3D::MIN_FILTER, osg::Texture3D::LINEAR );
        texture3D->setFilter( osg::Texture3D::MAG_FILTER, osg::Texture3D::LINEAR );
        texture3D->setWrap( osg::Texture3D::WRAP_R, osg::Texture3D::REPEAT );
        texture3D->setImage( ima );
        texture3D->setResizeNonPowerOfTwoHint( false );

        return texture3D;
    }
    return 0;
}



#endif  // WDATAMODULE_H

