//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2010 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
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

#include <iostream> // test() -> std::cout
#include <cmath> // test() -> log()

#include "../common/WAssert.h"

#include "WHistogram.h"

WHistogram::WHistogram() : uniformInterval(0)
{
    mappingPointer.first = mapping.begin();
    mappingPointer.second = 0;
}

WHistogram::WHistogram( unsigned int interval ) : uniformInterval(interval)
{
    mappingPointer.first = mapping.begin();
    mappingPointer.second = 0;
}

WHistogram::~WHistogram()
{
}

void WHistogram::add( double value )
{
    // IF(MIN || MAX needed) : here!
    elements[value]++;
}

void WHistogram::setUniformInterval( unsigned int value )
{
    // negativ ?
    //WAssert( elements.size > 0, "No elements in WHistogram." );
    uniformInterval = value;
    calculateMapping();
}

void WHistogram::calculateMapping()
{
    if( mapping.size() != 0 ) // is there already a mapping ?
    {
        mapping.clear();
    }

    unsigned int count = 0;
    unsigned int value = 0;
    for( std::map< double, WBar >::const_iterator iter = elements.begin(); iter != elements.end(); ++iter )
    {
        if( count == uniformInterval )
        {
            mapping.push_back( std::pair< const WBar*, unsigned int >( &iter->second, value ) );
            count = 0;
            value = iter->second.getValue();
        }
        else
        {
            value += iter->second.getValue();
            count++;
        }
    }
}

unsigned int WHistogram::operator[]( unsigned int position )
{
    if(mappingPointer.second == 0 || mappingPointer.second > position )
    {
        mappingPointer.first = mapping.begin();
        mappingPointer.second = 0;
    }

    for( unsigned int t = mappingPointer.second; t != position; ++t )
    {
        mappingPointer.first++;
    }
    mappingPointer.second = position; // == mappingPointer.second++
    return mappingPointer.first->second;
}

void WHistogram::setInterval( const std::list<unsigned int>& rangeList )
{
    if( mapping.size() != 0 ) // is there already a mapping ?
    {
        mapping.clear();
    }

    std::list<unsigned int>::const_iterator iter;
    for( iter = rangeList.begin(); iter != rangeList.end(); ++iter )
    {
        std::map< double, WBar >::const_iterator elementsIter = elements.begin();
        unsigned int barHeight = elementsIter->second.getValue();
        for( unsigned int i = 0; i != *iter; ++i )
        {
            elementsIter++;
            barHeight += elementsIter->second.getValue();
        }
        // elementsIter has to point at the first element of the next bucket
        elementsIter++;
        mapping.push_back( std::pair< const WBar*, unsigned int >( &elementsIter->second, barHeight ) );
    }
}

void WHistogram::test()
{
    calculateMapping();
    /*unsigned int rangeStart= 0;
    std::list< std:pair< WBar*, unsigned int > >::iterator iter;
    for(iter = mapping.begin(); iter != mapping.end(); iter++ )
    {
        std::cout << iter->second << ": " << rangeStart << " - " << rangeStart+uniformInterval;
        rangeStart += uniformInterval;
    }*/

    std::cout << mapping.size() << std::endl;
    std::list< std::pair< const WBar*, unsigned int > >::iterator it = mapping.begin();
    for(it = mapping.begin(); it != mapping.end(); ++it)
    {
        std::cout << it->second << " ";
    }
    std::cout << std::endl;

    for( unsigned int y = 10; y != 0; --y )
    {
        std::cout << "|";
        std::list< std::pair< const WBar*, unsigned int > >::iterator iter = mapping.begin();
        for(unsigned int x = 0; x != mapping.size(); ++x)
        {
            if( log((double)iter->second) > (double)(y-1)*2 )
            {
                std::cout << " #";
            } else {
                std::cout << "  ";
            }
            iter++;
        }
        std::cout << " |\n";
    }
    std::cout << "-" ;
    for(unsigned int i = 0; i != mapping.size(); ++i)
    {
        std::cout << "--";
    }
    std::cout << "--\n";
}

