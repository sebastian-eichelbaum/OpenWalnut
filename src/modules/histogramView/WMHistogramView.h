//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WMHISTOGRAMVIEW_H
#define WMHISTOGRAMVIEW_H

#include <string>
#include <vector>

#include <boost/thread.hpp>
#include <osg/Geode>

#include "core/common/math/linearAlgebra/WMatrixFixed.h"
#include "core/common/WHistogramBasic.h"

#include "core/dataHandler/WDataSetScalar.h"

#include "core/kernel/WModule.h"
#include "core/kernel/WModuleInputData.h"
#include "core/kernel/WModuleOutputData.h"

/**
 * \class InstanceCounter
 *
 * A simple threadsafe non-copyable counter.
 */
class InstanceCounter
{
public:
    /**
     * Constructor.
     */
    InstanceCounter()
        : m_counterMutex(),
          m_counter( 0 )
    {
    }

    /**
     * Increase count by 1 and return the new counter value. This is threadsafe.
     *
     * \return The new counter value.
     */
    int operator++ ()
    {
        boost::unique_lock< boost::mutex > lock( m_counterMutex );
        return ++m_counter;
    }

    /**
     * Decrease count by 1 and return the new counter value. This is threadsafe.
     *
     * \return The new counter value.
     */
    int operator-- ()
    {
        boost::unique_lock< boost::mutex > lock( m_counterMutex );
        return --m_counter;
    }

private:
    //! No copy construction.
    InstanceCounter( InstanceCounter& /* count */ );

    /**
     * No copy operator.
     *
     * \return Nothing.
     */
    InstanceCounter operator= ( InstanceCounter& /* count */ );

    //! A mutex to protect the counter from concurrent updates.
    boost::mutex m_counterMutex;

    //! The counter.
    int m_counter;
};


// TODO(reichenbach): choose color of frame and marker depending on the user's chosen window background color
/**
 * \class WMHistogramView
 *
 * A module that draws a histogram of one or mode scalar datasets in a custom widget.
 *
 * \ingroup modules
 */
class WMHistogramView : public WModule
{
public:
    /**
     * Constuctor.
     */
    WMHistogramView();

    /**
     * Destructor.
     */
    virtual ~WMHistogramView();

    /**
     * Gives back the name of this module.
     * \return the module's name.
     */
    virtual const std::string getName() const;

    /**
     * Gives back a description of this module.
     * \return description to module.
     */
    virtual const std::string getDescription() const;

    /**
     * Due to the prototype design pattern used to build modules, this method returns a new instance of this method. NOTE: it
     * should never be initialized or modified in some other way. A simple new instance is required.
     *
     * \return the prototype used to create every module in OpenWalnut.
     */
    virtual boost::shared_ptr< WModule > factory() const;

    /**
     * Get the icon for this module in XPM format.
     *
     * \return The icon.
     */
    virtual const char** getXPMIcon() const;

protected:
    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void moduleMain();

    /**
     * Initialize the connectors this module is using.
     */
    virtual void connectors();

    /**
     * Initialize the properties for this module.
     */
    virtual void properties();

    /**
     * Initialize requirements for this module.
     */
    virtual void requirements();

private:
    /**
     * Called on every mouse move event from the custom widget.
     *
     * \note this runs in OSG thread.
     * \param pos New mouse position.
     */
    void handleMouseMove( WVector2f pos );

    /**
     * Called on every resize event from the custom widget.
     *
     * \note this runs in OSG thread.
     * \param x X pos
     * \param y Y pos
     * \param width Widht
     * \param height Height
     */
    void handleResize( int x, int y, int width, int height );

    /**
     * Whenever a new info node is made this mutex should be used.
     */
    boost::mutex m_createInfoMutex;

    /**
     * Whenever a redraw is made this mutex should be used.
     */
    boost::mutex m_redrawMutex;

    /**
     * Redraws the histogram and add it to the main geode.
     */
    void redraw();

    /**
     * This updates the maximum value of the histograms.
     *
     * \param cumulative If true, histogram values of the datasets will be added for each bin.
     */
    void updateHistogramMax( bool cumulative );

    /**
     * Finds a good size for the frame, depending on the chosen spacing for axis labels.
     */
    void calculateFrameSize();

    /**
     * Finds a good position of the frame relative to the lower left corner of the window.
     * Depends on the current frame size and the size of the text to be written left to/under the
     * frame.
     */
    void calculateFramePosition();

    /**
     * This simply calculates a histogram per dataset, where the bin sizes and positions are the same for
     * each histogram.
     */
    void calculateHistograms();

    /**
     * Creates the geometry for histogram bars. The type parameter defines how the bars are drawn:
     *
     * 1 - transparent bars on top of each other
     * 2 - bars of a bin are drawn next to each other
     * 3 - stacked bars
     *
     * \param type The type of the bars.
     */
    void createGeometryBars( int type );

    /**
     * Creates the geometry for stairs (i.e. bars that are not filled).
     * The type parameter can be 1 (normal) or 2 (stacked).
     *
     * \param type The type of the stairs.
     */
    void createGeometryStairs( int type );

    /**
     * Creates the geometry for curves.
     * The type parameter can be 1 (normal) or 2 (stacked).
     *
     * \param type The type of the curves.
     */
    void createGeometryCurves( int type );

    /**
     * Creates the geometry for the frame and the ticks/labels.
     */
    void createFrame();

    /**
     * Writes the values of the currently selected histogram bin to the top right
     * corner of the window. The selected bin is the one pointed to by the mouse cursor.
     *
     * Also adds a quad denoting the currently selected bin that is drawn behind the geometry.
     *
     * \param mousePos Current mouse position
     */
    void createInfo( WVector2f mousePos );

    /**
     * This finds a suitable spacing of ticks to use for an axis with a certain length and
     * value interval.
     *
     * \param intervalLength The length of the value interval to represent.
     * \param availableSpace The space available (in pixels on screen) for the interval.
     * \param textSize The estimated size of the text for every tick.
     *
     * \return A good distance between ticks to use for labeling an axis.
     */
    double findOptimalSpacing( double intervalLength, double availableSpace, double textSize );

    /**
     * This transforms histogram space coordinates to window coordinates. Histogram coordinates
     * range from the minimum to the maximum values of all histograms in x-direction and from
     * 0 to the maximum bin value over all histograms (or the maximum bin value where the bins of all
     * datasets are added; this is for cumulative histogram types).
     *
     * \note The frame size and position must be already calculated for this function to give meaningful results.
     *
     * \param v The input in histogram coords.
     * \return The given position in window coords.
     */
    WVector2d histogramSpaceToWindowSpace( WVector2d const& v );

    /**
     * This is the inverse of histogramSpaceToWindowSpace.
     * \see histogramSpaceToWindowSpace
     *
     * \note The frame size and position must be already calculated for this function to give meaningful results.
     *
     * \param v The input in window coords.
     * \return The given position in histogram coords.
     */
    WVector2d windowSpaceToHistogramSpace( WVector2d const& v );

    /**
     * This simply prints a NYI message to the errorLog.
     */
    void createNothing();

    /**
     * A vector of histograms, one histogram per input. Histograms may be empty if an input
     * does not have a valid dataset.
     */
    std::vector< boost::shared_ptr< WHistogramBasic > > m_histograms;

    //! A condition for property updates.
    boost::shared_ptr< WCondition > m_propCondition;

    //! The width of the window.
    int m_windowWidth;

    //! The height of the window.
    int m_windowHeight;

    //! The position of the mouse cursor in window coordinates.
    WVector2d m_mousePos;

    //! Draws histogram bin info to the top right corner of the window.
    osg::ref_ptr< osg::Geode > m_infoNode;

    //! Draws a marker showing the currently selected histogram bin.
    osg::ref_ptr< osg::Geode > m_markerNode;

    //! Holds the reference to the custom widget used for displaying the histogram
    WUIView::SPtr m_widget;

    //! Draws the frame and ticks/labels.
    osg::ref_ptr< osg::Geode > m_frameNode;

    //! The lower left corner of the histogram in histogram coordinates.
    WVector2d m_histogramLowerLeft;

    //! The upper right corner of the histogram in histogram coordinates.
    WVector2d m_histogramUpperRight;

    //! The lower left vertex of the frame box in histogram coordinates.
    WVector2d m_frameLowerLeft;

    //! The upper right vertex of the frame box in histogram coordinates.
    WVector2d m_frameUpperRight;

    //! The spacing between labels at the histogram axis in histogram coordinates.
    WVector2d m_frameSpacing;

    //! The space to the left and under the frame in window coordinates.
    WVector2d m_framePosition;

    //! The distance between the histogram frame and the top resp. right side of the window in relative window coordinates.
    double const m_frameSize;

    //! The number of this WMHistogram instance. Used to generate a unique window title for every instance of this module.
    int m_instanceID;

    //! The instance counter used to get the instance ID.
    static InstanceCounter m_instanceCounter;

    //! The scene node of the custom window. All geometry nodes are added as children of this node.
    osg::ref_ptr< WGEGroupNode > m_mainNode;

    // the next 3 vectors all have the same size, which is the maximum number of inputs allowed for this module
    // see NUM_INPUTS in the .cpp
    //! A vector of input connectors.
    std::vector< boost::shared_ptr< WModuleInputData< WDataSetScalar > > > m_input;

    //! A vector of current datasets.
    std::vector< boost::shared_ptr< WDataSetScalar > > m_data;

    //! A vector of color properties for the datasets.
    std::vector< WPropColor > m_colors;

    //! A property that is used to set the number of bins to use.
    WPropInt m_histoBins;

    // the next two implement a kind of strategy pattern; the index of the selected element
    // is used as an index to the geometry functions vector
    //! Allows to select which one of the geometry generation functions should be used.
    WPropSelection m_styleSelection;

    //! A vector containing functions to use for histogram geometry generation.
    std::vector< boost::function< void( void ) > > m_geometryFunctions;
};

#endif  // WMHISTOGRAMVIEW_H
