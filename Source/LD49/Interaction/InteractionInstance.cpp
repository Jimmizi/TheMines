// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionInstance.h"

InteractionInstance::InteractionInstance()
{
}

InteractionInstance::~InteractionInstance()
{
	m_Interactors.clear();
}

void InteractionInstance::AddInteractor(IInteractor* pInteractor)
{
	m_Interactors.push_back(pInteractor);
}

void InteractionInstance::Initialise()
{
	for(IInteractor* pInteractor : m_Interactors)
	{
		pInteractor->StartInteraction();
	}
}

bool InteractionInstance::Update()
{
	bool bInteractorStillUpdating = false;
	
	for (IInteractor* pInteractor : m_Interactors)
	{
		if (!pInteractor->InteractionFinished())
		{
			pInteractor->UpdateInteraction();
			bInteractorStillUpdating = true;
		}
	}

	if(!bInteractorStillUpdating)
	{
		End();
		return true;
	}

	return false;
}

void InteractionInstance::End()
{
	for (IInteractor* pInteractor : m_Interactors)
	{
		pInteractor->EndInteraction();
	}
}