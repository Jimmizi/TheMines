// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/BoxComponent.h"

#include "Interactor.generated.h"

#define COLLISION_INTERACTION		ECC_GameTraceChannel1

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UInteractor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LD49_API IInteractor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	IInteractor() = default;
	IInteractor(UObject* derived);

	/// <summary>
	/// The type of interactions this Interactor can do.
	/// </summary>
	enum class InteractorType
	{
		// Can only be interacted with, cannot start interactions.
		InteractableOnly,

		// Can only start interactions, cannot be interacted with.
		InstigatorOnly,

		// Can both start interactions and be interacted with.
		Both
	};
	
	/// <summary>
	/// Checks if the interactor has the ability to be interacted with.
	/// </summary>
	/// <returns>True if able to be interacted with</returns>
	inline bool CanInteractWith() const;

	inline bool InteractionFinished() const;

	void UpdateNearbyInteractor();
	
	/// <summary>
	/// Checks if the interactor is currently able to be interacted with.
	/// </summary>
	/// <remarks>
	/// This only checks to see if they are free to be interacted with. 
	/// Use CanInteractWith() to check if they can be interacted with at all.
	/// </remarks>
	/// <returns>True if the interactor is free to be interacted with</returns>
	bool IsInteracting() const;

	void StartInteraction();
	void UpdateInteraction();
	void EndInteraction();

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnInteractionNearbyUpdate();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnInteractionStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnInteractionUpdate();

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnInteractionEnd();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void SetInteractionDone();

	virtual void SetInteractionDone_Implementation();

	/// <summary>
	/// The type of interactions I am able to process
	/// </summary>
	InteractorType m_eInteractorType;

protected:

private:

	bool m_bPerformingInteraction;
	bool m_bInteractionComplete;

	UObject* m_pSelf;
};
