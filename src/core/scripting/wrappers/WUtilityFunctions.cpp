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

#include <string>
#include <osgGA/TrackballManipulator>

#include "../../common/WLogger.h"

#include "../../graphicsEngine/WGraphicsEngine.h"

#include "../../kernel/WKernel.h"

#include "WUtilityFunctions.h"

void screenshot()
{
    if( WKernel::getRunningKernel()->getGraphicsEngine() )
    {
        WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getScreenCapture()->screenshot();
    }
    else
    {
        wlog::error( "Script" ) << "No graphics engine! Cannot make screenshot!";
    }
}

void initCamera( std::string const& view )
{
    if( WKernel::getRunningKernel()->getGraphicsEngine() )
    {
        osg::ref_ptr< osgGA::TrackballManipulator > cm = osg::dynamic_pointer_cast< osgGA::TrackballManipulator >(
            WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCameraManipulator() );
        if( cm )
        {
            osg::Quat q;

            if( view == "anterior" )
            {
                q = osg::Quat( 0., -0.707107, -0.707107, 0. );
            }
            else if( view == "posterior" )
            {
                q = osg::Quat( 0.707107, 0., 0., 0.707107 );
            }
            else if( view == "left" )
            {
                q = osg::Quat( 0.5, -0.5, -0.5, 0.5 );
            }
            else if( view == "right" )
            {
                q = osg::Quat( -0.5, -0.5, -0.5, -0.5 );
            }
            else if( view == "superior" )
            {
                q = osg::Quat( 0., 0., 0., 1 );
            }
            else if( view == "inferior" )
            {
                q = osg::Quat( 0., -1., 0., 0. );
            }
            else
            {
                q = osg::Quat( 0., 0., 0., 1 );

                wlog::warn( "Script" ) << "Unknown preset: " << view << "!";
            }

            cm->setRotation( q );
        }
        else
        {
            wlog::warn( "Script" ) << "GL Widget does not use a TrackballManipulator. Preset cannot be used.";
        }
    }
    else
    {
        wlog::error( "Script" ) << "No graphics engine! Cannot set camera preset!";
    }
}
