// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/InteractionManager.h"

template <class T>
struct Service
{
public:
	T& operator ()()
	{
		checkf(m_ptr, TEXT("%s is trying to be accessed but has not been assigned."), TNameOf<T>().GetName());
		return *m_ptr;
	}

	void operator =(T* data)
	{
		checkf(!m_ptr, TEXT("%s is trying to be provided but already has been."), TNameOf<T>().GetName());
		m_ptr = data;
	}

	void Reset() { m_ptr = nullptr; }

private:
	T* m_ptr = nullptr;
};

class LD49_API GameService
{
public:
	GameService();
	~GameService();

	static void ResetAll();
	
	static Service<AInteractionManager> Interaction;
};
