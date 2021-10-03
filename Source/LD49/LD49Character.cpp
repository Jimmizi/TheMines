// Copyright Epic Games, Inc. All Rights Reserved.

#include "LD49Character.h"

#include <queue>

#include "DrawDebugHelpers.h"
#include "GameService.h"
#include "LD49Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Components/SphereComponent.h"
#include "Interaction/InteractableActor.h"
#include "LD49/Oxygen/PlayerOxygenComponent.h"


DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ALD49Character

ALD49Character::ALD49Character()
	: IInteractor(this)
{
	m_eInteractorType = InteractorType::InstigatorOnly;

	InteractableArea = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerArea"));
	InteractableArea->SetupAttachment(RootComponent);
	
	InteractableArea->SetCollisionResponseToAllChannels(ECR_Ignore);

	InteractableArea->SetCollisionObjectType(COLLISION_INTERACTION);
	InteractableArea->SetCollisionResponseToChannel(COLLISION_INTERACTION, ECR_Overlap);
	InteractableArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	InteractableArea->OnComponentBeginOverlap.AddDynamic(this, &ALD49Character::OnOverlapBegin);
	InteractableArea->OnComponentEndOverlap.AddDynamic(this, &ALD49Character::OnOverlapEnd);
	
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	PrimaryActorTick.bCanEverTick = true;

	OxygenComponent = CreateDefaultSubobject<UPlayerOxygenComponent>(TEXT("OxygenComponent"));
	
	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void ALD49Character::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IInteractor* pOther = Cast<IInteractor>(OtherActor);
	if(pOther && std::find(m_NearbyInteractors.begin(), m_NearbyInteractors.end(), pOther) == m_NearbyInteractors.end())
	{
		m_NearbyInteractors.push_back(pOther);
	}
}

void ALD49Character::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(IInteractor* pOther = Cast<IInteractor>(OtherActor))
	{
		const auto iter = std::find(m_NearbyInteractors.begin(), m_NearbyInteractors.end(), pOther);
		if (iter != m_NearbyInteractors.end())
		{
			m_NearbyInteractors.erase(iter);
		}
	}
}

void ALD49Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}

//////////////////////////////////////////////////////////////////////////
// Input

void ALD49Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ALD49Character::TryStartInteraction);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ALD49Character::StopAllInteractions);

	// Bind fire event
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ALD49Character::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ALD49Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALD49Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ALD49Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ALD49Character::LookUpAtRate);
}

int ALD49Character::GetIndexOfBestInteractor() const
{
	const float INNER_ANGLE_TRESHOLD_FOR_DIST = 0.5f;
	
	std::priority_queue<std::pair<float, int>> sortedScores;
	std::priority_queue<std::pair<float, int>, std::vector<std::pair<float, int>>, std::greater<>> sortedDistances;

	FCollisionQueryParams traceParams = FCollisionQueryParams(FName(TEXT("Interact_Trace")), false, this);
 
	FVector vPlayerPosition = GetActorLocation();
	
	for (int i = 0; i < m_NearbyInteractors.size(); ++i)
	{
		const AInteractableActor* pInteractorActor = dynamic_cast<AInteractableActor*>(m_NearbyInteractors[i]);
		if (pInteractorActor)
		{
			FVector normForward = GetActorForwardVector();
			normForward.Normalize(0);

			FVector dir = pInteractorActor->GetActorLocation() - GetActorLocation();
			dir.Normalize(0);

			float dotProd = FVector::DotProduct(normForward, dir);

			//Do not add interactors behind the character
			if (dotProd < 0)
			{
				continue;
			}

			if(!pInteractorActor->SkipLineOfSightCheck)
			{
				//Re-initialize hit info
				FHitResult hitResult(ForceInit);
     
				//call GetWorld() from within an actor extending class
				if(GetWorld()->LineTraceSingleByChannel( hitResult, vPlayerPosition, pInteractorActor->GetInteractionPosition(), ECC_WorldStatic, traceParams))
				{
					continue;
				}
			}
			
			//Add all distances within the threshold. Use this to determine an
			//	overwhelming closeness to one of the interactors to prioritise that.
			if(dotProd >= INNER_ANGLE_TRESHOLD_FOR_DIST)
			{
				sortedDistances.emplace(GetDistanceTo(pInteractorActor), i);
			}
			
			sortedScores.emplace(dotProd, i);
		}
	}
	
	if (sortedDistances.size() > 1)
	{
		//TODO Perhaps think about using the score to further weight the best dot product result
		
		auto bestDistance = sortedDistances.top();
		sortedDistances.pop();

		//If the best distance is over a threshold of 30 units from the second best distance
		//	then use the best distance instead of the best angled towards (as we're next to it)
		if(sortedDistances.top().first - bestDistance.first > 30)
		{
			return bestDistance.second;
		}
		
	}

	return !sortedScores.empty() ? sortedScores.top().second : -1;
}

void ALD49Character::TryStartInteraction()
{
	if(m_NearbyInteractors.empty())
	{
		return;
	}
	else
	{
		const int bestIndex = GetIndexOfBestInteractor();
		if(bestIndex > -1)
		{
			GameService::Interaction().CreateInteraction({ this, m_NearbyInteractors[bestIndex] });
			//m_NearbyInteractors.erase(m_NearbyInteractors.begin() + bestIndex);
		}
	}
}

void ALD49Character::StopAllInteractions()
{
    for(IInteractor* interactor : m_NearbyInteractors)
    {
        if (interactor)
        {
            GameService::Interaction().StopInteraction({*this, *interactor});
        }
    }
}

void ALD49Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(!m_NearbyInteractors.empty())
	{
		const int iBestInteractor = GetIndexOfBestInteractor();
		for(int i = 0; i < m_NearbyInteractors.size(); ++i)
		{
			if (AInteractableActor* pActor = dynamic_cast<AInteractableActor*>(m_NearbyInteractors[i]))
			{
				pActor->UpdateNearbyInteractor();
				
				DrawDebugLine(GetWorld(), GetActorLocation(), pActor->GetInteractionPosition(),
					iBestInteractor == i ? pActor->IsInteracting() ? FColor::Yellow : FColor::Green : FColor::Red, false, -1, 0, 1);
			}
		}
	}
}

void ALD49Character::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ALD49Character::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ALD49Character::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void ALD49Character::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void ALD49Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ALD49Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ALD49Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ALD49Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ALD49Character::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ALD49Character::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ALD49Character::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ALD49Character::TouchUpdate);
		return true;
	}
	
	return false;
}

void ALD49Character::TriggerDeath_Implementation(EDeathEffect deathEffect)
{
	bIsCharacterDead = true;
	OnCharacterDied.Broadcast();
}