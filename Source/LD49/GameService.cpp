// Fill out your copyright notice in the Description page of Project Settings.

#include "GameService.h"

Service<AInteractionManager> GameService::Interaction;

GameService::GameService()
{
}

GameService::~GameService()
{
}

void GameService::ResetAll()
{
	Interaction.Reset();
}
