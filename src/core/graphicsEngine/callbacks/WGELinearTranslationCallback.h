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

#ifndef WGELINEARTRANSLATIONCALLBACK_H
#define WGELINEARTRANSLATIONCALLBACK_H

#include <osg/Node>
#include <osg/TexMat>
#include <osg/Uniform>
#include <osg/MatrixTransform>

#include "../../common/WProperties.h"


/**
 * This class is an OSG Callback which allows simple linear translation of a matrix transform node along a specified axis. It is controlled by a
 * WPropDouble. This way, one can simply implement movable slices and similar.
 *
 * \tparam T the type used as control mechanism. Typically, this should be an property whose type is cast-able to double. The type specified must
 * support access via T->get(). Specialize the class if you do not specify a pointer.
 */
template< typename T >
class WGELinearTranslationCallback: public osg::NodeCallback
{
public:
    /**
     * Constructor. Creates the callback. You still need to add it to the desired node.
     *
     * \param axe the axe to translate along. Should be normalized. If not, it scales the translation.
     * \param property the property containing the value
     * \param texMatrix optional pointer to a texture matrix which can be modified too to contain the normalized translation.
     * \param scaler scales the property by this value before creating the matrix.
     */
    WGELinearTranslationCallback( osg::Vec3 axe, T property, osg::ref_ptr< osg::TexMat > texMatrix, double scaler = 1.0 );

    /**
     * Constructor. Creates the callback. You still need to add it to the desired node.
     *
     * \param axe the axe to translate along. Should be normalized. If not, it scales the translation.
     * \param property the property containing the value
     * \param uniform optional pointer to a uniform that will contain the matrix. Useful if no tex-matrix is available anymore. The matrix is the
     *                matrix that is NOT scaled to be in texture space.
     * \param scaler scales the property by this value before creating the matrix.
     */
    WGELinearTranslationCallback( osg::Vec3 axe, T property, osg::ref_ptr< osg::Uniform > uniform, double scaler = 1.0 );

    /**
     * Constructor. Creates the callback. You still need to add it to the desired node.
     *
     * \param axe the axe to translate along. Should be normalized. If not, it scales the translation.
     * \param property the property containing the value
     * \param scaler scales the property by this value before creating the matrix.
     */
    WGELinearTranslationCallback( osg::Vec3 axe, T property, double scaler = 1.0 );

    /**
     * Destructor.
     */
    virtual ~WGELinearTranslationCallback();

    /**
     * This operator gets called by OSG every update cycle. It moves the underlying MatrixTransform according to the specified axis and value.
     *
     * \param node the osg node
     * \param nv the node visitor
     */
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

protected:
    /**
     * The axis to transform along.
     */
    osg::Vec3 m_axe;

    /**
     * The position
     */
    T m_pos;

    /**
     * Cache the old position for proper update
     */
    double m_oldPos;

    /**
     * Texture matrix that contains normalized translation.
     */
    osg::ref_ptr< osg::TexMat > m_texMat;

    /**
     * The uniform to set the matrix to.
     */
    osg::ref_ptr< osg::Uniform > m_uniform;

    /**
     * Scale the property prior to creating the matrix.
     */
    double m_scaler;
private:
};

template< typename T >
WGELinearTranslationCallback< T >::WGELinearTranslationCallback( osg::Vec3 axe, T property, osg::ref_ptr< osg::TexMat > texMatrix, double scaler ):
    osg::NodeCallback(),
    m_axe( axe ),
    m_pos( property ),
    m_oldPos( -1.0 ),
    m_texMat( texMatrix ),
    m_scaler( scaler )
{
    // initialize members
}

template< typename T >
WGELinearTranslationCallback< T >::WGELinearTranslationCallback( osg::Vec3 axe, T property, osg::ref_ptr< osg::Uniform > uniform, double scaler ):
    osg::NodeCallback(),
    m_axe( axe ),
    m_pos( property ),
    m_oldPos( -1.0 ),
    m_uniform( uniform ),
    m_scaler( scaler )
{
    // initialize members
}

template< typename T >
WGELinearTranslationCallback< T >::WGELinearTranslationCallback( osg::Vec3 axe, T property, double scaler ):
    osg::NodeCallback(),
    m_axe( axe ),
    m_pos( property ),
    m_oldPos( -1.0 ),
    m_scaler( scaler )
{
    // initialize members
}

template< typename T >
WGELinearTranslationCallback< T >::~WGELinearTranslationCallback()
{
    // cleanup
}

template< typename T >
void WGELinearTranslationCallback< T >::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // this node is a MatrixTransform
    float newPos = m_pos->get();
    if( newPos != m_oldPos )
    {
        m_oldPos = newPos;
        osg::MatrixTransform* m = static_cast< osg::MatrixTransform* >( node );
        if( m )
        {
            float max = m_pos->getMax()->getMax();
            float min = m_pos->getMin()->getMin();
            float size = max - min;
            float axeLen = m_axe.length();

            osg::Vec3 translation = m_axe * m_scaler * static_cast< float >( m_oldPos - min );

            // set both matrices
            if( m_texMat )
            {
                m_texMat->setMatrix( osg::Matrix::translate( translation / size / axeLen ) );
            }
            if( m_uniform )
            {
                m_uniform->set( osg::Matrix::translate( translation ) );
            }

            m->setMatrix( osg::Matrix::translate( translation ) );
        }
    }

    traverse( node, nv );
}

#endif  // WGELINEARTRANSLATIONCALLBACK_H

