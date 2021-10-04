#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LD49GameUtils.generated.h"

UCLASS()
class LD49_API ULD49GameUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintCallable, Category="GameUtils")
    static FVector QuantizeToGrid(const FVector& input, const float gridSize = 2.f, const FVector gridOffset = FVector::ZeroVector);
};
