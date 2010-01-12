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

#ifndef WPROGRESSCOMBINER_H
#define WPROGRESSCOMBINER_H

#include <set>
#include <string>

#include <boost/thread.hpp>

#include "WProgress.h"

/**
 * Base class for all kinds of progress combinations. You might want to derive from this class to implement some special progress
 * combination.
 */
class WProgressCombiner: public WProgress
{
public:

    /**
     * Default constructor. It creates a empty combiner.
     *
     * \param name the (optional) name of this progress.
     */
    explicit WProgressCombiner( std::string name = "" );

    /**
     * Destructor.
     */
    virtual ~WProgressCombiner();

    /**
     * Stops the progress. After finishing, the progress de-registers from its parent (if any).
     */
    virtual void finish();

    /**
     * Simple increment operator to signal a forward stepping.
     *
     * \note this actually is for ++p. p++ is not useful since it returns a copy of WProgress with the old count.
     *
     * \return the incremented WProgress instance.
     */
    virtual WProgressCombiner& operator++();

    /**
     * Returns the overall progress of this progress instance, including the child progress'.
     *
     * \return the progress.
     */
    virtual float getProgress();

    /**
     * Function updating the internal state. This needs to be called before any get function to ensure the getter return the right
     * values.
     */
    virtual void update();

protected:

    /**
     * The name of the combiner.
     */
    std::string m_name;

    /**
     * The current conglomerated progress. Set by update().
     */
    float m_progress;

    /**
     * Set of all child progress.
     */
    std::set< boost::shared_ptr< WProgress > > m_childs;

    /**
     * Lock for the above child set and the internal state update.
     */
    boost::shared_mutex m_updateLock;

private:
};

#endif  // WPROGRESSCOMBINER_H

