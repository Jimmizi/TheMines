// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableActor.h"

#include "DrawDebugHelpers.h"

// Sets default values
AInteractableActor::AInteractableActor()
	: IInteractor(this)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_eInteractorType = InteractorType::InteractableOnly;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SceneRoot->SetupAttachment(RootComponent);
	
	InteractableArea = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerArea"));
	//InteractableArea->SetupAttachment(SceneRoot);
	
	InteractableArea->SetCollisionResponseToAllChannels(ECR_Ignore);

	InteractableArea->SetCollisionObjectType(COLLISION_INTERACTION);
	InteractableArea->SetCollisionResponseToChannel(COLLISION_INTERACTION, ECR_Overlap);
	InteractableArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	InteractableArea->OnComponentBeginOverlap.AddDynamic(this, &AInteractableActor::OnOverlapBegin);
	InteractableArea->OnComponentEndOverlap.AddDynamic(this, &AInteractableActor::OnOverlapEnd);
	TArray<UActorComponent*> ParticleComponents = GetComponentsByClass(UBoxComponent::StaticClass());

//void GetComponents(TSubclassOf<UActorComponent> ComponentClass, TArray<UActorComponent*, AllocatorType>& OutComponents, bool bIncludeFromChildActors = false) const
	
	TArray<UBoxComponent*> MyComponents;
	GetComponents(MyComponents);
	
	for(UActorComponent* pComp : MyComponents)
	{
		if(UBoxComponent* pBox = dynamic_cast<UBoxComponent*>(pComp))
		{
			pBox->SetCollisionResponseToAllChannels(ECR_Ignore);

			pBox->SetCollisionObjectType(COLLISION_INTERACTION);
			pBox->SetCollisionResponseToChannel(COLLISION_INTERACTION, ECR_Overlap);
			pBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			if(pBox->GetName().Equals(NameOfBoxComponentToBeParent))
			{
				pBox->SetupAttachment(SceneRoot);
			}
		}
	}
}

void AInteractableActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void AInteractableActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

FVector AInteractableActor::GetInteractionPosition() const
{
	return GetActorLocation() + (GetActorForwardVector() * InteractionTraceOffset);
}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AInteractableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if WITH_EDITOR
	DrawDebugSphere(GetWorld(), GetInteractionPosition(), 2.5f, 8, FColor::Red);
	DrawDebugString(GetWorld(), GetInteractionPosition() + FVector(0.0f, 0.0f, 20.0f), (IsInteracting() ? TEXT("Is In Use") : TEXT("Is Free")), nullptr, FColor::White, 0.0001f);
#endif
}

