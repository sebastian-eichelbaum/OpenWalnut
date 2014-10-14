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

#ifndef WDATASETHISTOGRAM1D_H
#define WDATASETHISTOGRAM1D_H

#include <string>

#include "../common/WHistogramBasic.h"

#include "WDataSet.h"

/**
 * This data set type contains a 1D histogram.
 * \ingroup dataHandler
 */
class WDataSetHistogram1D : public WDataSet // NOLINT
{
public:
    /**
     * shared_ptr abbreviation
     */
    typedef boost::shared_ptr< WDataSetHistogram1D > SPtr;

    /**
     * const shared_ptr abbreviation
     */
    typedef boost::shared_ptr< const WDataSetHistogram1D > ConstSPtr;

    /**
     * Creates a dataset encapsulating a histogram.
     * The histogram will be copied in order to prevent changes to the data.
     *
     * \param histo The histogram to use.
     */
    explicit WDataSetHistogram1D( boost::shared_ptr< WHistogramBasic const > const& histo );

    /**
     * Construct an empty and unusable instance. This is needed for the prototype mechanism.
     */
    WDataSetHistogram1D();

    /**
     * Destroys this DataSet instance
     */
    virtual ~WDataSetHistogram1D();

    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual const std::string getName() const;

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual const std::string getDescription() const;

    /**
     * Returns the histogram.
     *
     * \return A const ref pointer to the histogram.
     */
    boost::shared_ptr< WHistogramBasic const > const& getHistogram() const;

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

protected:
    /**
     * The prototype as singleton.
     */
    static boost::shared_ptr< WPrototyped > m_prototype;

private:
    //! The histogram.
    boost::shared_ptr< WHistogramBasic const > const m_histogram;
};

#endif  // WDATASETHISTOGRAM1D_H

