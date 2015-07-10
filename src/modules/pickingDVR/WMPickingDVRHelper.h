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


template <typename T>
class WMEDUColor
{
private:
    T red;
    T green;
    T blue;
    T alpha;

public:
    WMEDUColor();
    WMEDUColor( T red, T green, T fBlue, T alpha );

    //Getter
    T getRed();
    T getGreen();
    T getBlue();
    T getAlpha();

    //Setter
    void setRed( T red );
    void setGreen( T green );
    void setBlue( T blue );
    void setAlpha( T alpha );

    void normalize();
};


template <typename T>
WMEDUColor<T>::WMEDUColor()
    : WMEDUColor( 0.0, 0.0, 0.0, 0.0 )
{
}

template <typename T>
WMEDUColor<T>::WMEDUColor( T red, T green, T blue, T alpha )
    : red( red ), green( green ), blue( blue ), alpha( alpha )
{
}

//Getter
template <typename T>
T WMEDUColor<T>::getRed()
{
    return this->red;
}

template <typename T>
T WMEDUColor<T>::getGreen()
{
    return this->green;
}

template <typename T>
T WMEDUColor<T>::getBlue()
{
    return this->blue;
}

template <typename T>
T WMEDUColor<T>::getAlpha()
{
    return this->alpha;
}

//Setter
template <typename T>
void WMEDUColor<T>::setRed( T red )
{
    this->red = red;
}

template <typename T>
void WMEDUColor<T>::setGreen( T green )
{
    this->green = green;
}

template <typename T>
void WMEDUColor<T>::setBlue( T blue )
{
    this->blue = blue;
}

template <typename T>
void WMEDUColor<T>::setAlpha( T alpha )
{
    this->alpha = alpha;
}

template <typename T>
void WMEDUColor<T>::normalize()
{
    this->red = this->red / 255.0;
    this->green = this->green / 255.0;
    this->blue = this->blue / 255.0;
    this->alpha = this->alpha / 255.0;
}


#endif  // WMPICKINGDVRHELPER_H
