#include "LD49GameUtils.h"

FVector ULD49GameUtils::QuantizeToGrid(const FVector& input, const float gridSize, const FVector gridOffset)
{
    // This is hacky because it will work only on sizes greater than zero, but it will do.
    int x = input.X;
    int y = input.Y;
    
    const int gridSizeInt = gridSize;
    
    if (gridSizeInt != 0)
    {
        x = x + (x % gridSizeInt);
        y = y + (y % gridSizeInt);
    }
    
    return {static_cast<float>(x) + gridOffset.X, static_cast<float>(y) + gridOffset.Y, input.Z + gridOffset.Z};
}
