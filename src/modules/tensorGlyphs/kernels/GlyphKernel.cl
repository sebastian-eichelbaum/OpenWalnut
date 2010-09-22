
/* Order passed by clBuildProgram */

#define NumOfCoeffs ((Order + 1)*(Order + 2) / 2)

__kernel void scaleGlyphs(__global float* tensorData,__constant float* factors,int4 numOfTensors)
{
    if (get_global_id(0) >= numOfTensors.x) return;
    if (get_global_id(1) >= numOfTensors.y) return;
    if (get_global_id(2) >= numOfTensors.z) return;

    __global float* tensor = tensorData + NumOfCoeffs * 
    (get_global_id(0) + get_global_id(1) * numOfTensors.x + get_global_id(2) * numOfTensors.x * numOfTensors.y);

    float scale = 0.0f;

    for (uint i = 0; i < NumOfCoeffs; i++)
    {
        scale += tensor[i] * tensor[i] * factors[i];
    }

    if (scale == 0.0f)
    {
        return;
    }

    scale = 0.4f * rsqrt(scale);

    for (uint i = 0; i < NumOfCoeffs; i++)
    {
        tensor[i] *= scale * factors[i];
    }
}

float evalGlyph(float4 position,__global float* tensor)
{
    float xPower[Order + 1];
    float yPower[Order + 1];
    float zPower[Order + 1];

    xPower[0] = 1.0f;
    yPower[0] = 1.0f;
    zPower[0] = 1.0f;

    for (uint i = 1; i <= Order; i++)
    {
        xPower[i] = xPower[i - 1] * position.x;
        yPower[i] = yPower[i - 1] * position.y;
        zPower[i] = zPower[i - 1] * position.z;
    }

    uint i = 0;

    float value = 0.0f;

    for (uint z = 0; z <= Order; z++)
    {
        for (uint y = 0; y <= Order - z; y++,i++)
        {
            value += tensor[i] * xPower[Order - z - y] * yPower[y] * zPower[z];
        }
    }

    return value;
}

float4 normal(float4 position,__global float* tensor)
{
    float xPower[Order + 1];
    float yPower[Order + 1];
    float zPower[Order + 1];

    xPower[0] = 1.0f;
    yPower[0] = 1.0f;
    zPower[0] = 1.0f;

    for (uint i = 1; i <= Order; i++)
    {
        xPower[i] = xPower[i - 1] * position.x;
        yPower[i] = yPower[i - 1] * position.y;
        zPower[i] = zPower[i - 1] * position.z;
    }

    uint i = 0;

    float value = 0.0f;
    float dxValue = 0.0f;
    float dyValue = 0.0f;
    float dzValue = 0.0f;

    for (uint z = 0; z <= Order; z++)
    {
        for (uint y = 0; y <= Order - z; y++,i++)
        {
            uint x = Order - z - y;

            value += tensor[i] * xPower[x] * yPower[y] * zPower[z];

            if (x > 0)
            {
                dxValue += tensor[i] * x * xPower[x - 1] * yPower[y] * zPower[z];
            }

            if (y > 0)
            {
                dyValue += tensor[i] * y * xPower[x] * yPower[y - 1] * zPower[z];
            }

            if (z > 0)
            {
                dzValue += tensor[i] * z * xPower[x] * yPower[y] * zPower[z - 1];
            }
        }
    }

    return normalize((1 + Order) * position - (float4)(dxValue,dyValue,dzValue,0.0f) / value);
}

float4 letThereBeLight(float4 position,float4 direction,__global float* tensor)
{
    float4 color;

    float t = 1.25f * length(position);

    if(t <= 0.0f)
    {
        color = (float4)(1.0f,0.0f,0.0f,1.0f);
    }
    else
    {
        color = (1.0f - t) * (float4)(1.0f,0.0f,0.0f,1.0f) + t * (float4)(1.0f,1.0f,0.0f,1.0f);
    }

    float cosViewNormal = -dot(direction,normal(normalize(position),tensor));
    float cosViewReflection = 2.0f * cosViewNormal * cosViewNormal - 1.0f;

    cosViewReflection *= cosViewReflection;
    cosViewReflection *= cosViewReflection;
    cosViewReflection *= cosViewReflection;

    return (0.5f * color + (0.3f * cosViewNormal + 0.2f * cosViewReflection) * (float4)(1.0f,1.0f,1.0f,1.0f));
}

float4 findIntersectionPoint(float4 origin,float4 direction,float lowerBound,float upperBound,__global float* tensor)
{
    float4 upperPoint = origin + lowerBound * direction;

    float invLength = rsqrt(dot(upperPoint,upperPoint));
    float value = 1.0f / invLength - evalGlyph((upperPoint * invLength),tensor);

    while (lowerBound < upperBound)
    {
        float stepSize = fmax((0.25f * value),0.01f);

        upperPoint += stepSize * direction;
        lowerBound += stepSize;

        invLength = rsqrt(dot(upperPoint,upperPoint));
        value = 1.0f / invLength - evalGlyph((upperPoint * invLength),tensor);

        if (value <= 0.0f)
        {
            float4 lowerPoint = upperPoint - 0.01f * direction;
            float4 midPoint;

            for (uint i = 0; i < 4; i++)
            {
                midPoint = (upperPoint + lowerPoint) * 0.5f;
                invLength = rsqrt(dot(midPoint,midPoint));

                if ((1.0f / invLength) < evalGlyph((midPoint * invLength),tensor))
                {
                    upperPoint = midPoint;
                }
                else
                {
                    lowerPoint = midPoint;
                }
            }

            return ((upperPoint + lowerPoint) * 0.5f);
        }
    }

    return origin;
}

int4 cellBound(float4 direction,int4 gridCell)
{
    if (direction.x >= 0.0f)
    {
        gridCell.x++;
    }

    if (direction.y >= 0.0f)
    {
        gridCell.y++;
    }

    if (direction.z >= 0.0f)
    {
        gridCell.z++;
    }

    return gridCell;
}

bool isInSliceX(int4 gridCell,int4 slices,int4 numOfTensors)
{
    return ((gridCell.x == slices.x) && (gridCell.y >= 0) && (gridCell.y < numOfTensors.y) && (gridCell.z >= 0) && (gridCell.z < numOfTensors.z));
}

bool isInSliceY(int4 gridCell,int4 slices,int4 numOfTensors)
{
    return ((gridCell.y == slices.y) && (gridCell.x >= 0) && (gridCell.x < numOfTensors.x) && (gridCell.z >= 0) && (gridCell.z < numOfTensors.z));
}

bool isInSliceZ(int4 gridCell,int4 slices,int4 numOfTensors)
{
    return ((gridCell.z == slices.z) && (gridCell.x >= 0) && (gridCell.x < numOfTensors.x) && (gridCell.y >= 0) && (gridCell.y < numOfTensors.y));
}

bool traverse(float4 initialPoint,float4 direction,float4 tDelta,float4* tMax,float4* tSlice,int4* gridCell,
              int4* entryCells,int4 stepDirection,int4 slices,int4 sliceEnabled,int4 numOfTensors)
{
    if ((*tMax).x < (*tMax).y)
    {
        if ((*tMax).x < (*tMax).z)
        {
            (*gridCell).x += stepDirection.x;
            (*tMax).x += tDelta.x;
        }
        else
        {
            (*gridCell).z += stepDirection.z;
            (*tMax).z += tDelta.z;
        }
    }
    else
    {
        if ((*tMax).y < (*tMax).z)
        {
            (*gridCell).y += stepDirection.y;
            (*tMax).y += tDelta.y;
        }
        else
        {
            (*gridCell).z += stepDirection.z;
            (*tMax).z += tDelta.z;
        }
    }

    while (!(sliceEnabled.x && isInSliceX(*gridCell,slices,numOfTensors)) && 
           !(sliceEnabled.y && isInSliceY(*gridCell,slices,numOfTensors)) && 
           !(sliceEnabled.z && isInSliceZ(*gridCell,slices,numOfTensors)))
    {
        if ((*tSlice).x < (*tSlice).y)
        {
            if ((*tSlice).x < (*tSlice).z)
            {
                if ((*tSlice).x == INFINITY)
                {
                    return true;
                }

                if ((*tSlice).x >= 0.0f)
                {
                    (*gridCell) = entryCells[0];
                }

                (*tSlice).x = INFINITY;
            }
            else
            {
                if ((*tSlice).z == INFINITY)
                {
                    return true;
                }

                if ((*tSlice).z >= 0.0f)
                {
                    (*gridCell) = entryCells[2];
                }

                (*tSlice).z = INFINITY;
            }
        }
        else
        {
            if ((*tSlice).y < (*tSlice).z)
            {
                if ((*tSlice).y == INFINITY)
                {
                    return true;
                }

                if ((*tSlice).y >= 0.0f)
                {
                    (*gridCell) = entryCells[1];
                }

                (*tSlice).y = INFINITY;
            }
            else
            {
                if ((*tSlice).z == INFINITY)
                {
                    return true;
                }

                if ((*tSlice).z >= 0.0f)
                {
                    (*gridCell) = entryCells[2];
                }

                (*tSlice).z = INFINITY;
            }
        }
    }

    (*tMax) = fabs(convert_float4(cellBound(direction,(*gridCell))) - initialPoint) * tDelta;

    return false;
}

float4 jumpToSliceBound(float4 initialPoint,float4 direction,
                        int4 sliceLowerBound,int4 sliceUpperBound)
{
    float4 tSliceBound;

    if ((initialPoint.x >= sliceLowerBound.x) && (initialPoint.x <= sliceUpperBound.x))
    {
        tSliceBound.x = 0.0f;
    }
    else
    {
        if (direction.x == 0.0f)
        {
            tSliceBound.x = INFINITY;
        }
        else
        {
            if (direction.x < 0.0f)
            {
                tSliceBound.x = (sliceUpperBound.x - initialPoint.x) / direction.x;
            }
            else
            {
                tSliceBound.x = (sliceLowerBound.x - initialPoint.x) / direction.x;
            }
        }
    }

    if ((initialPoint.y >= sliceLowerBound.y) && (initialPoint.y <= sliceUpperBound.y))
    {
        tSliceBound.y = 0.0f;
    }
    else
    {
        if (direction.y == 0.0f)
        {
            tSliceBound.y = INFINITY;
        }
        else
        {
            if (direction.y < 0.0f)
            {
                tSliceBound.y = (sliceUpperBound.y - initialPoint.y) / direction.y;
            }
            else
            {
                tSliceBound.y = (sliceLowerBound.y - initialPoint.y) / direction.y;
            }
        }
    }

    if ((initialPoint.z >= sliceLowerBound.z) && (initialPoint.z <= sliceUpperBound.z))
    {
        tSliceBound.z = 0.0f;
    }
    else
    {
        if (direction.z == 0.0f)
        {
            tSliceBound.z = INFINITY;
        }
        else
        {
            if (direction.z < 0.0f)
            {
                tSliceBound.z = (sliceUpperBound.z - initialPoint.z) / direction.z;
            }
            else
            {
                tSliceBound.z = (sliceLowerBound.z - initialPoint.z) / direction.z;
            }
        }
    }

    return tSliceBound;
}

int4 calcGridCell(float4 initialPoint,float4 direction,float tSlice,float4 tSliceBound)
{
    int4 entryCell;

    if (tSlice > 0.0f)
    {
        float4 newPos = initialPoint + tSlice * direction;

        if (tSliceBound.x == tSlice)
        {
            if (direction.x < 0.0f)
            {
                entryCell.x = rint(newPos.x) - 1.0f;
            }
            else
            {
                entryCell.x = rint(newPos.x);
            }
        }
        else
        {
            entryCell.x = floor(newPos.x);
        }

        if (tSliceBound.y == tSlice)
        {
            if (direction.y < 0.0f)
            {
                entryCell.y = rint(newPos.y) - 1.0f;
            }
            else
            {
                entryCell.y = rint(newPos.y);
            }
        }
        else
        {
            entryCell.y = floor(newPos.y);
        }

        if (tSliceBound.z == tSlice)
        {
            if (direction.z < 0.0f)
            {
                entryCell.z = rint(newPos.z) - 1.0f;
            }
            else
            {
                entryCell.z = rint(newPos.z);
            }
        }
        else
        {
            entryCell.z = floor(newPos.z);
        }

        entryCell.w = 0;
    }

    return entryCell;
}

void findSliceEntryPoints(float4 initialPoint,float4 direction,float4* tSlice,
                          int4* entryCells,int4 slices,int4 numOfTensors)
{
    int4 sliceLowerBound;
    int4 sliceUpperBound;
    float4 tSliceBound;

    sliceLowerBound.x = slices.x;
    sliceLowerBound.y = 0;
    sliceLowerBound.z = 0;

    sliceUpperBound.x = slices.x + 1;
    sliceUpperBound.y = numOfTensors.y;
    sliceUpperBound.z = numOfTensors.z;

    tSliceBound = jumpToSliceBound(initialPoint,direction,sliceLowerBound,sliceUpperBound);
    (*tSlice).x = fmax(fmax(tSliceBound.x,tSliceBound.y),tSliceBound.z);
    entryCells[0] = calcGridCell(initialPoint,direction,(*tSlice).x,tSliceBound);

    sliceLowerBound.x = 0;
    sliceLowerBound.y = slices.y;
    sliceLowerBound.z = 0;

    sliceUpperBound.x = numOfTensors.x;
    sliceUpperBound.y = slices.y + 1;
    sliceUpperBound.z = numOfTensors.z;

    tSliceBound = jumpToSliceBound(initialPoint,direction,sliceLowerBound,sliceUpperBound);
    (*tSlice).y = fmax(fmax(tSliceBound.x,tSliceBound.y),tSliceBound.z);
    entryCells[1] = calcGridCell(initialPoint,direction,(*tSlice).y,tSliceBound);

    sliceLowerBound.x = 0;
    sliceLowerBound.y = 0;
    sliceLowerBound.z = slices.z;

    sliceUpperBound.x = numOfTensors.x;
    sliceUpperBound.y = numOfTensors.y;
    sliceUpperBound.z = slices.z + 1;

    tSliceBound = jumpToSliceBound(initialPoint,direction,sliceLowerBound,sliceUpperBound);
    (*tSlice).z = fmax(fmax(tSliceBound.x,tSliceBound.y),tSliceBound.z);
    entryCells[2] = calcGridCell(initialPoint,direction,(*tSlice).z,tSliceBound);
}

__kernel void renderGlyphs(float4 initialPoint,float4 direction,float4 x,float4 y,
                           float near,float far,uint width,uint height,
                           int4 slices,int4 sliceEnabled,int4 numOfTensors,__global float* tensorData,
                           __write_only image2d_t color,__write_only image2d_t depth)
{
    if (get_global_id(0) >= width)
    {
        return;
    }

    if (get_global_id(1) >= height)
    {
        return;
    }

    float depthScale = rsqrt(dot(direction,direction));

    initialPoint = initialPoint + ((float)get_global_id(0) / (float)width) * x + ((float)get_global_id(1) / (float)height) * y;
    direction *= depthScale;

    int4 stepDirection = convert_int4(sign(direction));
    int4 gridCell = convert_int4(floor(initialPoint));
    int4 entryCells[3];
    float4 tDelta = (float4)1.0f / fabs(direction);
    float4 tMax = fabs(convert_float4(cellBound(direction,gridCell)) - initialPoint) * tDelta;
    float4 tSlice;

    findSliceEntryPoints(initialPoint,direction,&tSlice,entryCells,slices,numOfTensors);

    if (traverse(initialPoint,direction,tDelta,&tMax,&tSlice,&gridCell,entryCells,stepDirection,slices,sliceEnabled,numOfTensors))
    {
        write_imagef(depth,(int2)(get_global_id(0),get_global_id(1)),1.0f);

        return;
    }

    while (true)
    {
        __global float* tensor = tensorData + NumOfCoeffs * 
        (gridCell.x + gridCell.y * numOfTensors.x + gridCell.z * numOfTensors.x * numOfTensors.y);

        float4 centerToInPoint = initialPoint - (convert_float4(gridCell) + (float4)(0.5f,0.5f,0.5f,0.0f));

        float tempDot = -dot(centerToInPoint,direction);
        float discrim = tempDot * tempDot - dot(centerToInPoint,centerToInPoint) + 0.25f;

        if (discrim > 0.0f)
        {
            discrim = sqrt(discrim);

            float4 intPoint = findIntersectionPoint(centerToInPoint,direction,(tempDot - discrim),(tempDot + discrim),tensor);

            if (any(intPoint != centerToInPoint))
            {
                float fragDepth = (dot(intPoint - initialPoint,direction) * depthScale - 1.0f) / (far - near) * near;

                write_imagef(color,(int2)(get_global_id(0),get_global_id(1)),letThereBeLight(intPoint,direction,tensor));
                write_imagef(depth,(int2)(get_global_id(0),get_global_id(1)),fragDepth);

                return;
            }
        }

        if (traverse(initialPoint,direction,tDelta,&tMax,&tSlice,&gridCell,entryCells,stepDirection,slices,sliceEnabled,numOfTensors))
        {
            write_imagef(depth,(int2)(get_global_id(0),get_global_id(1)),1.0f);

            return;
        }
    }
}