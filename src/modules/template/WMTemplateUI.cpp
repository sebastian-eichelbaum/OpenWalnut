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

#include <string>

#include <boost/bind.hpp>

#include "core/kernel/WKernel.h"
#include "core/graphicsEngine/WGERequirement.h"
#include "core/ui/WUI.h"

#include "WMTemplateUI.h"

WMTemplateUI::WMTemplateUI()
    : WModule()
{
}

WMTemplateUI::~WMTemplateUI()
{
}

boost::shared_ptr< WModule > WMTemplateUI::factory() const
{
    return boost::shared_ptr< WModule >( new WMTemplateUI() );
}

const std::string WMTemplateUI::getName() const
{
    return "Template UI";
}

const std::string WMTemplateUI::getDescription() const
{
    return "Show some custom module UI.";
}

void WMTemplateUI::connectors()
{
    // We do not need any connectors. Have a look at WMTemplate.cpp if you want to know what this means.
    WModule::connectors();
}

void WMTemplateUI::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    WModule::properties();
}

void WMTemplateUI::requirements()
{
    // we need graphics to draw anything
    m_requirements.push_back( new WGERequirement() );
    m_requirements.push_back( new WUIRequirement() );
}

void WMTemplateUI::handleMouseMove( WVector2f pos )
{
    debugLog() << "MOUSE MOVED: " << pos;
}

void WMTemplateUI::handleResize( int /* x */, int /* y */, int width, int height )
{
    debugLog() << "RESIZE: " << width << ", " << height;
}

void WMTemplateUI::moduleMain()
{
/*    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );

    for( std::size_t k = 0; k < m_input.size(); ++k )
    {
        m_moduleState.add( m_input[ k ]->getDataChangedCondition() );
    }

    m_instanceID = ++m_instanceCounter;

    // resize data vector to the number of input connectors
    m_data.resize( m_input.size() );

    ready();

    //! Holds the reference to the custom widget used for displaying the histogram
    WUIGridWidget::SPtr m_widgetGrid = WKernel::getRunningKernel()->getUI()->getWidgetFactory()->createGridWidget( getName() );
    WUITabbedWidget::SPtr m_tab =  WKernel::getRunningKernel()->getUI()->getWidgetFactory()->createTabbedWidget( "tabs"+ getName(), m_widgetGrid );

    m_widget = WKernel::getRunningKernel()->getUI()->getWidgetFactory()->createViewWidget(
            getName() + string_utils::toString( m_instanceID ),
            WGECamera::TWO_D, m_shutdownFlag.getValueChangeCondition(), m_tab );
//            WGECamera::TWO_D, m_shutdownFlag.getValueChangeCondition() );


    WUIViewWidget::SPtr m_widgetTab2 = WKernel::getRunningKernel()->getUI()->getWidgetFactory()->createViewWidget(
            getName() + "hey" + string_utils::toString( m_instanceID ),
            WGECamera::TWO_D, m_shutdownFlag.getValueChangeCondition(), m_tab );

    osg::ref_ptr< WUIViewEventHandler > eh = new WUIViewEventHandler( m_widget );
    eh->subscribeMove( boost::bind( &WMTemplateUI::handleMouseMove, this, _1 ) );
    eh->subscribeResize( boost::bind( &WMTemplateUI::handleResize, this, _1, _2, _3, _4 ) );
    m_widget->addEventHandler( eh );

       //! Holds the reference to the custom widget used for displaying the histogram
    WUIGridWidget::SPtr m_widgetGrid2 = WKernel::getRunningKernel()->getUI()->getWidgetFactory()->createGridWidget( "23"+ getName(), m_widgetGrid );

    WUIViewWidget::SPtr m_widget2 = WKernel::getRunningKernel()->getUI()->getWidgetFactory()->createViewWidget(
            getName() + "kk" + string_utils::toString( m_instanceID ),
            WGECamera::TWO_D, m_shutdownFlag.getValueChangeCondition(), m_widgetGrid2 );

    WUIViewWidget::SPtr m_widget3 = WKernel::getRunningKernel()->getUI()->getWidgetFactory()->createViewWidget(
            getName() + "kk2" + string_utils::toString( m_instanceID ),
            WGECamera::TWO_D, m_shutdownFlag.getValueChangeCondition(), m_widgetGrid2 );


    WUIPropertyGroupWidget::SPtr m_widgetProps = WKernel::getRunningKernel()->getUI()->getWidgetFactory()->createPropertyGroupWidget( "Propsy",
            m_properties, m_widgetGrid2 );

    debugLog() << "huhu 1" << m_tab->addTab( m_widget, "Widget 1" );
    debugLog() << "huhu 2" << m_tab->addTab( m_widgetTab2, "Widget 2" );

    m_widgetGrid->placeWidget( m_tab, 0, 0 );
    m_widgetGrid->placeWidget( m_widgetGrid2, 0, 1 );

    m_widgetGrid->setColumnStretch( 0, 2 );
    m_widgetGrid->setColumnStretch( 1, 1 );

    m_widgetGrid2->placeWidget( m_widget2, 0, 0 );
    m_widgetGrid2->placeWidget( m_widget3, 1, 0 );
    m_widgetGrid2->placeWidget( m_widgetProps, 2, 0 );

    m_widgetGrid->show();

    if( m_widget )
    {
        // window width and height
        m_windowWidth = m_widget->width();
        m_windowHeight = m_widget->height();

        m_mainNode = m_widget->getScene();
        if( !m_mainNode )
        {
            errorLog() << "Could not acquire scene node from widget.";
        }
    }
    else
    {
        errorLog() << "Could not create widget for the histogram.";
    }

    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";

        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        // if we do not have a main node, there is no point in doing anything
        if( m_mainNode )
        {
            m_redrawMutex.lock();

            bool dataChanged = false;
            bool hasData = false;
            for( std::size_t k = 0; k < m_data.size(); ++k )
            {
                dataChanged = dataChanged || ( m_input[ k ]->getData() && m_data[ k ] != m_input[ k ]->getData() );
                hasData = hasData || ( m_input[ k ]->getData() || m_data[ k ] );
            }

            if( !hasData )
            {
                continue;
            }

            bool colorChanged = false;
            for( std::size_t k = 0; k < m_colors.size(); ++k )
            {
                colorChanged = colorChanged | m_colors[ k ]->changed();
            }

            if( dataChanged || colorChanged || m_histoBins->changed() || m_styleSelection->changed() )
            {
                infoLog() << "Recalculating histogram.";

                // get current data
                for( std::size_t k = 0; k < m_data.size(); ++k )
                {
                    m_data[ k ] = m_input[ k ]->getData();
                }

                if( dataChanged || m_histoBins->changed() )
                {
                    calculateHistograms();
                }

                // remove all child nodes from main node
                m_mainNode->clear();
                redraw();
            }

            m_redrawMutex.unlock();
        }
    }

    debugLog() << "Shutting down...";

    // clear main node, just in case
    if( m_mainNode )
    {
        m_mainNode->clear();
    }

    m_widgetGrid->close();

    debugLog() << "Finished. Good bye!";*/
}

