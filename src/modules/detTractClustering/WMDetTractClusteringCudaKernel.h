//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2010 RRZK, University of Cologne
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

#ifndef WMDETTRACTCLUSTERINGCUDAKERNEL_H
#define WMDETTRACTCLUSTERINGCUDAKERNEL_H

class ProgressWrapper;

/**
 * initialize upper trigonal matrix distmat with Zhang metric
 *
 * the fibres have a total of ncoords coords (x,y,z interleaved),
 * there are ntracts fibres, the sizes[i] coords for fibre i start at offsets[i],
 * the longest fibre has a length of maxlength points,
 * threshold is 'proximity threshold',
 * progress is reported through progress
 *
 * returns:
 *   true on success
 *   false otherwise (e.g. if maxlength is longer than can be handled by the CUDA kernel)
 */  
extern "C" bool distCuda(float *distmat,
        const int ncoords, const float *coords,
        const int ntracts, const int *offsets, const int *sizes,
        const float threshold, ProgressWrapper *progress );
#endif
