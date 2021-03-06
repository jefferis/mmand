#include <RcppArmadillo.h>

#include "Array.h"

Neighbourhood Array::getNeighbourhood () const
{
    return this->getNeighbourhood(dims);
}

Neighbourhood Array::getNeighbourhood (const int width) const
{
    std::vector<int> widths(nDims);
    for (int i=0; i<nDims; i++)
        widths[i] = width;
    return this->getNeighbourhood(widths);
}

Neighbourhood Array::getNeighbourhood (const std::vector<int> &widths) const
{
    Neighbourhood neighbourhood;
    
    neighbourhood.widths = widths;
    std::vector<int> extremes(nDims);
    for (int i=0; i<nDims; i++)
    {
        if (neighbourhood.widths[i] % 2 == 0)
            neighbourhood.widths[i]++;
        extremes[i] = (neighbourhood.widths[i] - 1) / 2;
    }
    
    neighbourhood.size = 1;
    std::vector<long> steps(nDims+1);
    steps[0] = 1;
    for (int i=0; i<nDims; i++)
    {
        neighbourhood.size *= neighbourhood.widths[i];
        steps[i+1] = steps[i] * dims[i];
    }
    
    neighbourhood.locs = arma::Mat<int>(neighbourhood.size, nDims);
    neighbourhood.offsets = std::vector<long>(neighbourhood.size);
    
    for (int j=0; j<neighbourhood.size; j++)
    {
        if (j==0)
        {
            for (int i=0; i<nDims; i++)
                neighbourhood.locs(j,i) = -extremes[i];
        }
        else
        {
            neighbourhood.locs(j,0) = neighbourhood.locs(j-1,0) + 1;
            for (int i=0; i<nDims; i++)
            {
                if (neighbourhood.locs(j,i) > extremes[i])
                {
                    neighbourhood.locs(j,i) = -extremes[i];
                    neighbourhood.locs(j,i+1) = neighbourhood.locs(j-1,i+1) + 1;
                }
                else if (i < (nDims-1))
                    neighbourhood.locs(j,i+1) = neighbourhood.locs(j-1,i+1);
            }
        }
        
        neighbourhood.offsets[j] = 0;
        for (int i=0; i<nDims; i++)
            neighbourhood.offsets[j] += neighbourhood.locs(j,i) * steps[i];
    }
    
    return neighbourhood;
}

void Array::flattenIndex (const std::vector<int> &loc, long &result) const
{
    // Dimensionalities 1-3 are most common so treat them as special cases for speed
    switch (nDims)
    {
        case 1:
        result = loc[0];
        break;
        
        case 2:
        result = loc[0] + loc[1] * dims[0];
        break;
        
        case 3:
        result = loc[0] + loc[1] * dims[0] + loc[2] * dims[0] * dims[1];
        break;
        
        default:
        {
            long temp;
            result = loc[0];
            
            for (int i=1; i<nDims; i++)
            {
                temp = loc[i];
                for (int j=0; j<i; j++)
                    temp *= dims[j];
                result += temp;
            }
        }
    }
}

void Array::expandIndex (const long &loc, std::vector<int> &result) const
{
    long temp;
    result[0] = loc % dims[0];
    
    for (int i=1; i<nDims; i++)
    {
        temp = 1;
        for (int j=0; j<i; j++)
            temp *= dims[j];
        result[i] = (loc / temp) % dims[i];
    }
}
