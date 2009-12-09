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

#ifndef WSTATUSREPORT_H
#define WSTATUSREPORT_H

#include <string>

/**
 * A counter for reporting the progress of some work. The work is represented
 * as steps, and hence the progress of the ratio of finished_steps / overall_steps.
 */
class WStatusReport
{
friend class WStatusReportTest;
public:
    /**
     * Constructs a new status reporter with the number of total steps
     * which it should reach to reach the 100 percent.
     *
     * \param totalSteps Number of total steps which symbolize 100 percent.
     */
    explicit WStatusReport( unsigned int totalSteps );

    /**
     * Returns the total number of steps which are considered as 100 percent
     * work.
     */
    unsigned int getTotalSteps() const;

    /**
     * Returns the number of steps done so far
     */
    unsigned int getFinishedSteps() const;

    /**
     * Returns the progress in percentage done so far: e.g. 0.7 == 70%
     */
    double progress() const;

    /**
     * Increments the finished work by one step.
     */
    WStatusReport& operator++();  // prefix increment

    /**
     * Increments the finished work by the number of the given steps.
     *
     * \param numSteps Number of steps to increment
     * \return The progress()
     */
    double operator+=( unsigned int numSteps );

    /**
     * Generates a string which represents the progress as bar of chars.
     *
     * \param symbol The char used for the progressbar indication
     * \param finalNumOfSymbols How many symbols represent the 100 percent
     */
    std::string stringBar( char symbol = '#', unsigned int finalNumOfSymbols = 50 ) const;

protected:
private:
    unsigned int m_totalSteps; //!< indication of the overall work
    unsigned int m_finishedSteps; //!< indicates the work done so far
};

#endif  // WSTATUSREPORT_H
