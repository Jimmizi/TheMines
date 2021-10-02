// Copyright Epic Games, Inc. All Rights Reserved.

#include "LD49TestNewClass.h"
#include "Components/SphereComponent.h"

DEFINE_LOG_CATEGORY(LogFsm);

ALD49TestNewClass::ALD49TestNewClass() 
    : m_fsm(0)
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ALD49TestNewClass::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
    SetActorTickEnabled(true);
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bTickEvenWhenPaused = true;
    PrimaryActorTick.TickGroup = TG_PrePhysics;
}

void ALD49TestNewClass::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}

void ALD49TestNewClass::OnStart ( const int value, const float time )
{
    m_start = true;
    m_timer = time;
    m_someOtherValue = 0;
    
    m_fsm.SetState<IdleState>();
}

void ALD49TestNewClass::Tick ( float time )
{
    m_fsm.Process(time, *this);
}

// //====================\\
// || BEGIN OF IdleState ||
// \\====================//

ALD49TestNewClass::IdleState::IdleState ( )
{
    UE_LOG(LogFsm, Log, TEXT("Idle State created."));
}

void ALD49TestNewClass::IdleState::ProcessFSM( const float dt, ALD49TestNewClass& master )
{
    if (master.m_start)
    {
        UE_LOG(LogFsm, Log, TEXT("Idle State Processing."));
        master.m_fsm.Continue<ProcessingState>(master.m_someOtherValue, master.m_timer);
    }
}

// //==================\\
// || END OF IdleState ||
// \\==================//

// //==========================\\
// || BEGIN OF ProcessingState ||
// \\==========================//

ALD49TestNewClass::ProcessingState::ProcessingState ( const int thatValue, const float time )
    : m_moarValue(thatValue)
    , m_timer(time)
{
   UE_LOG(LogFsm, Log, TEXT("Processing State created.")); 
}

void ALD49TestNewClass::ProcessingState::ProcessFSM( const float dt, ALD49TestNewClass& master )
{
    const float normalisedTime = (1.f - (m_timer / master.m_timer)) * 100.f;
    UE_LOG(LogFsm, Log, TEXT("Processing: %f."), m_timer);
    m_timer -= dt;
    if (m_timer < 0.f)
    {
        UE_LOG(LogFsm, Log, TEXT("Processing State Switching."));
        master.m_fsm.Continue<EndState>(m_moarValue);
    }
}

ALD49TestNewClass::ProcessingState::~ProcessingState()
{
    UE_LOG(LogFsm, Log, TEXT("Processing State Ended."));
}


// //========================\\
// || END OF ProcessingState ||
// \\========================//

// //===================\\
// || BEGIN OF EndState ||
// \\===================//

ALD49TestNewClass::EndState::EndState ( const int finalValue )
    : m_endingValue(finalValue)
{
    UE_LOG(LogFsm, Log, TEXT("End. Value: %d"), finalValue);
}

void ALD49TestNewClass::EndState::ProcessFSM( const float dt, ALD49TestNewClass& master )
{
    UE_LOG(LogFsm, Log, TEXT("Annoying..."));
    master.m_someOtherValue = m_endingValue;
}

// //=================\\
// || END OF EndState ||
// \\=================//
