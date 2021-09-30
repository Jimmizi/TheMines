// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include <vector>

#include "InteractionInstance.h"

#include "InteractionManager.generated.h"

UCLASS()
class LD49_API AInteractionManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractionManager();

	bool CreateInteraction(std::initializer_list<IInteractor*> pInteractors);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	std::vector<TUniquePtr<InteractionInstance>> m_CurrentInteractions;
};
