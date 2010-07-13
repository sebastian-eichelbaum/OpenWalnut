/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
*
* This library is open source and may be redistributed and/or modified under
* the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
* (at your option) any later version.  The full license is in LICENSE file
* included with this distribution, and on the openscenegraph.org website.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* OpenSceneGraph Public License for more details.
*/

#ifndef WGESTATSHANDLER_H
#define WGESTATSHANDLER_H

#include <osg/AnimationPath>
#include <osgText/Text>
#include <osgGA/GUIEventHandler>
#include <osgGA/AnimationPathManipulator>

#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>

#include <osgDB/fstream>

/**
 * This class is an event handler managing render statistics. It basically is the osgViewer::StatsHandler but uses our font directory scheme.
 */
class WGEStatsHandler : public osgGA::GUIEventHandler
{
    public:

        WGEStatsHandler();

        enum StatsType
        {
            NO_STATS = 0,
            FRAME_RATE = 1,
            VIEWER_STATS = 2,
            CAMERA_SCENE_STATS = 3,
            VIEWER_SCENE_STATS = 4,
            LAST = 5
        };

        void setKeyEventTogglesOnScreenStats(int key) { _keyEventTogglesOnScreenStats = key; }
        int getKeyEventTogglesOnScreenStats() const { return _keyEventTogglesOnScreenStats; }

        void setKeyEventPrintsOutStats(int key) { _keyEventPrintsOutStats = key; }
        int getKeyEventPrintsOutStats() const { return _keyEventPrintsOutStats; }

        double getBlockMultiplier() const { return _blockMultiplier; }

        void reset();

        osg::Camera* getCamera() { return _camera.get(); }
        const osg::Camera* getCamera() const { return _camera.get(); }

        virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

        /** Get the keyboard and mouse usage of this manipulator.*/
        virtual void getUsage(osg::ApplicationUsage& usage) const;

    protected:

        void setUpHUDCamera(osgViewer::ViewerBase* viewer);

        osg::Geometry* createBackgroundRectangle(const osg::Vec3& pos, const float width, const float height, osg::Vec4& color);

        osg::Geometry* createGeometry(const osg::Vec3& pos, float height, const osg::Vec4& colour, unsigned int numBlocks);

        osg::Geometry* createFrameMarkers(const osg::Vec3& pos, float height, const osg::Vec4& colour, unsigned int numBlocks);

        osg::Geometry* createTick(const osg::Vec3& pos, float height, const osg::Vec4& colour, unsigned int numTicks);

        osg::Node* createCameraTimeStats(const std::string& font, osg::Vec3& pos, float startBlocks, bool acquireGPUStats, float characterSize, osg::Stats* viewerStats, osg::Camera* camera);

        void setUpScene(osgViewer::ViewerBase* viewer);

        void updateThreadingModelText();

        int                                 _keyEventTogglesOnScreenStats;
        int                                 _keyEventPrintsOutStats;

        int                                 _statsType;

        bool                                _initialized;
        osg::ref_ptr<osg::Camera>           _camera;

        osg::ref_ptr<osg::Switch>           _switch;

        osgViewer::ViewerBase::ThreadingModel          _threadingModel;
        osg::ref_ptr<osgText::Text>         _threadingModelText;

        unsigned int                        _frameRateChildNum;
        unsigned int                        _viewerChildNum;
        unsigned int                        _cameraSceneChildNum;
        unsigned int                        _viewerSceneChildNum;
        unsigned int                        _numBlocks;
        double                              _blockMultiplier;

        float                               _statsWidth;
        float                               _statsHeight;


};

#endif      // WGESTATSHANDLER_H
