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

#ifndef WGRIDTRANSFORMORTHO_H
#define WGRIDTRANSFORMORTHO_H

#include "../common/exceptions/WPreconditionNotMet.h"
#include "../common/math/WMatrix.h"
#include "../common/math/linearAlgebra/WLinearAlgebra.h"



/**
 * Implements an orthogonal grid transformation.
 *
 * \class WGridTransformOrthoTemplate
 */
template< typename T >
class WGridTransformOrthoTemplate
{
    // this (friend) is necessary to allow casting
    template <class U>
    friend class WGridTransformOrthoTemplate;
public:
    /**
     * Convenience typedef for 3d vectors of the appropriate numerical type.
     */
    typedef WMatrixFixed< T, 3, 1 > Vector3Type;

    /**
     * Constructs an identity transform.
     */
    WGridTransformOrthoTemplate();

    /**
     * Copy constructor.
     * Copies the data from an WGridTransformOrthoTemplate object with arbitary numerical type.
     *
     * \param rhs A WGridTransformOrthoTemplate object, which mustn't have the same numerical type.
     */
    template< typename InputType >
    WGridTransformOrthoTemplate( WGridTransformOrthoTemplate< InputType > const& rhs ); // NOLINT -- no explicit, this allows casts
    /**
     * Construct a transformation that scales the grid space.
     * \param scaleX The scale in the x-direction.
     * \param scaleY The scale in the y-direction.
     * \param scaleZ The scale in the z-direction.
     */
    template< typename InputType >
    WGridTransformOrthoTemplate( InputType scaleX, InputType scaleY, InputType scaleZ );

    /**
     * Construct a transformation from a transformation matrix. The provided matrix
     * represents the transformation from grid to world space.
     * \param mat The matrix.
     */
    template< typename InputType >
    WGridTransformOrthoTemplate( WMatrix< InputType > const& mat );  // NOLINT

    /**
     * Destructor.
     */
    ~WGridTransformOrthoTemplate();


    /**
     * Assignment operator.
     * Copies the data from an WGridTransformOrthoTemplate object with arbitary numerical type.
     *
     * \param rhs A WGridTransformOrthoTemplate object, which mustn't have the same numerical type.
     *
     * \return this
     */
    template< typename InputType >
    WGridTransformOrthoTemplate< T >& operator=( WGridTransformOrthoTemplate< InputType > const& rhs );
    /**
     * Transforms a position from grid space to world space.
     * \param position The position in grid space.
     * \return The same position in world space.
     */
    Vector3Type positionToWorldSpace( Vector3Type const& position ) const;

    /**
     * Transforms a position from world space to grid space.
     * \param position The position in world space.
     * \return The same position in grid space.
     */
    Vector3Type positionToGridSpace( Vector3Type const& position ) const;

    /**
     * Transforms a direction from grid space to world space.
     * \param direction The direction in grid space.
     * \return The same direction in world space.
     */
    Vector3Type directionToWorldSpace( Vector3Type const& direction ) const;

    /**
     * Transforms a direction from world space to grid space.
     * \param direction The position in world space.
     * \return The same position in grid space.
     */
    Vector3Type directionToGridSpace( Vector3Type const& direction ) const;

    /**
     * Returns the distance between samples in x direction.
     * \return The distance between samples in x direction.
     */
    T getOffsetX() const;

    /**
     * Returns the distance between samples in y direction.
     * \return The distance between samples in y direction.
     */
    T getOffsetY() const;

    /**
     * Returns the distance between samples in z direction.
     * \return The distance between samples in z direction.
     */
    T getOffsetZ() const;

    /**
     * Returns the vector determining the direction of samples in x direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) x-axis.
     * \return The vector determining the direction of samples in x direction.
     */
    Vector3Type getDirectionX() const;

    /**
     * Returns the vector determining the direction of samples in y direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) y-axis.
     * \return The vector determining the direction of samples in y direction.
     */
    Vector3Type getDirectionY() const;

    /**
     * Returns the vector determining the direction of samples in z direction.
     * Adding this vector to a grid position in world coordinates yields the position of the next sample
     * along the grids (world coordinate) z-axis.
     * \return The vector determining the direction of samples in z direction.
     */
    Vector3Type getDirectionZ() const;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in x direction.
     * \return The vector determining the unit (normalized) direction of samples in x direction.
     */
    Vector3Type getUnitDirectionX() const;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in y direction.
     * \return The vector determining the unit (normalized) direction of samples in y direction.
     */
    Vector3Type getUnitDirectionY() const;

    /**
     * Returns the vector determining the unit (normalized) direction of samples in z direction.
     * \return The vector determining the unit (normalized) direction of samples in z direction.
     */
    Vector3Type getUnitDirectionZ() const;

    /**
     * Returns the position of the origin of the grid.
     * \return The position of the origin of the grid.
     */
    Vector3Type getOrigin() const;

    /**
     * Returns the scaling of the grid.
     * \return The scaling of the grid.
     */
    const Vector3Type& getScaling() const;

    /**
     * Returns a 4x4 matrix that represents the grid's transformaion.
     * \return The grid's transformation.
     */
    // NOTE: this is temporary and should be removed as soon as all modules are
    // adapted to the grid transform object
    WMatrix< T > getTransformationMatrix() const;

    /**
     * Cast the transformation to the corresponding 4x4 matrix.
     *
     * \return the matrix representing the transform
     */
    operator WMatrix4d() const;

    /**
     * Check if this transform does not include a rotation.
     *
     * \return True, if this transform only scales and translates.
     */
    bool isNotRotated() const;

    /**
     * Translate by a vector.
     *
     * \param vec The vector.
     */
    template< typename VecType >
    void translate( VecType const& vec );

    /**
     * Scale the transform.
     *
     * \param scale A vector of scaling coeffs for the 3 directions.
     */
    template< typename VecType >
    void scale( VecType const& scale );

private:
    /**
     * This is a helper function which copies the parameter of another instance to its own.
     *
     * \param input A WGridTransformOrthoTemplate object with the numerical type InputType.
     */
    template< typename InputType >
    void copyFrom( WGridTransformOrthoTemplate< InputType > const& input );

    //! normalized direction of the grid's x-axis in world coordinates
    Vector3Type m_unitDirectionX;

    //! normalized direction of the grid's y-axis in world coordinates
    Vector3Type m_unitDirectionY;

    //! normalized direction of the grid's z-axis in world coordinates
    Vector3Type m_unitDirectionZ;

    //! the scaling factors for the 3 axes, i.e. the distance between samples
    Vector3Type m_scaling;

    //! the origin of the grid in world coordinates
    Vector3Type m_origin;
};

typedef WGridTransformOrthoTemplate< double > WGridTransformOrtho;
typedef WGridTransformOrthoTemplate< float > WGridTransformOrthoFloat;

template< typename T >
WGridTransformOrthoTemplate< T >::WGridTransformOrthoTemplate()
    : m_unitDirectionX( 1.0, 0.0, 0.0 ),
      m_unitDirectionY( 0.0, 1.0, 0.0 ),
      m_unitDirectionZ( 0.0, 0.0, 1.0 ),
      m_scaling( 1.0, 1.0, 1.0 ),
      m_origin( 0.0, 0.0, 0.0 )
{
}

template< typename T >
template< typename InputType >
WGridTransformOrthoTemplate< T >::WGridTransformOrthoTemplate( WGridTransformOrthoTemplate< InputType > const& rhs  )
{
    copyFrom( rhs );
}

template< typename T >
template< typename InputType >
WGridTransformOrthoTemplate< T >::WGridTransformOrthoTemplate( InputType scaleX, InputType scaleY, InputType scaleZ )
    : m_unitDirectionX( ( scaleX > 0.0 ) - ( scaleX < 0.0 ), 0.0, 0.0 ),
      m_unitDirectionY( 0.0, ( scaleY > 0.0 ) - ( scaleY < 0.0 ), 0.0 ),
      m_unitDirectionZ( 0.0, 0.0, ( scaleZ > 0.0 ) - ( scaleZ < 0.0 ) ),
      m_scaling( fabs( scaleX ), fabs( scaleY ), fabs( scaleZ ) ),
      m_origin( 0.0, 0.0, 0.0 )
{
    WPrecond( m_scaling[ 0 ] != 0.0 && m_scaling[ 1 ] != 0.0 && m_scaling[ 2 ] != 0.0, "" );
}

template< typename T >
template< typename InputType >
WGridTransformOrthoTemplate< T >::WGridTransformOrthoTemplate( WMatrix< InputType > const& mat )
{
    WPrecond( mat.getNbRows() == 4 && mat.getNbCols() == 4, "" );
    m_unitDirectionX = Vector3Type( mat( 0, 0 ), mat( 1, 0 ), mat( 2, 0 ) );
    m_unitDirectionY = Vector3Type( mat( 0, 1 ), mat( 1, 1 ), mat( 2, 1 ) );
    m_unitDirectionZ = Vector3Type( mat( 0, 2 ), mat( 1, 2 ), mat( 2, 2 ) );

    m_scaling = Vector3Type( length( m_unitDirectionX ), length( m_unitDirectionY ), length( m_unitDirectionZ ) );

    WPrecond( m_scaling[ 0 ] != 0.0 && m_scaling[ 1 ] != 0.0 && m_scaling[ 2 ] != 0.0, "" );
    m_unitDirectionX /= m_scaling[ 0 ];
    m_unitDirectionY /= m_scaling[ 1 ];
    m_unitDirectionZ /= m_scaling[ 2 ];

    WPrecondLess( fabs( dot( m_unitDirectionX, m_unitDirectionY ) ), 0.0001 );
    WPrecondLess( fabs( dot( m_unitDirectionX, m_unitDirectionZ ) ), 0.0001 );
    WPrecondLess( fabs( dot( m_unitDirectionY, m_unitDirectionZ ) ), 0.0001 );
    m_origin = Vector3Type( mat( 0, 3 ), mat( 1, 3 ), mat( 2, 3 ) );
}

template< typename T >
WGridTransformOrthoTemplate< T >::~WGridTransformOrthoTemplate()
{
}

template< typename T >
template< typename InputType >
WGridTransformOrthoTemplate< T >& WGridTransformOrthoTemplate< T >::operator=( WGridTransformOrthoTemplate< InputType > const& rhs )
{
    if( this != &rhs )
    {
        copyFrom( rhs );
    }
    return *this;
}

template< typename T >
typename WGridTransformOrthoTemplate< T >::Vector3Type WGridTransformOrthoTemplate< T >::positionToWorldSpace( Vector3Type const& position ) const
{
    return Vector3Type( m_scaling[ 0 ] * position[ 0 ] * m_unitDirectionX[ 0 ] +
                        m_scaling[ 1 ] * position[ 1 ] * m_unitDirectionY[ 0 ] +
                        m_scaling[ 2 ] * position[ 2 ] * m_unitDirectionZ[ 0 ] +
                        m_origin[ 0 ],
                        m_scaling[ 0 ] * position[ 0 ] * m_unitDirectionX[ 1 ] +
                        m_scaling[ 1 ] * position[ 1 ] * m_unitDirectionY[ 1 ] +
                        m_scaling[ 2 ] * position[ 2 ] * m_unitDirectionZ[ 1 ] +
                        m_origin[ 1 ],
                        m_scaling[ 0 ] * position[ 0 ] * m_unitDirectionX[ 2 ] +
                        m_scaling[ 1 ] * position[ 1 ] * m_unitDirectionY[ 2 ] +
                        m_scaling[ 2 ] * position[ 2 ] * m_unitDirectionZ[ 2 ] +
                        m_origin[ 2 ] );
}

template< typename T >
typename WGridTransformOrthoTemplate< T >::Vector3Type WGridTransformOrthoTemplate< T >::positionToGridSpace( Vector3Type const& position ) const
{
    Vector3Type p = position - m_origin;
    p = Vector3Type( dot( p, m_unitDirectionX ), dot( p, m_unitDirectionY ), dot( p, m_unitDirectionZ ) );
    p[ 0 ] /= m_scaling[ 0 ];
    p[ 1 ] /= m_scaling[ 1 ];
    p[ 2 ] /= m_scaling[ 2 ];
    return p;
}

template< typename T >
typename WGridTransformOrthoTemplate< T >::Vector3Type WGridTransformOrthoTemplate< T >::directionToWorldSpace( Vector3Type const& direction ) const
{
    return Vector3Type( m_scaling[ 0 ] * direction[ 0 ] * m_unitDirectionX[ 0 ] +
                        m_scaling[ 1 ] * direction[ 1 ] * m_unitDirectionY[ 0 ] +
                        m_scaling[ 2 ] * direction[ 2 ] * m_unitDirectionZ[ 0 ],

                        m_scaling[ 0 ] * direction[ 0 ] * m_unitDirectionX[ 1 ] +
                        m_scaling[ 1 ] * direction[ 1 ] * m_unitDirectionY[ 1 ] +
                        m_scaling[ 2 ] * direction[ 2 ] * m_unitDirectionZ[ 1 ],

                        m_scaling[ 0 ] * direction[ 0 ] * m_unitDirectionX[ 2 ] +
                        m_scaling[ 1 ] * direction[ 1 ] * m_unitDirectionY[ 2 ] +
                        m_scaling[ 2 ] * direction[ 2 ] * m_unitDirectionZ[ 2 ] );
}

template< typename T >
typename WGridTransformOrthoTemplate< T >::Vector3Type WGridTransformOrthoTemplate< T >::directionToGridSpace( Vector3Type const& direction ) const
{
    Vector3Type p( dot( direction, m_unitDirectionX ), dot( direction, m_unitDirectionY ), dot( direction, m_unitDirectionZ ) );
    p[ 0 ] /= m_scaling[ 0 ];
    p[ 1 ] /= m_scaling[ 1 ];
    p[ 2 ] /= m_scaling[ 2 ];
    return p;
}

template< typename T >
T WGridTransformOrthoTemplate< T >::getOffsetX() const
{
    return m_scaling[ 0 ];
}

template< typename T >
T WGridTransformOrthoTemplate< T >::getOffsetY() const
{
    return m_scaling[ 1 ];
}

template< typename T >
T WGridTransformOrthoTemplate< T >::getOffsetZ() const
{
    return m_scaling[ 2 ];
}

template< typename T >
typename WGridTransformOrthoTemplate< T >::Vector3Type WGridTransformOrthoTemplate< T >::getDirectionX() const
{
    return m_unitDirectionX * m_scaling[ 0 ];
}

template< typename T >
typename WGridTransformOrthoTemplate< T >::Vector3Type WGridTransformOrthoTemplate< T >::getDirectionY() const
{
    return m_unitDirectionY * m_scaling[ 1 ];
}

template< typename T >
typename WGridTransformOrthoTemplate< T >::Vector3Type WGridTransformOrthoTemplate< T >::getDirectionZ() const
{
    return m_unitDirectionZ * m_scaling[ 2 ];
}

template< typename T >
typename WGridTransformOrthoTemplate< T >::Vector3Type WGridTransformOrthoTemplate< T >::getUnitDirectionX() const
{
    return m_unitDirectionX;
}

template< typename T >
typename WGridTransformOrthoTemplate< T >::Vector3Type WGridTransformOrthoTemplate< T >::getUnitDirectionY() const
{
    return m_unitDirectionY;
}

template< typename T >
typename WGridTransformOrthoTemplate< T >::Vector3Type WGridTransformOrthoTemplate< T >::getUnitDirectionZ() const
{
    return m_unitDirectionZ;
}

template< typename T >
typename WGridTransformOrthoTemplate< T >::Vector3Type WGridTransformOrthoTemplate< T >::getOrigin() const
{
    return m_origin;
}

template< typename T >
inline const typename WGridTransformOrthoTemplate< T >::Vector3Type& WGridTransformOrthoTemplate< T >::getScaling() const
{
    return m_scaling;
}

template< typename T >
WMatrix< T > WGridTransformOrthoTemplate< T >::getTransformationMatrix() const
{
    WMatrix< T > mat( 4, 4 );
    mat.makeIdentity();
    mat( 0, 0 ) = m_scaling[ 0 ] * m_unitDirectionX[ 0 ];
    mat( 1, 0 ) = m_scaling[ 0 ] * m_unitDirectionX[ 1 ];
    mat( 2, 0 ) = m_scaling[ 0 ] * m_unitDirectionX[ 2 ];
    mat( 0, 1 ) = m_scaling[ 1 ] * m_unitDirectionY[ 0 ];
    mat( 1, 1 ) = m_scaling[ 1 ] * m_unitDirectionY[ 1 ];
    mat( 2, 1 ) = m_scaling[ 1 ] * m_unitDirectionY[ 2 ];
    mat( 0, 2 ) = m_scaling[ 2 ] * m_unitDirectionZ[ 0 ];
    mat( 1, 2 ) = m_scaling[ 2 ] * m_unitDirectionZ[ 1 ];
    mat( 2, 2 ) = m_scaling[ 2 ] * m_unitDirectionZ[ 2 ];
    mat( 0, 3 ) = m_origin[ 0 ];
    mat( 1, 3 ) = m_origin[ 1 ];
    mat( 2, 3 ) = m_origin[ 2 ];
    return mat;
}

template< typename T >
WGridTransformOrthoTemplate< T >::operator WMatrix4d() const
{
    WMatrix4d mat = WMatrix4d::identity();
    mat( 0, 0 ) = m_scaling[ 0 ] * m_unitDirectionX[ 0 ];
    mat( 0, 1 ) = m_scaling[ 0 ] * m_unitDirectionX[ 1 ];
    mat( 0, 2 ) = m_scaling[ 0 ] * m_unitDirectionX[ 2 ];
    mat( 1, 0 ) = m_scaling[ 1 ] * m_unitDirectionY[ 0 ];
    mat( 1, 1 ) = m_scaling[ 1 ] * m_unitDirectionY[ 1 ];
    mat( 1, 2 ) = m_scaling[ 1 ] * m_unitDirectionY[ 2 ];
    mat( 2, 0 ) = m_scaling[ 2 ] * m_unitDirectionZ[ 0 ];
    mat( 2, 1 ) = m_scaling[ 2 ] * m_unitDirectionZ[ 1 ];
    mat( 2, 2 ) = m_scaling[ 2 ] * m_unitDirectionZ[ 2 ];
    mat( 3, 0 ) = m_origin[ 0 ];
    mat( 3, 1 ) = m_origin[ 1 ];
    mat( 3, 2 ) = m_origin[ 2 ];
    return mat;
}

template< typename T >
bool WGridTransformOrthoTemplate< T >::isNotRotated() const
{
    return m_unitDirectionX == Vector3Type( T( 1.0 ), T( 0.0 ), T( 0.0 ) )
        && m_unitDirectionY == Vector3Type( T( 0.0 ), T( 1.0 ), T( 0.0 ) )
        && m_unitDirectionZ == Vector3Type( T( 0.0 ), T( 0.0 ), T( 1.0 ) );
}

template< typename T >
template< typename VecType >
void WGridTransformOrthoTemplate< T >::translate( VecType const& vec )
{
    m_origin[ 0 ] += vec[ 0 ];
    m_origin[ 1 ] += vec[ 1 ];
    m_origin[ 2 ] += vec[ 2 ];
}

template< typename T >
template< typename VecType>
void WGridTransformOrthoTemplate< T >::scale( VecType const& scale )
{
    m_scaling[ 0 ] *= scale[ 0 ];
    m_scaling[ 1 ] *= scale[ 1 ];
    m_scaling[ 2 ] *= scale[ 2 ];
}

template< typename T >
template< typename InputType >
void WGridTransformOrthoTemplate< T >::copyFrom( WGridTransformOrthoTemplate< InputType > const& input )
{
    this->m_unitDirectionX = static_cast< Vector3Type >( input.m_unitDirectionX );
    this->m_unitDirectionY = static_cast< Vector3Type >( input.m_unitDirectionY );
    this->m_unitDirectionZ = static_cast< Vector3Type >( input.m_unitDirectionZ );
    this->m_scaling = static_cast< Vector3Type >( input.m_scaling );
    this->m_origin = static_cast< Vector3Type >( input.m_origin );
}

#endif  // WGRIDTRANSFORMORTHO_H
