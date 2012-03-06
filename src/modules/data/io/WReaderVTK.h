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

#ifndef WREADERVTK_H
#define WREADERVTK_H

#include <fstream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "core/common/WStringUtils.h"
#include "core/dataHandler/WDataSet.h"
#include "core/dataHandler/exceptions/WDHIOFailure.h"
#include "core/dataHandler/exceptions/WDHNoSuchFile.h"
#include "core/dataHandler/exceptions/WDHParseError.h"

#include "core/dataHandler/io/WReader.h"

/**
 * Reads simple VTK files. For VTK just see http://www.vtk.org.
 * Currently only a subset of the legacy format is supported.
 *
 * Formats read: Original ASCII and BINARY. The xml format is not supported.
 * Grids read are: STRUCTURED_POINTS
 * Data types read: SCALARS, VECTORS, TENSORS
 *
 * Endianess is kept as on the platform, so for intel hosts, most files,
 * which are typically stored in little endian, are not readable if they
 * are binary files.
 *
 * \ingroup dataHandler
 */
class WReaderVTK : public WReader // NOLINT
{
    friend class WReaderVTKTest;
    public:
    /**
     * Constructs and makes a new VTK reader for separate thread start.
     *
     * \param fname File name where to read data from
     * \throws WDHNoSuchFile
     */
    explicit WReaderVTK( std::string fname );

    /**
     * Destroys this instance and closes the file.
     */
    virtual ~WReaderVTK() throw();

    /**
     * Reads the fiber file and creates a dataset out of it.
     *
     * \return Reference to the dataset.
     */
    virtual boost::shared_ptr< WDataSet > read();

    protected:
    /**
     * Read VTK header from file.
     *
     * \return The offset where header ends, so we may skip this next operation.
     * \throws WDHIOFailure, WDHParseError
     */
    void readHeader();

    /**
     * Read VTK POINT_DATA field from input stream.
     */
    void readPointData();

    /**
     * Read VTK STRUCTURED_POINTS field
     */
    void readStructuredPoints();

    /**
     * Read VTK SCALARS field
     * \post the data set pointer is set to the data set constructed of the current grid
     *       and the read scalar values
     * \param nbScalars
     *      the number of scalars to read
     * \param name
     *      the name of the data set that may be overwritten by information in the scalars line
     */
    void readScalars( size_t nbScalars, const std::string & name );

    /**
     * Read VTK SCALARS field
     * \post the data set pointer is set to the data set constructed of the current grid
     *       and the read vector values
     * \param nbVectors
     *      the number of vectors to read
     * \param name
     *      the name of the data set that may be overwritten by information in the vectors line
      */
    void readVectors( size_t nbVectors, const std::string & name );

    /**
     * Read VTK TENSORS field
     * \post the data set pointer is set to the data set constructed of the current grid
     *       and the read tensor values
     * \param nbTensors
     *      the number of tensors to read
     * \param name
     *      the name of the data set that may be overwritten by information in the tensors line
      */
    void readTensors( size_t nbTensors, const std::string & name );


    std::vector< std::string > m_header; //!< VTK header of the read file

    /**
     * Stores for every point its x,y and z float value successivley.
     * \note The i'th point starts at the 3*i index since every point consumes
     * 3 elements.
     */
    boost::shared_ptr< std::vector< float > > m_points;

    //boost::shared_ptr< std::vector< size_t > > m_fiberStartIndices; //!< Stores the start indices (in the point array) for every fiber

    //boost::shared_ptr< std::vector< size_t > > m_fiberLengths; //!< Stores the length of every fiber

    /**
     * Stores for every point the fiber where it belongs to.
     * \note This vector has as many components as there are points, hence its
     * length is just a third of the length of the points vector.
     */
    //boost::shared_ptr< std::vector< size_t > > m_pointFiberMapping;

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



    /**
     * reference to the currently loading data set
     */
    boost::shared_ptr< WDataSet > newDataSet;

    /**
     * reference to the currently loading grid
     */
    boost::shared_ptr< WGridRegular3D > newGrid;

    /**
     * internal flag whether we read ascii or binary
     */
    bool isAscii;
};

template< typename T > inline T WReaderVTK::getLexicalCast( std::string stringValue, const std::string& errMsg ) const
{
    T result;
    try
    {
        result = string_utils::fromString< T >( stringValue );
    }
    catch( const std::exception &e )
    {
        throw WDHParseError( std::string( "Cast error in VTK file: " + m_fname + ": " + errMsg + ": " + stringValue ) );
    }

    return result;
}
#endif  // WREADERVTK_H
