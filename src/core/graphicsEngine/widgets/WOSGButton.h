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


#ifndef WOSGBUTTON_H
#define WOSGBUTTON_H

#include <string>

#include <osgWidget/Box> //NOLINT

#include "../../common/WProperties.h"

#include "WOSGButtonLabel.h"


/**
 * Class implements an osgWidget::Box with a label that can be used as a button in the 3D scene
 */
class WOSGButton : public osgWidget::Box // NOLINT
{
public:
    /**
     * constructor
     *
     * \param name name of the button, will also be displayed on the label
     * \param type taken from the osgWidget::Box
     * \param resize_hint
     * \param pushable denotes if the button is pushable, i.e. keeps a pushed state or not
     */
    WOSGButton( std::string name, osgWidget::Box::BoxType type, bool resize_hint, bool pushable );

    /**
     * destructor
     */
    virtual ~WOSGButton();

    /**
     * setter for id
     *
     * \param id
     */
    void setId( size_t id );

    /**
     * getter
     *
     * \return id
     */
    size_t getId();

    /**
     * setter
     * \param label
     */
    void setLabel( std::string label );

    /**
     * getter
     * \return true when button was clicked
     */
    bool clicked();

    /**
     * getter
     * \return true when button was pushed
     */
    bool pushed();

    /**
     * setter
     * \param pushed if true the button is pushed
     */
    void setPushed( bool pushed );

    /**
     * setter for the background color of the label
     *
     * \param color the color
     */
    void setBackgroundColor( const WColor& color );


private:
    WOSGButtonLabel* m_label; //!< stores pointer to the label object

    size_t m_id; //!< id
};

inline bool WOSGButton::clicked()
{
    return m_label->clicked();
}

inline bool WOSGButton::pushed()
{
    return m_label->pushed();
}

inline void WOSGButton::setPushed( bool pushed )
{
    return m_label->setPushed( pushed );
}

inline size_t WOSGButton::getId()
{
    return m_id;
}

#endif  // WOSGBUTTON_H
