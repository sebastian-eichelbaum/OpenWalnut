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

#include <cmath>
#include <iostream>

#include "../../common/math/WMath.h"
#include "../../common/math/linearAlgebra/WVectorFixed.h"

#include "../../common/WTimer.h"

#include "WGEAnimationManipulator.h"

WGEAnimationManipulator::WGEAnimationManipulator( WTimer::ConstSPtr timer ):
    m_matrix( osg::Matrixd::identity() ),
    m_timer( timer ),
    m_homeOffsetTime( timer->elapsed() )
{
    // initialize
}

WGEAnimationManipulator::~WGEAnimationManipulator()
{
    // clean up
}

void WGEAnimationManipulator::setByMatrix( const osg::Matrixd& matrix )
{
    m_matrix.invert( matrix );
}

void WGEAnimationManipulator::setByInverseMatrix( const osg::Matrixd& matrix )
{
    m_matrix = matrix;
}

osg::Matrixd WGEAnimationManipulator::getMatrix() const
{
    return osg::Matrixd::inverse( m_matrix );
}

osg::Matrixd WGEAnimationManipulator::getInverseMatrix() const
{
    return m_matrix;
}

bool WGEAnimationManipulator::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /* us */ )
{
    switch( ea.getEventType() )
    {
    case osgGA::GUIEventAdapter::FRAME:
        handleFrame();
        return false;
    case osgGA::GUIEventAdapter::KEYDOWN:
        // if ( ea.getKey() == ' ' )   // space resets
        // {
        //     home( 0 );
        //     return true;
        // }
    default:
        break;
    }
    return false;
}

void WGEAnimationManipulator::init( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
    home( ea, us );
}

void WGEAnimationManipulator::home( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /* us */ )
{
    home( ea.getTime() );
}

void WGEAnimationManipulator::home( double /* currentTime */ )
{
    m_homeOffsetTime = m_timer->elapsed();
}

void WGEAnimationManipulator::setTimer( WTimer::ConstSPtr timer )
{
    m_timer = timer;
    home( 0 );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

double degToRad( double deg )
{
    return deg * ( pi() / 180.0 );
}

double radToDeg( double rad )
{
    return rad * ( 180.0 / pi() );
}

template < typename T >
T sign( T value )
{
    if( value > T( 0 ) )
    {
        return 1;
    }
    if( value < T( 0 ) )
    {
        return -1;
    }
    return T( 0 );
}

template < typename T >
T positive( T value )
{
    return value > T( 0 ) ? 1 : 0;
}

/**
 * Function which smooths the given value. It uses the cos function to do it. If the value is larger than the maximum, maximum is returned. If
 * value is smaller then min, min is returned. In between, the function looks like $$cos( pi + x )$$ with $$x \in [0, pi]$$.
 *
 * \param value the value.
 * \param min minimum, used for clamping.
 * \param max maximum, used for clamping.
 *
 * \return
 */
double smooth( double value, double min, double max )
{
    if( value >= max )
    {
        return max;
    }
    if( value < min )
    {
        return min;
    }

    double scaledValue = ( value - min ) / max;
    return min + max * ( 0.5 * ( 1.0 + cos( pi() + ( scaledValue * piDouble ) ) ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Transformators
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Base class for all further transformations.
 */
class Transformation: public osg::Matrixd       // NOTE: yes this is a bad idea since osg::Matrixd does not provide a virtual destructor.
{
public:
    /**
     * The time in seconds when this transformation is finished.
     *
     * \return finish time in seconds.
     */
    virtual double finish() const
    {
        return m_finishTime;
    }

    /**
     * The time in seconds this transformation is running.
     *
     * \return duration in seconds
     */
    virtual double duration() const
    {
        return m_duration;
    }

    /**
     * Vector representing the X axe
     */
    static WVector3d axeX;

    /**
     * Vector representing the Y axe
     */
    static WVector3d axeY;

    /**
     * Vector representing the Z axe
     */
    static WVector3d axeZ;

protected:
    /**
     * The time this is finished.
     */
    double m_finishTime;

    /**
     * Duration time.
     */
    double m_duration;
};

// Initialize the statics
WVector3d Transformation::axeX = WVector3d( 1.0, 0.0, 0.0 );
WVector3d Transformation::axeY = WVector3d( 0.0, 1.0, 0.0 );
WVector3d Transformation::axeZ = WVector3d( 0.0, 0.0, 1.0 );

/**
 * Provides a time dependent rotation around a specified axis.
 */
class Rotator: public Transformation
{
public:
    /**
     * Create a rotation matrix which rotates a certain number of degree with a given speed. This means, that the time interval is defined by speed
     * and degree.
     *
     * \param degree rotate this number of degree
     * \param speed  rotation speed in degree per second
     * \param time   current time in seconds
     * \param startTime time offset. When to start rotation
     * \param axes the axes to rotate
     *
     * \return the rotation matrix at the current time.
     */
    Rotator( double time, double startTime, WVector3d axes, double degree, double speed ):
        Transformation()
    {
        m_duration = degree / speed;
        m_finishTime = startTime + m_duration;

        double rtime = positive( time - startTime ) * ( time - startTime );
        double rangle = smooth( speed * rtime, 0.0, degree );
        makeRotate( degToRad( rangle ), axes[0], axes[1], axes[2] );
    }
};

/**
 * Provides a comfortable zoomer lens.
 */
class Zoomer: public Transformation
{
public:
    /**
     * Zooms the scene with the given factor. This is a scaling on all axes. A factor < 1.0 is zooming out. A factor > 1.0 is zooming in.
     * Negative values are handled by their absolute value.
     *
     * \param time current time
     * \param startTime when to start the transformation
     * \param factor zooming factor.
     * \param speed the speed in zoom / second
     */
    Zoomer( double time, double startTime, double factor, double speed ):
        Transformation()
    {
        // get a scaling factor
        double zfactor = std::abs( factor );
        if( factor < 1.0 )
        {
            zfactor = 1.0 / factor;
        }
        zfactor -= 1.0;

        m_duration = zfactor / speed;
        m_finishTime = startTime + m_duration;

        double rtime = time - startTime;
        double sfactor = 1.0 + smooth( ( speed * rtime ), 0.0, zfactor );

        if( factor < 1.0 )
        {
            makeScale( 1.0 / sfactor, 1.0 / sfactor, 1.0 / sfactor );
        }
        else
        {
            makeScale( sfactor, sfactor, sfactor );
        }
    }
};

/**
 * Provides a time-dependent translation.
 */
class Translator: public Transformation
{
public:
    /**
     * Translates the scene using the given direction. The speed is given as units per second.
     *
     * \param time current time
     * \param startTime the time when to start the transformation
     * \param direction the direction. Length is important here.
     * \param speed speed in direction-vector per second
     */
    Translator( double time, double startTime, WVector3d direction, double speed ):
        Transformation()
    {
        m_duration = 1.0 / speed;
        m_finishTime = startTime + m_duration;

        double rtime = time - startTime;
        double scaler = smooth( speed * rtime, 0.0, 1.0 );
        makeTranslate( ( direction * scaler ).operator osg::Vec3d() );
    }
};

void WGEAnimationManipulator::handleFrame()
{
    // calculate the proper sec:frame coordinate:

    // time in seconds, it always relates to a 24 frames per second system
    double elapsed = m_timer->elapsed() - m_homeOffsetTime;

    // this brings the BBox to the center, makes it larger and rotates the front towards the camera
    osg::Matrixd mBBTranslate = osg::Matrixd::translate( -159.0 / 2.0, -199.0 / 2.0, -159.0 / 2.0 );
    osg::Matrixd mBBScale     = osg::Matrixd::scale( 1.5, 1.5, 1.5 );
    osg::Matrixd mBBRotate    = osg::Matrixd::rotate( -pi() / 2.0, 1.0, 0.0, 0.0 ) *
                                osg::Matrixd::rotate(  pi(), 0.0, 1.0, 0.0 );
    // Scene 1:
    // construct the animation here.
    Rotator rotateToBack =          Rotator( elapsed, 0.0, Transformation::axeY, 360.0, 22.5 );
    Rotator rotateToBottom =        Rotator( elapsed, rotateToBack.finish() - 3.0, -1.0 * Transformation::axeX, 15.0, 5.0 );
    Zoomer  zoomToIrgendwas =        Zoomer( elapsed, rotateToBottom.finish() - 2.0, 2.0, 0.25 );
    Translator translateABitUp = Translator( elapsed, rotateToBottom.finish() - 2.0, Transformation::axeY * 90.0, 0.25 );

    Zoomer  zoomToNei1 =              Zoomer( elapsed, translateABitUp.finish() - 2.0, 2.0, 0.15 );
    Zoomer  zoomToNaus =             Zoomer( elapsed, zoomToNei1.finish() + 1.0, 0.1, 3.0 );
    Zoomer  zoomToNei2 =              Zoomer( elapsed, zoomToNaus.finish(), 5.0, 1.5 );

    m_matrix = mBBTranslate * mBBScale * mBBRotate * rotateToBack
                                                   * rotateToBottom
                                                   * zoomToIrgendwas
                                                   * translateABitUp
                                                   * zoomToNei1
                                                   * zoomToNaus
                                                   * zoomToNei2;

    // Scene 2:
    // this brings the BBox to the center, makes it larger and rotates the front towards the camera
    // osg::Matrixd mBBTranslate = osg::Matrixd::translate( -159.0 / 2.0, -199.0 / 2.0, -179.0 / 2.0 );
    // osg::Matrixd mBBScale     = osg::Matrixd::scale( 2.0, 2.0, 2.0 );
    // osg::Matrixd mBBRotate    = osg::Matrixd::rotate( -pi() / 2.0, 1.0, 0.0, 0.0 ) *
    //                             osg::Matrixd::rotate(  pi(), 0.0, 1.0, 0.0 );
    // Transformation rotateToBack =          Rotator( elapsed, 0.0, Transformation::axeY, 360.0, 22.5 );
    // Transformation translateABitUp = Translator( elapsed, rotateToBack.finish() - 5.0, Transformation::axeY * -45.0, 0.25 );
    // Transformation zoomNei = Zoomer( elapsed,  rotateToBack.finish() - 5.0, 2.00, 0.25 );
    // Transformation rotateABit = Rotator( elapsed, zoomNei.finish() -1.0, Transformation::axeY, 360.0 + 45.0, 12.0 );
    //
    // m_matrix = mBBTranslate * mBBScale * mBBRotate * rotateToBack
    //                                                * translateABitUp
    //                                                * zoomNei
    //                                                * rotateABit;
}

