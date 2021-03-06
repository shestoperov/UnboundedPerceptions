// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "TriggerInteractable.h"
#include "RTSGameMode.h"

ATriggerInteractable::ATriggerInteractable()
{
	PrimaryActorTick.bCanEverTick = false;
	scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(scene);
	staticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	staticMesh->SetupAttachment(RootComponent);
	staticMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3); //Interactable
}

void ATriggerInteractable::BeginPlay()
{
	Super::BeginPlay();
	gameModeRef = Cast<ARTSGameMode>(GetGameInstance());
}

void ATriggerInteractable::Interact_Implementation(ABaseHero* hero)
{
	gameModeRef->GetTriggerManager()->ActivateTrigger(triggerActivatedOnInteract);
}

FVector ATriggerInteractable::GetInteractableLocation_Implementation(ABaseHero* hero)
{
	return GetActorLocation();
}

bool ATriggerInteractable::CanInteract_Implementation()
{
	for(FConditionData condition : useConditions)
	{
		if(!gameModeRef->GetConditionalManager()->GetCondition(condition))
		{
			return false;
		}
	}
	return true;
}




