// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LD49GameMode.generated.h"



UCLASS(minimalapi)
class ALD49GameMode : public AGameModeBase
{
	GENERATED_BODY()

	static const int MAX_LANTERN_FUEL = 60;

public:
	ALD49GameMode();

	UFUNCTION(BlueprintCallable, Category = "LD49 Game")
	int GetLanternFuelRemaining() const { return m_iLanternFuelRemaining; }

	UFUNCTION(BlueprintCallable, Category = "LD49 Game")
	bool IsLanternFuelEmpty() const { return m_iLanternFuelRemaining <= 0; }

	UFUNCTION(BlueprintCallable, Category = "LD49 Game")
	void AddToLanternFuel(int iAmount) { m_iLanternFuelRemaining += iAmount; }

	UFUNCTION(BlueprintCallable, Category = "LD49 Game")
	void FullyRefuelLantern() { m_iLanternFuelRemaining = MAX_LANTERN_FUEL; }

	UFUNCTION(BlueprintCallable, Category = "LD49 Game")
	bool IsLanternMissingFuel() const { return m_iLanternFuelRemaining < MAX_LANTERN_FUEL; }

private:
	int m_iLanternFuelRemaining;
};



