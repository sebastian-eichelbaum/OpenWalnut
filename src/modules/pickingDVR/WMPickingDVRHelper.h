//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2015 A. Betz, D. Gerlicher, OpenWalnut Community, Nemtics, BSV@Uni-Leipzig
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

#ifndef WMPICKINGDVRHELPER_H
#define WMPICKINGDVRHELPER_H

/**
 * Color converter for different scalings, i.e. [0,1] vs. [0,255]
 */
template <typename T>
class WMPickingColor
{
private:
    T red; //!< Red color component value
    T green; //!< Green color component value
    T blue; //!< Blue color component value
    T alpha; //!< Opacity compnent value

public:
    /**
     * Standard constructor that creates transparent black.
     */
    WMPickingColor();

    /**
     * Constructor creating the color.
     *
     * \param red red color component value
     * \param green green color component value
     * \param blue blue color copmonent value
     * \param alpha opacity component value
     */
    WMPickingColor( T red, T green, T blue, T alpha );

    /**
     * Get red color component value.
     *
     * \return red value
     */
    T getRed();

    /**
     * Get green color component value.
     *
     * \return green value
     */
    T getGreen();

    /**
     * Get blue color component value.
     *
     * \return blue value
     */
    T getBlue();

    /**
     * Get opacity color component value.
     *
     * \return opacity value
     */
    T getAlpha();

    /**
     * Set red color component value.
     *
     * \param red red value
     */
    void setRed( T red );

    /**
     * Set green color component value.
     *
     * \param green green value
     */
    void setGreen( T green );

    /**
     * Set blue color component value.
     *
     * \param blue blue value
     */
    void setBlue( T blue );

    /**
     * Set opacity color component value.
     *
     * \param alpha opacity value
     */
    void setAlpha( T alpha );

    void normalize(); //!< Scales color down from [0,255] to [0,1]
};


template <typename T>
WMPickingColor<T>::WMPickingColor()
    : WMPickingColor( 0.0, 0.0, 0.0, 0.0 )
{
}

template <typename T>
WMPickingColor<T>::WMPickingColor( T red, T green, T blue, T alpha )
    : red( red ), green( green ), blue( blue ), alpha( alpha )
{
}

//Getter
template <typename T>
T WMPickingColor<T>::getRed()
{
    return this->red;
}

template <typename T>
T WMPickingColor<T>::getGreen()
{
    return this->green;
}

template <typename T>
T WMPickingColor<T>::getBlue()
{
    return this->blue;
}

template <typename T>
T WMPickingColor<T>::getAlpha()
{
    return this->alpha;
}

//Setter
template <typename T>
void WMPickingColor<T>::setRed( T red )
{
    this->red = red;
}

template <typename T>
void WMPickingColor<T>::setGreen( T green )
{
    this->green = green;
}

template <typename T>
void WMPickingColor<T>::setBlue( T blue )
{
    this->blue = blue;
}

template <typename T>
void WMPickingColor<T>::setAlpha( T alpha )
{
    this->alpha = alpha;
}

template <typename T>
void WMPickingColor<T>::normalize()
{
    this->red = this->red / 255.0;
    this->green = this->green / 255.0;
    this->blue = this->blue / 255.0;
    this->alpha = this->alpha / 255.0;
}


#endif  // WMPICKINGDVRHELPER_H
