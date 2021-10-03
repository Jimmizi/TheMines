// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerOxygenComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LD49_API UPlayerOxygenComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static UPlayerOxygenComponent* GetPlayerOxygenComponent(const UObject* WorldContext);

public:
	UPlayerOxygenComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	

private:
	void TriggerDeath();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxOxygenLevel = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OxygenLevel = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float OxygenDepleteRate = 1.0f;
};
