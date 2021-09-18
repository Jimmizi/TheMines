// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionManager.h"

#include "LD49/GameService.h"

// Sets default values
AInteractionManager::AInteractionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

bool AInteractionManager::CreateInteraction(std::initializer_list<IInteractor*> pInteractors)
{
	for(IInteractor* pInteractor : pInteractors)
	{
		if(pInteractor->IsInteracting())
		{
			return false;
		}
	}
	
	m_CurrentInteractions.push_back(MakeUnique<InteractionInstance>());
	InteractionInstance* pInstance = m_CurrentInteractions[m_CurrentInteractions.size()-1].Get();

	for(IInteractor* pInteractor : pInteractors)
	{
		pInstance->AddInteractor(pInteractor);
	}

	pInstance->Initialise();
	return true;
}

// Called when the game starts or when spawned
void AInteractionManager::BeginPlay()
{
	GameService::Interaction = this;
	
	Super::BeginPlay();
}

// Called every frame
void AInteractionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for(int i = m_CurrentInteractions.size()-1; i >= 0; --i)
	{
		if(m_CurrentInteractions[i]->Update())
		{
			m_CurrentInteractions[i].Reset();
			m_CurrentInteractions.erase(m_CurrentInteractions.begin() + i);
		}
	}
}

