// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "utils/fsm.h"
#include "LD49TestNewClass.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFsm, Log, All);

class USphereComponent;

UCLASS(config=Game)
class ALD49TestNewClass : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly)
	USphereComponent* CollisionComp;

public:
	ALD49TestNewClass();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	
	UFUNCTION(BlueprintCallable)
    void OnStart(const int value, const float time);
    
    virtual void Tick(float time) override;
    
    UPROPERTY(BlueprintReadOnly);
    int m_someOtherValue{0};
private:
    float m_timer{0.f};
    bool m_start{false};
    
    struct IdleState
    {
        IdleState();
        void ProcessFSM(const float dt, ALD49TestNewClass& master);
    };

    struct ProcessingState
    {
        ProcessingState(const int thatValue, const float time);
        void ProcessFSM(const float dt, ALD49TestNewClass& master);
        ~ProcessingState();
    private:
        int m_moarValue{0};
        float m_timer{0.f};
    };

    struct EndState
    {
        EndState(const int finalValue);
        void ProcessFSM(const float dt, ALD49TestNewClass& master);
    private:
        int m_endingValue{0};
    };
        
    using fsm_t = static_fsm<IdleState, ProcessingState, EndState>;
    
    fsm_t m_fsm;
};


