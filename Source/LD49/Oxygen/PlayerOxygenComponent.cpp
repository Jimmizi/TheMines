#include "LD49/Oxygen/PlayerOxygenComponent.h"
#include "LD49/LD49Character.h"


UPlayerOxygenComponent::UPlayerOxygenComponent()
{	
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerOxygenComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerOxygenComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bDeathTriggered)
	{
		OxygenLevel -= DeltaTime * OxygenDepleteRate;
		if (OxygenLevel <= 0)
		{
			OxygenLevel = 0;
			TriggerDeath();
		}
	}
}

void UPlayerOxygenComponent::TriggerDeath()
{
	bDeathTriggered = true;
	if (auto* Character = Cast<ALD49Character>(GetOwner()))
	{		
		if (!Character->IsCharacterDead())
		{
			Character->TriggerDeath(EDeathEffect::Suffocate);
		}
	}
}

UPlayerOxygenComponent* UPlayerOxygenComponent::GetPlayerOxygenComponent(const UObject* WorldContext)
{
	if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContext))	
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			if (APawn* Pawn = PlayerController->GetPawn())
			{
				Pawn->FindComponentByClass<UPlayerOxygenComponent>();
			}
		}
	}

	return	nullptr;
}

