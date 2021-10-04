// Fill out your copyright notice in the Description page of Project Settings.
#include "InteractionManager.h"
#include <set>

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
	
    InteractionInstance& created = m_currentInteractions.emplace_back();

	for(IInteractor* pInteractor : pInteractors)
	{
		created.AddInteractor(pInteractor);
	}

	created.Initialise();
	return true;
}

void AInteractionManager::StopInteraction(std::initializer_list<InteractorElement> interactors)
{
    std::set<IInteractor*> found;
    for(IInteractor& other : interactors)
    {
        for (InteractionInstance& iter : m_currentInteractions)
        {
            for(IInteractor* instance : iter.GetInteractors())
            {
                if (instance == &other)
                {
                    found.insert(instance);
                    break;
                }                
            }
        }
    }
    
    for(IInteractor* instance : found)
    {
        instance->EndInteraction();
    }
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

	for(int i = m_currentInteractions.size()-1; i >= 0; --i)
	{
		if(m_currentInteractions[i].Update())
		{
			m_currentInteractions.erase(m_currentInteractions.begin() + i);
		}
	}
}

