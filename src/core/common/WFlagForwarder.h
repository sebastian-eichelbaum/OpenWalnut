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

#ifndef WFLAGFORWARDER_H
#define WFLAGFORWARDER_H

#include <boost/shared_ptr.hpp>
#include <boost/signals2/signal.hpp>

#include "WFlag.h"
#include "WExportCommon.h"

/**
 * This class helps especially container module programmers to easily synchronize the value of one flag with several other
 * flag. Assume the following scenario: you have a container module with two isosurface modules whose isovalue-properties
 * need to be in sync with the isovalue-property your container module provides to the outside world. Here, WFlagForwaderd
 * comes into play. Add the first isosurface's isovalue-property to the container modules m_properties list and forward it to
 * the other isovalue-property of the second isosurface module. Now they are in sync.
 * Be aware, that this is not a property itself!
 *
 * \note this class is not thread-safe for performance reasons. It is possible to add further flags to the forward-list but
 * be yourself aware that you might miss value changes if you add the flag right between two value changes of the source flag.
 *
 * \note Also be aware that this class only forwards changes in the flag value! Changes of "hidden" in PropertyVariables
 * are not propagated.
 *
 * \note The template parameter is the type encapsulated inside the flag. I.e. for WFlag< bool > use T=bool
 *
 * \param T the encapsulated type inside the flag. I.e. for WFlag< int32_t > use T=int32_t
 */
template < typename T >
class OWCOMMON_EXPORT WFlagForwarder // NOLINT
{
public:

    /**
     * Default constructor.
     *
     * \param source the property to be used as reference. All other properties will be synced with this one.
     */
    explicit WFlagForwarder( boost::shared_ptr< WFlag< T > > source );

    /**
     * Destructor.
     */
    virtual ~WFlagForwarder();

    /**
     * Forward the source property to the specified one. This ensures that the flag in "to" always has the value of the source flag.
     * There is no remove method.
     *
     * \param to the property to sync with source.
     */
    void forward( boost::shared_ptr< WFlag< T > > to );

protected:

    /**
     * The source property to which all other properties are synced to.
     */
    boost::shared_ptr< WFlag< T > > m_source;

    /**
     * The signal fired by m_source upon value change
     */
    boost::signals2::connection m_sourceConnection;

    /**
     * Signal forwarding the new value.
     */
    boost::signals2::signal< void( T ) > signal_forward;

    /**
     * This is a callback and gets called whenever the source property has changed.
     */
    void sourceChanged();

private:

    /**
     * Disallow copy construction.
     *
     * \param rhs the other forwarder.
     */
    WFlagForwarder( const WFlagForwarder& rhs );

    /**
     * Disallow copy assignment.
     *
     * \param rhs the other forwarder.
     * \return this.
     */
    WFlagForwarder& operator=( const WFlagForwarder& rhs );
};

template < typename T >
WFlagForwarder< T >::WFlagForwarder( boost::shared_ptr< WFlag< T > > source ):
    m_source( source )
{
    // connect the source's change signal
    m_sourceConnection = source->getValueChangeCondition()->subscribeSignal( boost::bind( &WFlagForwarder::sourceChanged, this ) );
}

template < typename T >
WFlagForwarder< T >::~WFlagForwarder()
{
    // cleanup (disconnect all)
    m_sourceConnection.disconnect();
    signal_forward.disconnect_all_slots();
}

template < typename T >
void WFlagForwarder< T >::forward( boost::shared_ptr< WFlag< T > >  to )
{
    to->set( m_source->get() );

    // NOTE: we do not need to store the signals2::connection here as the destructor disconnects ALL slots
    signal_forward.connect( boost::bind( &WFlag< T >::set, to.get(), _1, false ) );
}

template < typename T >
void WFlagForwarder< T >::sourceChanged()
{
    signal_forward( m_source->get() );
}

#endif  // WFLAGFORWARDER_H

