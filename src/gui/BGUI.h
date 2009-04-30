//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#ifndef BGUI_H
#define BGUI_H

/**
 * This class prescribes the interface to the GUI.
 * \ingroup gui
 */
class BGUI
{
public:
    virtual ~BGUI();

    /**
     * \brief
     * enable/disable the access to the GUI
     * \post
     * GUI enabled/disabled
     * \param enable true iff gui should be enabled
     */
    virtual void setEnabled( bool enable ) = 0;
};

/**
  \defgroup gui GUI

  \brief
  This module implements the graphical user interface for OpenWalnut.

*/

#endif  // BGUI_H
