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

#include <algorithm>
#include <string>

#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>

#include "core/common/WLogger.h"
#include "core/common/WStringUtils.h"
#include "core/kernel/WProjectFile.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleContainer.h"
#include "core/kernel/WModule.h"

#include "WQtNetworkEditor.h"
#include "WQtNetworkItemGrid.h"
#include "WQtNetworkEditorProjectFileIO.h"

WQtNetworkEditorProjectFileIO::WQtNetworkEditorProjectFileIO( WQtNetworkEditor* ne ):
    WProjectFileIO(),
    m_networkEditor( ne ),
    m_networkScale( 1.0 )
{
    // initialize members
    // We want to set the layout before actually starting the network.
    setApplyOrder( PRE_MODULES );
}

WQtNetworkEditorProjectFileIO::~WQtNetworkEditorProjectFileIO()
{
    // cleanup
}

WProjectFileIO::SPtr WQtNetworkEditorProjectFileIO::clone( WProjectFile* project ) const
{
    // Nothing special. Simply create new instance.
    WProjectFileIO::SPtr p( new WQtNetworkEditorProjectFileIO( m_networkEditor ) );
    p->setProject( project );
    return p;
}

/**
 * Interpret a string as a ";"- separated sequence of doubles.
 *
 * \param seq   the sequence
 * \param size  the number of ints needed
 * \param loaded will contain the number of ints really loaded.
 *
 * \return the values
 */
int* parseIntSequence( std::string seq, int size, int& loaded ) // NOLINT: yes it is a non const ref.
{
    // parse the string
    // -> tokenize it and fill pointer appropriately
    typedef boost::tokenizer<boost::char_separator< char > > tokenizer;
    boost::char_separator< char > sep( ";" );   // separate by ;
    tokenizer tok( seq, sep );

    // each value must be stored at the proper position
    int* values = new int[ size ];
    int i = 0;
    loaded = 0;
    for( tokenizer::iterator it = tok.begin(); ( it != tok.end() ) && ( i < size ); ++it )
    {
        values[ i ] = string_utils::fromString< int >( ( *it ) );
        ++i;
        ++loaded;
    }

    // finally, set the values
    return values;
}

bool WQtNetworkEditorProjectFileIO::parse( std::string line, unsigned int lineNumber )
{
    static const boost::regex networkCoordRe( "^ *QT5GUI_NETWORK:([0-9]*)=(.*)$" );
    static const boost::regex networkFlagsRe( "^ *QT5GUI_NETWORK_Flags:([0-9]*)=(.*)$" );
    static const boost::regex networkScaleRe( "^ *QT5GUI_NETWORK_Scale:(.*)$" );

    // use regex to parse it
    boost::smatch matches;  // the list of matches
    if( boost::regex_match( line, matches, networkCoordRe ) )
    {
        // network editor coordinate
        // matches[1] is the ID of the module
        // matches[2] is the network coordinate
        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Network \"" << matches[2] << "\" with module ID " << matches[1];

        unsigned int id = string_utils::fromString< unsigned int >( matches[1] );
        int loaded = 0;
        int* coordRaw = parseIntSequence( string_utils::toString( matches[2] ), 4, loaded );

        if( loaded == 2 ) // loaded coords
        {
            // store. Applied later.
            m_networkCoords[ id ] = QPoint( coordRaw[0], coordRaw[1] );
        }

        return true;
    }
    else if( boost::regex_match( line, matches, networkFlagsRe ) )
    {
        // network editor coordinate
        // matches[1] is the ID of the module
        // matches[2] is the network coordinate
        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Network Flags\"" << matches[2] << "\" with module ID " << matches[1];

        unsigned int id = string_utils::fromString< unsigned int >( matches[1] );
        int loaded = 0;
        int* coordRaw = parseIntSequence( string_utils::toString( matches[2] ), 4, loaded );

        if( loaded == 2 ) // loaded flags
        {
            // store. Applied later.
            m_networkFlags[ id ] =  QPoint( coordRaw[0], coordRaw[1] );
        }

        return true;
    }
    else if( boost::regex_match( line, matches, networkScaleRe ) )
    {
        wlog::debug( "Project Loader [Parser]" ) << "Line " << lineNumber << ": Network Scale " << matches[1];
        m_networkScale = string_utils::fromString< double >( matches[1] );

        return true;
    }

    return false;
}

void WQtNetworkEditorProjectFileIO::done()
{
    // apply changes

    // get bounding box
    QPoint bbTL; // top left
    QPoint bbBR; // bottom right
    for( ModuleNetworkCoordinates::const_iterator it = m_networkCoords.begin(); it != m_networkCoords.end(); ++it )
    {
        // init first element
        if( it == m_networkCoords.begin() )
        {
            bbTL = ( *it ).second;
            bbBR = ( *it ).second;
        }
        else
        {
            // min/max
            bbTL.rx() = std::min( bbTL.x(), ( *it ).second.x() );
            bbTL.ry() = std::min( bbTL.y(), ( *it ).second.y() );
            bbBR.rx() = std::max( bbBR.x(), ( *it ).second.x() );
            bbBR.ry() = std::max( bbBR.y(), ( *it ).second.y() );
        }
    }

    // request a reserved grid zone:
    // NOTE: future possibility?!
    // unsigned int reservedAreadID = m_networkEditor->getLayout()->reserveRect( bbTL, bbBR );

    // get next free column
    int firstFree = m_networkEditor->getLayout()->getGrid()->getFirstFreeColumn();

    // iterate our list.
    for( ModuleNetworkCoordinates::const_iterator it = m_networkCoords.begin(); it != m_networkCoords.end(); ++it )
    {
        // map the ID to a real module
        WModule::SPtr module = getProject()->mapToModule( ( *it ).first );
        if( module )
        {
            // register in network editor instance as a default position
            // ensure that the coordinates where relative to the top-left item. Although this is done by the writing code, we should assume someone
            // has modified the file. Why? One of the primary directives for the project file IO classes is to be fault tolerant.
            // NOTE: usually bbTL should be 0,0
            QPoint p = ( *it ).second - bbTL;
            p.rx() += firstFree;
            m_networkEditor->getLayout()->setModuleDefaultPosition( module, p );
        }
    }

    for( ModuleNetworkCoordinates::const_iterator it = m_networkFlags.begin(); it != m_networkFlags.end(); ++it )
    {
        // map the ID to a real module
        WModule::SPtr module = getProject()->mapToModule( ( *it ).first );
        if( module )
        {
            // register in network editor instance as a default flag
            // NOTE: we where lazy and used QPoint for storing both bools
            QPoint p = ( *it ).second;
            m_networkEditor->getLayout()->setModuleDefaultFlags( module, static_cast< bool >( p.x() ),
                                                                         static_cast< bool >( p.y() ) );
        }
    }

    m_networkEditor->getView()->scale( m_networkScale, m_networkScale );
}

void WQtNetworkEditorProjectFileIO::save( std::ostream& output ) // NOLINT
{
    output << "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl <<
              "// QtGUI Network Information" << std::endl <<
              "//////////////////////////////////////////////////////////////////////////////////////////////////////////////////" << std::endl <<
              std::endl;

    // write the relative grid positions of each module
    WQtNetworkItemGrid* grid = m_networkEditor->getLayout()->getGrid();

    // IMPORTANT: we store relative coordinates. So we need the bounding rect to get relative coords
    QPoint bb = grid->getGridBoundingRect().topLeft();

    // iterate each module:
    // Grab container and lock
    WModuleContainer::ModuleSharedContainerType::ReadTicket container = WKernel::getRunningKernel()->getRootContainer()->getModules();
    // Iterate
    for( WModuleContainer::ModuleConstIterator iter = container->get().begin(); iter != container->get().end(); ++iter )
    {
        // a module has its own project file specific ID:
        unsigned int id = getProject()->mapFromModule( *iter );

        // get the graphical item for this module
        WQtNetworkItem* item = m_networkEditor->findItemByModule( *iter );

        // as grid for position
        if( grid->isInGrid( item ) )
        {
            QPoint p = grid->whereIs( item );

            // NOTE: we write relative coordinates! So subtract bounding box top-left corner from each coordinate.
            output << "QT5GUI_NETWORK:" << id << "=" << p.x() - bb.x() << ";" << p.y() - bb.y() << std::endl;
            output << "QT5GUI_NETWORK_Flags:" << id << "=" << item->wasLayedOut() << ";" << item->wasManuallyPlaced() << std::endl;
        }
        // else: not in grid. We do not save info for this module
    }

    output << "QT5GUI_NETWORK_Scale:" << m_networkEditor->getView()->transform().m11() << std::endl;
}
