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

#ifndef WOSGBUTTONLABEL_H
#define WOSGBUTTONLABEL_H

#include <osgWidget/Label>
#include <osgWidget/Util>
#include <osgWidget/WindowManager>

/**
 * implements mouse interaction for a osgWidget label
 */
class WOSGButtonLabel : public osgWidget::Label
{
public:
    /**
     * constructor
     * \param pushable denotes if the button is pushable, i.e. keeps a pushed state or not
     */
    explicit WOSGButtonLabel( bool pushable );

    /**
     * destructor
     */
    ~WOSGButtonLabel();

    /**
     * function handles mouse click on label, the function was copied from an osg example, as everything in
     * the osg it wasn't documented, so it's not entirely sure what the params are for and what they do
     *
     * \return bool
     */
    bool mousePush( double, double, osgWidget::WindowManager* );

    /**
     * getter for clicked flag, resets the flag to false
     *
     * \return true if the label has been clicked
     */
    bool clicked();

    /**
     * getter for pushed flag
     *
     * \return true if the button is pushed
     */
    bool pushed();

    /**
     * setter
     * \param pushed if true the button is pushed
     */
    void setPushed( bool pushed );

protected:
private:
    bool m_clicked; //!< if true the label has been clicked since it was last checked for

    bool m_pushable; //!< button is pushable or not

    bool m_pushed; //!< true if button is pushable and was clicked
};

inline bool WOSGButtonLabel::clicked()
{
    bool tmp = m_clicked;
    m_clicked = false;
    return tmp;
}

inline bool WOSGButtonLabel::pushed()
{
    return m_pushed;
}

inline void WOSGButtonLabel::setPushed( bool pushed )
{
    m_pushed = pushed;
}

#endif  // WOSGBUTTONLABEL_H
