// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactor.h"

#include "InteractableActor.generated.h"

UCLASS()
class LD49_API AInteractableActor : public AActor, public IInteractor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable", meta = (AllowPrivateAccess = "true"))
	float InteractionTraceOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* InteractableArea;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SceneRoot;
	
public:	
	// Sets default values for this actor's properties
	AInteractableActor();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable", meta = (AllowPrivateAccess = "true"))
	bool Interacting;
	/** called when something enters the sphere component */
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** called when something leaves the sphere component */
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool ShouldTickIfViewportsOnly() const override { return true; }
	FVector GetInteractionPosition() const;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	bool m_bHasOpened;
};
