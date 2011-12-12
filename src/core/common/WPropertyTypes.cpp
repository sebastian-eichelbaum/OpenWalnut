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

#include <string>

#include "WPropertyTypes.h"
#include "WTransferFunction.h"

std::ostream& WPVBaseTypes::operator<<( std::ostream& out, const WPVBaseTypes::PV_TRIGGER& c )
{
    // print it as nice string
    switch( c )
    {
        case PV_TRIGGER_TRIGGERED:
            out << "PV_TRIGGER_TRIGGERED";
            break;
        default:
            out << "PV_TRIGGER_READY";
    }

    return out;
}

std::istream& WPVBaseTypes::operator>>( std::istream& in, WPVBaseTypes::PV_TRIGGER& c )
{
    std::string s;
    in >> s;

    // interpret string
    if( s == "PV_TRIGGER_TRIGGERED" )
    {
        c = PV_TRIGGER_TRIGGERED;
    }
    else
    {
        c = PV_TRIGGER_READY;
    }

    return in;
}

bool WPVBaseTypes::isPropertyGroup( PROPERTY_TYPE  type )
{
    switch( type )
    {
    case PV_GROUP:
    case PV_STRUCT:
        return true;
    default:
        return false;
    }
}

namespace PROPERTY_TYPE_HELPER
{
    WPVBaseTypes::PV_TRANSFERFUNCTION WStringConversion< WPVBaseTypes::PV_TRANSFERFUNCTION >::create(
            const WPVBaseTypes::PV_TRANSFERFUNCTION& /*old*/, const std::string str )
    {
        WTransferFunction tf;
        std::vector< std::string > tokens;
        tokens = string_utils::tokenize( str, ";" );
        //WAssert( tokens.size() >= 16, "There weren't 16 values for a 4x4 Matrix" );
        size_t idx = 0;
        while ( idx < tokens.size() )
        {
            std::vector< std::string > innerTokens;
            innerTokens = string_utils::tokenize( tokens[ idx ], ":" );
            // evaluate inner tokens
            {
                if ( innerTokens[ 0 ] == "c" )
                {
                    tf.addColor( boost::lexical_cast< double >( innerTokens[ 1 ].c_str() ), // isovalue
                            WColor( boost::lexical_cast< double >( innerTokens[ 2 ].c_str() ), // red
                                boost::lexical_cast< double >( innerTokens[ 3 ].c_str() ), // green
                                boost::lexical_cast< double >( innerTokens[ 4 ].c_str() ), // blue
                                1. ) );  // blue
                }
                else if ( innerTokens[ 0 ] == "a" )
                {
                    tf.addAlpha( boost::lexical_cast< double >( innerTokens[ 1 ].c_str() ),
                            boost::lexical_cast< double >( innerTokens[ 2 ].c_str() ) );
                }
                idx++;
            }
        }
        return tf;
    }

    std::string WStringConversion< WPVBaseTypes::PV_TRANSFERFUNCTION >::asString( const WPVBaseTypes::PV_TRANSFERFUNCTION& tf )
    {
        std::ostringstream out;
        size_t numColors = tf.numColors();
        for ( size_t i = 0; i < numColors; ++i )
        {
            double iso = tf.getColorIsovalue( i );
            WColor c = tf.getColor( i );
            out << "c:" << iso << ":" << c[ 0 ] << ":" << c[ 1 ] << ":" << c[ 2 ] << ";";
        }
        size_t numAlphas = tf.numAlphas();
        for ( size_t i = 0; i < numAlphas; ++i )
        {
            double iso = tf.getAlphaIsovalue( i );
            double alpha = tf.getAlpha( i );
            out << "a:" << iso << ":" << alpha;
            if ( i != numAlphas-1 )
            {
                out << ";";
            }
        }
        return out.str();
    }
}
