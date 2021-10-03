// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactor.h"
// Add default functionality here for any IInteractor functions that are not pure virtual.

IInteractor::IInteractor(UObject* derived)
{
	m_pSelf = derived;
}

bool IInteractor::CanInteractWith() const
{
	return m_canBeInteracted && (m_eInteractorType == InteractorType::InteractableOnly || m_eInteractorType == InteractorType::Both);
}

bool IInteractor::InteractionFinished() const
{
	return m_bInteractionComplete;
}

void IInteractor::UpdateNearbyInteractor()
{
	Execute_OnInteractionNearbyUpdate(m_pSelf);
}

bool IInteractor::IsInteracting() const
{
	return m_bPerformingInteraction;
}

void IInteractor::StartInteraction()
{
	m_bPerformingInteraction = true;
	Execute_OnInteractionStart(m_pSelf);
}

void IInteractor::UpdateInteraction()
{
	Execute_OnInteractionUpdate(m_pSelf);
}

void IInteractor::EndInteraction()
{
	Execute_OnInteractionEnd(m_pSelf);
	m_bPerformingInteraction = false;
	m_bInteractionComplete = false;
}

void IInteractor::SetInteractionDone_Implementation()
{
	m_bInteractionComplete = true;
}

void IInteractor::TerminateInteraction()
{
    Execute_SetInteractionDone(m_pSelf);
}
