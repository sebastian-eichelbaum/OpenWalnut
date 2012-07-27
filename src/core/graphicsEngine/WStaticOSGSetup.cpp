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

///////////////////////////////////////////////////////////////////////////////
// Static OSG Setup
//
// When linking OpenSceneGraph statically, we are required to register the
// osgDB plugins and serializers manually. Please add all plugins and
// serializers you will need here.
//
// NOTE: If you add plugins, be careful to also add the dependent OSG libs in
//       out CMake files (osgFX and similar)
///////////////////////////////////////////////////////////////////////////////

#ifdef OSG_LIBRARY_STATIC

// NOTE: This code was originally taken from OSG's Android example.

//osg
#include <osg/GL>
#include <osg/GLExtensions>
#include <osg/Depth>
#include <osg/Program>
#include <osg/Shader>
#include <osg/Node>
#include <osg/Group>
#include <osg/Notify>
//osgText
#include <osgText/Text>
//osgDB
#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
//osg_viewer
#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osgViewer/Renderer>
#include <osgViewer/ViewerEventHandlers>
//osgGA
#include <osgGA/GUIEventAdapter>
#include <osgGA/MultiTouchTrackballManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/SphericalManipulator>

#include "WStaticOSGSetup.h"

//Static plugins Macro
// USE_OSGPLUGIN( ive );
// USE_OSGPLUGIN( osg );
// USE_OSGPLUGIN( osg2 );
// USE_OSGPLUGIN( terrain );
// USE_OSGPLUGIN( rgb );
// USE_OSGPLUGIN( OpenFlight );
// USE_OSGPLUGIN( dds );

//Static DOTOSG
USE_DOTOSGWRAPPER_LIBRARY( osg );
// USE_DOTOSGWRAPPER_LIBRARY( osgFX );
// USE_DOTOSGWRAPPER_LIBRARY( osgParticle );
// USE_DOTOSGWRAPPER_LIBRARY( osgTerrain );
// USE_DOTOSGWRAPPER_LIBRARY( osgText );
// USE_DOTOSGWRAPPER_LIBRARY( osgVolume );
// NOTE: although we need the osgViewer, uncommenting the below line causes an segfault (at least on Android).
// USE_DOTOSGWRAPPER_LIBRARY( osgViewer );

//Static serializer
USE_SERIALIZER_WRAPPER_LIBRARY( osg );
// USE_SERIALIZER_WRAPPER_LIBRARY( osgAnimation );
// USE_SERIALIZER_WRAPPER_LIBRARY( osgFX );
// USE_SERIALIZER_WRAPPER_LIBRARY( osgManipulator );
// USE_SERIALIZER_WRAPPER_LIBRARY( osgParticle );
// USE_SERIALIZER_WRAPPER_LIBRARY( osgTerrain );
USE_SERIALIZER_WRAPPER_LIBRARY( osgText );
// USE_SERIALIZER_WRAPPER_LIBRARY( osgVolume );

#endif // OSG_LIBRARY_STATIC
