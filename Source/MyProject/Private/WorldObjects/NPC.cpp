// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "NPC.h"
#include "BaseHero.h"
#include "RTSGameMode.h"
#include "UserInput.h"
#include "BasePlayer.h"
#include "RTSGameState.h"
#include "UI/HUDManager.h"
#include "DialogSystem/DialogBox.h"
#include "DialogSystem/DialogUI.h"
#include "Quests/QuestManager.h"

// Sets default values
ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = true;
	onDialogEndTriggerData.triggerType = ETriggerType::OpenHUDTrigger;
	onDialogEndTriggerData.enabled = true;

	TArray<FString> triggerValues;
	triggerValues.Add(FString::FromInt(static_cast<uint8>(HUDs::HS_Social)));
	onDialogEndTriggerData.triggerValues = triggerValues;
	onDialogEndTriggerData.numCalls = -1;

	GetCapsuleComponent()->SetCollisionProfileName("NPC");
	GetMesh()->SetCollisionProfileName("NoCollision");
}

void ANPC::BeginPlay()
{
	Super::BeginPlay();
	controllerRef = Cast<AUserInput>(GetWorld()->GetFirstPlayerController());

#if UE_EDITOR
	//Figure out how many quests this NPC can give
	GetWorld()->GetTimerManager().SetTimer(BeginPlayDelayTimer, this, &ANPC::CountQuestDialogs, 2.f, false);
#endif

	//Callback to help maintain quest count even when new topics are learned
	Cast<ABasePlayer>(controllerRef->PlayerState)->OnDialogLearned.AddDynamic(this, &ANPC::OnTopicLearned);

	//Load up all triggers that tried to activate on this object but this object wasn't loaded at the time
	TArray<FTriggerData> savedTriggers;
	Cast<ARTSGameMode>(GetWorld()->GetAuthGameMode())->GetTriggerManager()->GetTriggerRecords().MultiFind(*GetGameName().ToString(), savedTriggers);

	for(FTriggerData& trigger : savedTriggers)
	{
		controllerRef->GetGameMode()->GetTriggerManager()->ActivateTrigger(trigger);
	}
}

// Called every frame
void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANPC::SetSpecificDialog(FGameplayTag topic, FName newConversationName)
{
	//If our this conversationTopic was a quest giving dialog, decrease the available quest count because its being removed
	FName oldConversationName = GetConversationName(topic);

	if(IsQuestDialog(oldConversationName) && IsTopicLearned(topic))
		--numAvailableQuests;
	//If this conversationTopic is a questGiving dialog, increase the quest count because it's the new conversation for this topic
	if(IsQuestDialog(newConversationName) && IsTopicLearned(topic))
		++numAvailableQuests;
	conversationTopics[topic] = newConversationName;
}

FName ANPC::GetConversationName(FGameplayTag conversationTopic) const
{
	//does this NPC know anything about the topic?
	if (conversationTopics.Contains(conversationTopic))
	{
		return conversationTopics[conversationTopic];
	}
	else //if no, return some default reply like "I'm not sure what that is sire!"
	{
		return defaultResponseName;
	}
}

void ANPC::Interact_Implementation(ABaseHero* hero)
{
	GetController()->StopMovement();
	currentlyTalkingHero = hero;

	FVector currentLocation = GetActorLocation();
	FVector difference = hero->GetActorLocation() - currentLocation;
	FVector projectedDirection = FVector(difference.X, difference.Y, 0);
	SetActorRotation(FRotationMatrix::MakeFromX(FVector(projectedDirection)).Rotator());

	//If this npc wants to converse, we go to another screen after the initial conversation where we can interact more
	if (!controllerRef->GetBasePlayer()->interactedHero)
	{
		if (bWantsToConverse)
		{
			controllerRef->GetHUDManager()->GetSocialWindow()->SetNPC(this);
			SetupAppropriateView();
			if(conversationStarterName != "")
				controllerRef->GetHUDManager()->AddHUD(conversationStarterName, TArray<FTriggerData>{onDialogEndTriggerData}, currentlyTalkingHero);
			else
				//they shouldnt have time to talk but not start with some kind of conversation starter
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("FORGOT TO SET A CONVERSATION STARTER!!!"));
		}
		else
		{
			//Just close dialog screen after we're finished talking
			if(conversationStarterName != "")
				controllerRef->GetHUDManager()->AddHUD(conversationStarterName, TArray<FTriggerData>{FTriggerData::defaultTrigger}, currentlyTalkingHero);
			else
			{
				//if there was no entry for conversationStarterName, just display a default one
				controllerRef->GetHUDManager()->AddHUD(TArray<FDialogData>{
					FDialogData({ 0 }, NSLOCTEXT("NPCDialog", "Default", "This person is silent..."), FName())
				}, TArray<FTriggerData>{onDialogEndTriggerData}, currentlyTalkingHero);
			}
		}
		controllerRef->GetGameMode()->GetQuestManager()->OnTalkNPC(this, FGameplayTag());
		AddConversedDialog(conversationStarterName);
	}
}

void ANPC::SetupAppropriateView()
{
	controllerRef->GetHUDManager()->GetSocialWindow()->SetConversationView();
}

FVector ANPC::GetInteractableLocation_Implementation()
{
	return GetActorLocation();
}

bool ANPC::CanInteract_Implementation()
{
	return true;
}

void ANPC::CountQuestDialogs()
{
	//Check if default conversation is a quest
	FName conversationName = defaultResponseName;
	if (conversationName.ToString().StartsWith("Quest"))
		++numAvailableQuests;

	//Loop through all of our dialogTopics we know
	TArray<FGameplayTag> topics;

	//if this NPC wants to converse and we can talk to him/her about more than one topic
	if (bWantsToConverse && conversationTopics.GetKeys(topics) > 0)
	{
		for (FGameplayTag topic : topics)
		{
			if (IsTopicLearned(topic))
			{
				conversationName = GetConversationName(topic);

				if (IsQuestDialog(conversationName))
					++numAvailableQuests;
			}
		}
	}
}

bool ANPC::IsQuestDialog(FName conversationName)
{
	return conversationName.ToString().StartsWith("Quest") ? true : false;
}

bool ANPC::IsTopicLearned(FGameplayTag topic)
{
	return controllerRef->GetBasePlayer()->HasDialogTopic(topic);
}

void ANPC::OnTopicLearned(FGameplayTag topicLearned)
{
	if(IsQuestDialog(GetConversationName(topicLearned)))
	{
		++numAvailableQuests;
	}
}






