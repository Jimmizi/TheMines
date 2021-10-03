// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <vector>
#include "Interactor.h"

/**
 * 
 */
class LD49_API InteractionInstance
{
public:
	InteractionInstance();
	~InteractionInstance();

	void AddInteractor(IInteractor* pInteractor);

	void Initialise();
	bool Update();
	void End();
	
    const std::vector<IInteractor*>& GetInteractors() const;

private:
	std::vector<IInteractor*> m_Interactors;
};
