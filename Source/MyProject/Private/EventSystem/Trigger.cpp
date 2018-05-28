// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "Trigger.h"
#include "MyGameInstance.h"
#include "ResourceManager.h"
#include "UserInput.h"
#include "BasePlayer.h"
#include "UI/HUDManager.h"
#include "Quests/QuestManager.h"
#include "WorldObjects/NPC.h"
#include "WorldObjects/Unit.h"
#include "WorldObjects/BaseHero.h"

FTriggerData FTriggerData::defaultTrigger = FTriggerData();

FTriggerData::FTriggerData()
{
	
}

void UTriggerManager::ActivateTrigger(UPARAM(ref) FTriggerData& triggerData)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("TRIGGER ACTIVATED!"));
	if (triggerData.enabled)
	{
		if (triggerData.numCalls > 0)
		{
			TriggerEffect(triggerData);
			--triggerData.numCalls;
		}
		else
		{
			if (triggerData.numCalls == -1)
				TriggerEffect(triggerData);
		}
	}
}

void UTriggerManager::ChangeDialog(const FTriggerData& tdata)
{
	check(tdata.triggerObjects.Num() == 1 && tdata.triggerValues.Num() <= 2)
	if(ANPC* npc = ResourceManager::FindTriggerObjectInWorld<ANPC>(tdata.triggerObjects[0], cpcRef->GetWorld()))
	if(tdata.triggerValues.Num() == 1)
		npc->SetCurrentDialog(*tdata.triggerValues[0]);
	else
		npc->SetSpecificDialog(*tdata.triggerValues[0],*tdata.triggerValues[1]);
}

void UTriggerManager::ModifyStats(const FTriggerData& tdata)
{
	check(tdata.triggerObjects.Num() == 1 && tdata.triggerValues.Num() <= 2)
	if (AUnit* unit = ResourceManager::FindTriggerObjectInWorld<AUnit>(tdata.triggerObjects[0], cpcRef->GetWorld()))
		return;
}

void UTriggerManager::OpenHUDTrigger(const FTriggerData& tdata)
{
	checkf(tdata.triggerObjects.Num() == 0 && tdata.triggerValues.Num() == 1, TEXT("Incorrect parameters for OPENHUDTRIGGER"))
	checkf(tdata.triggerValues[0].IsNumeric(), TEXT("OPENHUDTRIGGER triggerValue should be numeric"))
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, tdata.triggerValues[0]);
	cpcRef->GetHUDManager()->AddHUD(FCString::Atoi(*tdata.triggerValues[0]));
}

void UTriggerManager::ChangeParty(const FTriggerData& tdata)
{
	checkf(tdata.triggerObjects.Num() <= 4, TEXT("Incorrect number of parameters %d for ChangePartyTrigger"), tdata.triggerObjects.Num());
	int index = 0;
	TArray<ABaseHero*> newHeroes;

	for(FString heroName : tdata.triggerObjects)
	{
		++index;
		//empty string means no hero at that slot
		if(heroName.IsEmpty())
		{
			break;
		}

		ABaseHero* hero = ResourceManager::FindTriggerObjectInWorld<ABaseHero>(heroName, cpcRef->GetWorld());
		if(hero)
		{
			newHeroes.Add(hero);
		}
	}
	cpcRef->GetBasePlayer()->UpdateParty(newHeroes);
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::FromInt(newHeroes.Num()));
}

void UTriggerManager::ActivateOtherTrigger(const FTriggerData& tdata)
{
}

void UTriggerManager::DeactivateOtherTrigger(const FTriggerData& tdata)
{
}

void UTriggerManager::ChangeTriggerType(const FTriggerData& tdata)
{
}

void UTriggerManager::AddQuest(const FTriggerData& tdata)
{
	checkf(tdata.triggerObjects.Num() == 0 && tdata.triggerValues.Num() == 2, TEXT("Incorrect parameters for OPENHUDTRIGGER"))
	checkf(tdata.triggerValues[1].IsNumeric(), TEXT("ADDQUESTTRIGGER triggerValue 2 should be numeric"))
	gameInstanceRef->GetQuestManager()->AddNewQuest(gameInstanceRef->GetQuestManager()->questClassList[*tdata.triggerValues[0]], FCString::Atoi(*tdata.triggerValues[0]) != 0);
}

void UTriggerManager::TriggerEffect(FTriggerData& triggerData)
{
	#if UE_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::FromInt(static_cast<uint8>(triggerData.triggerType)) + " Type Trigger Activated");
	#endif

	switch (triggerData.triggerType)
	{
		case TriggerType::None: return; break;
		case TriggerType::OpenHUDTrigger: OpenHUDTrigger(triggerData); break;
		case TriggerType::ActivateOtherTrigger: ActivateTrigger(UPARAM(ref) triggerData); break;
		case TriggerType::ChangeDialog: ChangeDialog(triggerData); break;
		case TriggerType::ChangePartyTrigger: ChangeParty(triggerData); break;
		default: UE_LOG(LogTemp, Warning, TEXT("Unknown Trigger type attempted to be activated!"));
	}
}
