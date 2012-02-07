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

#ifndef WTERMINALCOLOR_H
#define WTERMINALCOLOR_H

#include <string>



/**
 * Helper class to provide a convenient way to colorize output on the console.
 */
class WTerminalColor // NOLINT
{
friend class WTerminalColorTest;
public:
    /**
     * Define possible attributes.
     */
    enum TerminalColorAttribute
    {
        Off         = 0,
        Bold        = 1,
        Underscore  = 4,
        Blink       = 5,
        Reverse     = 7,
        Concealed   = 8,
        Default     = 9         // this actually disables coloring
    };

    /**
     * Foreground colors.
     */
    enum TerminalColorForeground
    {
        FGBlack   = 30,
        FGRed     = 31,
        FGGreen   = 32,
        FGYellow  = 33,
        FGBlue    = 34,
        FGMagenta = 35,
        FGCyan    = 36,
        FGWhite   = 37
    };

    /**
     * Background colors.
     */
    enum TerminalColorBackground
    {
        BGNone    = 50,
        BGBlack   = 40,
        BGRed     = 41,
        BGGreen   = 42,
        BGYellow  = 43,
        BGBlue    = 44,
        BGMagenta = 45,
        BGCyan    = 46,
        BGWhite   = 47
    };

    /**
     * Constructor to create a color code which actually does not do any coloring.
     */
    WTerminalColor();

    /**
     * Creates a new terminal color.
     *
     * \param attrib attribute, like bold or blink
     * \param foreground foreground color
     * \param background background color
     */
    WTerminalColor( TerminalColorAttribute attrib, TerminalColorForeground foreground, TerminalColorBackground background = BGNone );

    /**
     * Destructor.
     */
    virtual ~WTerminalColor();

    /**
     * Gives the control string which actually enables the color.
     *
     * \param ostr the stream to extend by the color code.
     *
     * \return the color control string
     */
    std::ostream& operator<<( std::ostream& ostr ) const;

    /**
     * Gives the control string which actually enables the color.
     *
     * \return the color control string
     */
    std::string operator()() const;

    /**
     * Colorizes the given string and resets color after it.
     *
     * \param s the string to colorize
     *
     * \return the string including control sequences.
     */
    std::string operator()( const std::string s ) const;

    /**
     * Combines strings.
     *
     * \param istr the string to combine
     *
     * \return the concatenated string.
     */
    std::string operator+( const std::string& istr ) const;

    /**
     * Resets the color and returns control string.
     *
     * \return the control string which resets color settings.
     */
    std::string operator!() const;

    /**
     * With this you can easily trigger whether the color control string is used or if "" is returned.
     *
     * \param enabled true to have colors.
     */
    void setEnabled( bool enabled );

    /**
     * Is coloring enabled?
     *
     * \return true if enabled
     */
    bool isEnabled() const;

protected:
    /**
     * The string actually containing the control sequence to enable colors on the console.
     */
    std::string m_colorString;

    /**
     * Control sequence to reset color.
     */
    std::string m_colorResetString;

    /**
     * Color attributes.
     */
    TerminalColorAttribute m_attrib;

    /**
     * The foreground color.
     */
    TerminalColorForeground m_foreground;

    /**
     * The background color.
     */
    TerminalColorBackground m_background;

private:
    /**
     * Actually generates the control sequences.
     */
    void generateControlStrings();

    /**
     * True when colors should are used.
     */
    bool m_enabled;
};

#endif  // WTERMINALCOLOR_H

