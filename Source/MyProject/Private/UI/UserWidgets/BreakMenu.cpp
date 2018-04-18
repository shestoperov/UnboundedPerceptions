// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "BreakMenu.h"
#include "UserInput.h"
#include "UI/HUDManager.h"
#include "MyGameInstance.h"
#include "RTSGameMode.h"

void UBreakMenu::Construct_Implementation()
{
	Super::Construct_Implementation();
	gameModeRef = Cast<ARTSGameMode>(GetWorld()->GetAuthGameMode());
	gameInstanceRef = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
}

void UBreakMenu::Resume()
{
	CPC->GetHUDManager()->AddHUD(static_cast<int>(HUDs::HS_Break));
}

void UBreakMenu::SaveLoad()
{
	CPC->GetHUDManager()->AddHUD(static_cast<uint8>(HUDs::HS_SaveLoad));
	Resume();
}

void UBreakMenu::Options()
{
	CPC->GetHUDManager()->AddHUD(static_cast<int>(HUDs::HS_Settings));
	Resume();
}

void UBreakMenu::Exit()
{
	gameInstanceRef->LoadLevelAsync(gameModeRef->GetStartingLevelName());
}
