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

#ifndef WQTINTERVALEDIT_H
#define WQTINTERVALEDIT_H

#include <math.h>
#include <limits>
#include <iostream>

#include <QtGui/QSlider>
#include <QtGui/QLineEdit>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>

template< typename DataType, typename DisplayType >
DisplayType toDisplayType( const DataType& value )
{
    return static_cast< DisplayType >( round( value ) );
}

/**
 * Base class for interval editing. You should not use this class. Use the derived \ref WQtIntervalEdit template.
 */
class WQtIntervalEditBase: public QWidget
{
    Q_OBJECT
public:
    /**
     * Default constructor.
     *
     * \param parent the parent widget
     */
    WQtIntervalEditBase( QWidget* parent );

    /**
     * Destructor.
     */
    virtual ~WQtIntervalEditBase();

    /**
     * Update the current state
     */
    virtual void update() = 0;

signals:
    /**
     * Called whenever the user changes the current minimum OR \ref setMin was called.
     */
    void minimumChanged();

    /**
     * Called whenever the user changes the current ,aximum OR \ref setMax was called.
     */
    void maximumChanged();

protected slots:

    /**
     * Slot called when the minimum slider changes.
     */
    virtual void minSliderChanged();

    /**
     * Slot called when the maximum slider changes.
     */
    virtual void maxSliderChanged();

    /**
     * Slot called when the minimum edit changes.
     */
    virtual void minEditChanged();

    /**
     * Slot called when the maximum edit changes.
     */
    virtual void maxEditChanged();

protected:
    /**
     * Resolution of the sliders.
     */
    const static int SliderResolution;

    /**
     * The slider for the minimum.
     */
    QSlider m_minSlider;

    /**
     * The slider for the maximumum.
     */
    QSlider m_maxSlider;

    /**
     * The minimum edit.
     */
    QLineEdit m_minEdit;

    /**
     * The maximum edit.
     */
    QLineEdit m_maxEdit;

    /**
     * A label for the minimum.
     */
    QLabel m_minLabel;

    /**
     * A label for the maximum.
     */
    QLabel m_maxLabel;

    /**
     * The layout containing min and max layout.
     */
    QGridLayout m_layout;

private:
};

/**
 * Simple widget allowing to define a certain interval.
 *
 * \tparam DataType interval for which integral type?
 */
template< typename DataType, typename DisplayType >
class WQtIntervalEdit: public WQtIntervalEditBase
{
public:
    /**
     * Default constructor.
     *
     * \param parent the parent widget
     */
    WQtIntervalEdit( QWidget* parent );

    /**
     * Destructor.
     */
    virtual ~WQtIntervalEdit();

    /**
     * Set the allowed minimum.
     *
     * \param min the min.
     */
    void setAllowedMin( DataType min = std::numeric_limits< DataType >::min() );

    /**
     * Set the allowed maximum.
     *
     * \param max the max
     */
    void setAllowedMax( DataType max = std::numeric_limits< DataType >::max() );

    /**
     * Get the currently allowed min.
     *
     * \return the min
     */
    const DataType& getAllowedMin() const;

    /**
     * Get the currently allowed max.
     *
     * \return the max
     */
    const DataType& getAllowedMax() const;

    /**
     * Set the currently selected min
     *
     * \param min the min
     */
    void setMin( DataType min = std::numeric_limits< DataType >::min() );

    /**
     * Set the currently selected max
     *
     * \param max the max
     */
    void setMax( DataType max = std::numeric_limits< DataType >::max() );

    /**
     * Get the currently selected minimum.
     *
     * \return the min
     */
    const DataType& getMin() const;

    /**
     * Get the currently selected maximum.
     *
     * \return the max
     */
    const DataType& getMax() const;

    /**
     * Update the current state
     */
    virtual void update();
protected:
    /**
     * Slot called when the minimum slider changes.
     */
    virtual void minSliderChanged();

    /**
     * Slot called when the maximum slider changes.
     */
    virtual void maxSliderChanged();

    /**
     * Slot called when the minimum edit changes.
     */
    virtual void minEditChanged();

    /**
     * Slot called when the maximum edit changes.
     */
    virtual void maxEditChanged();

private:
    /**
     * The allowed maximum
     */
    DataType m_allowedMax;

    /**
     * The allowed minimum
     */
    DataType m_allowedMin;

    /**
     * The current minumum.
     */
    DataType m_min;

    /**
     * The current maximum
     */
    DataType m_max;
};


template< typename DataType, typename DisplayType >
WQtIntervalEdit< DataType, DisplayType >::WQtIntervalEdit( QWidget* parent ):
    WQtIntervalEditBase( parent ),
    m_allowedMax( std::numeric_limits< DataType >::max() ),
    m_allowedMin( std::numeric_limits< DataType >::min() ),
    m_min( std::numeric_limits< DataType >::min() ),
    m_max( std::numeric_limits< DataType >::max() )

{
    // initialize members
}

template< typename DataType, typename DisplayType >
WQtIntervalEdit< DataType, DisplayType >::~WQtIntervalEdit()
{
    // cleanup
}

template< typename DataType, typename DisplayType >
void WQtIntervalEdit< DataType, DisplayType >::setAllowedMin( DataType min )
{
    if( ( min > m_allowedMax ) || ( m_allowedMin == min ) )
    {
        return;
    }

    // if the new allowed min is larger than the currently set min -> change m_min too
    if( min > m_min )
    {
        m_min = min;
        emit minimumChanged();
    }
    m_allowedMin = min;
    update();
}

template< typename DataType, typename DisplayType >
void WQtIntervalEdit< DataType, DisplayType >::setAllowedMax( DataType max )
{
    if( ( max < m_allowedMin ) || ( m_allowedMax == max ) )
    {
        return;
    }
    // if the new allowed max is smaller than the currently set max -> change m_max too
    if( max < m_max )
    {
        m_max = max;
        emit maximumChanged();
    }
    m_allowedMax = max;
    update();
}

template< typename DataType, typename DisplayType >
const DataType& WQtIntervalEdit< DataType, DisplayType >::getAllowedMin() const
{
    return m_allowedMin;
}

template< typename DataType, typename DisplayType >
const DataType& WQtIntervalEdit< DataType, DisplayType >::getAllowedMax() const
{
    return m_allowedMax;
}

template< typename DataType, typename DisplayType >
void WQtIntervalEdit< DataType, DisplayType >::setMin( DataType min )
{
    DataType newVal = ( min < m_allowedMin ) ? m_allowedMin : min;
    if( m_min == newVal )
    {
        return;
    }
    m_min = newVal;

    // ensure the maximum is larger than the minimum
    if( m_max < m_min )
    {
        m_max = m_min;
        emit maximumChanged();
    }
    emit minimumChanged();
    update();
}

template< typename DataType, typename DisplayType >
void WQtIntervalEdit< DataType, DisplayType >::setMax( DataType max )
{
    DataType newVal = ( max > m_allowedMax ) ? m_allowedMax : max;
    if( m_max == newVal )
    {
        return;
    }
    m_max = newVal;
    // ensure the minimum is smaller than the specified max
    if( m_max < m_min )
    {
        m_min = m_max;
        emit minimumChanged();
    }
    emit maximumChanged();
    update();
}

template< typename DataType, typename DisplayType >
const DataType& WQtIntervalEdit< DataType, DisplayType >::getMin() const
{
    return m_min;
}

template< typename DataType, typename DisplayType >
const DataType& WQtIntervalEdit< DataType, DisplayType >::getMax() const
{
    return m_max;
}

template< typename DataType, typename DisplayType >
void WQtIntervalEdit< DataType, DisplayType >::update()
{
    // set the new slider value
    double percMin = static_cast< double >( SliderResolution ) *
                     ( static_cast< double >( m_min ) - static_cast< double >( m_allowedMin ) ) /
                     ( static_cast< double >( m_allowedMax ) - static_cast< double >( m_allowedMin ) );
    double percMax = static_cast< double >( SliderResolution ) *
                     ( static_cast< double >( m_max ) - static_cast< double >( m_allowedMin ) ) /
                     ( static_cast< double >( m_allowedMax ) - static_cast< double >( m_allowedMin ) );

    m_minSlider.setValue( percMin );
    m_maxSlider.setValue( percMax );

    // set the text edits
    m_minEdit.setText( QString::number( m_min ) );
    m_maxEdit.setText( QString::number( m_max ) );
}

template< typename DataType, typename DisplayType >
void WQtIntervalEdit< DataType, DisplayType >::minSliderChanged()
{
    double perc = static_cast< double >( m_minSlider.value() ) / static_cast< double >( SliderResolution );
    DataType newMin = static_cast< DataType >(
                            static_cast< double >( m_allowedMin ) + perc * ( static_cast< double >( m_allowedMax ) -
                                                                             static_cast< double >( m_allowedMin ) ) );
    setMin( newMin );
}

template< typename DataType, typename DisplayType >
void WQtIntervalEdit< DataType, DisplayType >::maxSliderChanged()
{
    float perc = static_cast< float >( m_maxSlider.value() ) / static_cast< float >( SliderResolution );
    DataType newMax = static_cast< DataType >(
                            static_cast< double >( m_allowedMin ) + perc * ( static_cast< double >( m_allowedMax ) -
                                                                             static_cast< double >( m_allowedMin ) ) );

    setMax( newMax );
}

template< typename DataType, typename DisplayType >
void WQtIntervalEdit< DataType, DisplayType >::minEditChanged()
{
    // try to get the value
    bool valid;
    double value = m_minEdit.text().toDouble( &valid );
    if( !valid )
    {
        m_minEdit.setText( QString::number( static_cast< DataType >( m_min ) ) );
        return;
    }

    setMin( value );
}

template< typename DataType, typename DisplayType >
void WQtIntervalEdit< DataType, DisplayType >::maxEditChanged()
{
    // try to get the value
    bool valid;
    double value = m_maxEdit.text().toDouble( &valid );
    if( !valid )
    {
        m_maxEdit.setText( QString::number( static_cast< DataType >( m_max ) ) );
        return;
    }

    setMax( value );
}

#endif  // WQTINTERVALEDIT_H

