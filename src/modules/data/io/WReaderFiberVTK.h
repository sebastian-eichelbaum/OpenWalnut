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

#ifndef WREADERFIBERVTK_H
#define WREADERFIBERVTK_H

#include <fstream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "core/common/WStringUtils.h"
#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/exceptions/WDHIOFailure.h"
#include "core/dataHandler/exceptions/WDHNoSuchFile.h"
#include "core/dataHandler/exceptions/WDHParseError.h"

#include "core/dataHandler/io/WReader.h"

/**
 * Reads fibers from a VTK file. For VTK just see http://www.vtk.org.
 * Currently only a subset of the legacy format is supported: MedInria's
 * Fib-VTK format. The byte order of the files is assumed to be big endian by
 * default.
 *
 * \ingroup dataHandler
 */
class WReaderFiberVTK : public WReader // NOLINT
{
friend class WReaderFiberVTKTest;
public:
    /**
     * Constructs and makes a new VTK reader for separate thread start.
     *
     * \param fname File name where to read data from
     * \throws WDHNoSuchFile
     */
    explicit WReaderFiberVTK( std::string fname );

    /**
     * Destroys this instance and closes the file.
     */
    virtual ~WReaderFiberVTK() throw();

    /**
     * Reads the fiber file and creates a dataset out of it.
     *
     * \return Reference to the dataset.
     */
    virtual boost::shared_ptr< WDataSetFibers > read();

protected:
    /**
     * Read VTK header from file.
     *
     * \return The offset where header ends, so we may skip this next operation.
     * \throws WDHIOFailure, WDHParseError
     */
    void readHeader();

    /**
     * Read VTK POINTS field from input stream.
     */
    void readPoints();

    /**
     * Read VTK VALUES field from input stream.
     */
    void readValues();

    /**
     * Read VTK LINES field from input stream.
     */
    void readLines();

    std::vector< std::string > m_header; //!< VTK header of the read file

    /**
     * Stores for every point its x,y and z float value successivley.
     * \note The i'th point starts at the 3*i index since every point consumes
     * 3 elements.
     */
    boost::shared_ptr< std::vector< float > > m_points;

    boost::shared_ptr< std::vector< size_t > > m_fiberStartIndices; //!< Stores the start indices (in the point array) for every fiber

    boost::shared_ptr< std::vector< size_t > > m_fiberLengths; //!< Stores the length of every fiber

    WDataSetFibers::VertexParemeterArray m_fiberParameters; //!< additional colors if found in file.

    /**
     * Stores for every point the fiber where it belongs to.
     * \note This vector has as many components as there are points, hence its
     * length is just a third of the length of the points vector.
     */
    boost::shared_ptr< std::vector< size_t > > m_pointFiberMapping;

    boost::shared_ptr< std::ifstream > m_ifs; //!< Pointer to the input file stream reader.

private:
    /**
     * Reads the next line from current position in stream of the fiber VTK file.
     *
     * \param desc In case of trouble while reading, this gives information in
     * the error message about what was tried to read
     * \throws WDHIOFailure, WDHParseError
     * \return Next line as string.
     */
    std::string getLine( const std::string& desc );

    /**
     * Try to cast from the given string to the template value T. If the cast fails a
     * WDHParseError is thrown.
     *
     * \throws WDHParseError
     * \param stringValue The string to cast from
     * \param errMsg Error message incase the cast fails
     * \return The casted value from the given string.
     */
    template< typename T > T getLexicalCast( std::string stringValue, const std::string& errMsg ) const;
};

template< typename T > inline T WReaderFiberVTK::getLexicalCast( std::string stringValue, const std::string& errMsg ) const
{
    T result;
    try
    {
        result = string_utils::fromString< T >( stringValue );
    }
    catch( const std::exception &e )
    {
        throw WDHParseError( std::string( "Cast error in VTK fiber file: " + m_fname + ": " + errMsg + ": " + stringValue ) );
    }

    return result;
}
#endif  // WREADERFIBERVTK_H
