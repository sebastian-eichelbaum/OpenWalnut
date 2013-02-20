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
 * Grids read are: STRUCTURED_POINTS, RECTILINEAR_GRID is interpreted as a grid with
 *                 equal spacing per coordinate axis
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
    //! allow member access by the test class
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
     * Reads the data file and creates a dataset out of it.
     *
     * \return Reference to the dataset.
     */
    virtual boost::shared_ptr< WDataSet > read();

protected:
    /**
     * Read VTK header from file. Sets the m_isASCII member accordingly.
     *
     * \return true, if the header was valid
     */
    bool readHeader();

    /**
     * Read VTK Domain specification and create a matching grid.
     *
     * \return The constructed grid or an invalid pointer if any problem occurred.
     */
    boost::shared_ptr< WGridRegular3D > readStructuredPoints();

    /**
     * Read VTK Domain specification and create a matching grid.
     *
     * \return The constructed grid or an invalid pointer if any problem occurred.
     */
    boost::shared_ptr< WGridRegular3D > readRectilinearGrid();

    /**
     * Read domain information for structured points.
     *
     * \param grid The grid constructed from the domain information.
     *
     * \return The value set containing the values read from the file or an invalid pointer if anything went wrong.
     */
    boost::shared_ptr< WValueSetBase > readData( boost::shared_ptr< WGridRegular3D > const& grid );

    /**
     * Read VTK SCALARS field
     * \post the data set pointer is set to the data set constructed of the current grid
     *       and the read scalar values
     * \param nbScalars
     *      the number of scalars to read
     * \param name
     *      the name of the data set that may be overwritten by information in the scalars line
     *
     * \return The resulting value set.
     */
    boost::shared_ptr< WValueSetBase > readScalars( size_t nbScalars, const std::string & name );

    /**
     * Read VTK SCALARS field
     * \post the data set pointer is set to the data set constructed of the current grid
     *       and the read vector values
     * \param nbVectors
     *      the number of vectors to read
     * \param name
     *      the name of the data set that may be overwritten by information in the vectors line
     *
     * \return The resulting value set.
     */
    boost::shared_ptr< WValueSetBase > readVectors( size_t nbVectors, const std::string & name );

    /**
     * Read HARDI data from the file.
     *
     * \param nbPoints The number of voxels.
     * \param nbGradients The number of gradients for this HARDI dataset.
     *
     * \return The data read as a value set of floats.
     */
    boost::shared_ptr< WValueSetBase > readHARDI( std::size_t nbPoints, std::size_t nbGradients, const std::string& /*name*/ );

    /**
     * Read VTK TENSORS field
     * \post the data set pointer is set to the data set constructed of the current grid
     *       and the read tensor values
     * \param nbTensors
     *      the number of tensors to read
     * \param name
     *      the name of the data set that may be overwritten by information in the tensors line
     *
     * \return The resulting value set.
     */
    boost::shared_ptr< WValueSetBase > readTensors( size_t nbTensors, const std::string & name );

    std::vector< std::string > m_header; //!< VTK header of the read file

    boost::shared_ptr< std::ifstream > m_ifs; //!< Pointer to the input file stream reader.

private:
    //! The types of domains supported so far.
    enum DomainType
    {
        //! structured points
        STRUCTURED_POINTS,

        //! rectilinear grid
        RECTILINEAR_GRID,

        //! anything else
        UNSUPPORTED_DOMAIN
    };

    //! The types of attributes supported so far.
    enum AttributeType
    {
        //! scalar data
        SCALARS,

        //! vector data
        VECTORS,

        //! tensor data
        TENSORS,

        //! array data
        ARRAYS,

        //! anything else
        UNSUPPORTED_ATTRIBUTE
    };

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
     * Read the coordinates of the dataset's domain.
     *
     * \param name The name to look for in the first line.
     * \param dim The number of coordinates.
     * \param coords The resulting vector of coordinates.
     */
    void readCoords( std::string const& name, std::size_t dim, std::vector< float >& coords );

    /**
     * Read values from the file. The m_isASCII flag must be initialized accordingly.
     *
     * \param values The values read from the file.
     * \param numValues The number of values to read.
     */
    void readValuesFromFile( std::vector< float >& values, std::size_t numValues ); // NOLINT: is non-const ref since we write results to this vector

    /**
     * Read gradients from a gradients file which has the same basename as the data file and ends in .bvec.
     *
     * \return A pointer to a vector of gradients.
     */
    boost::shared_ptr< std::vector< WVector3d > > readGradients();

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
    boost::shared_ptr< WDataSet > m_newDataSet;

    /**
     * reference to the currently loading grid
     */
    boost::shared_ptr< WGridRegular3D > m_newGrid;

    /**
     * internal flag whether we read ascii or binary
     */
    bool m_isASCII;

    //! The type of domain specified in the file.
    DomainType m_domainType;

    //! The type of the attributes first read from the file.
    AttributeType m_attributeType;
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
