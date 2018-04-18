// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "WorldObjects/Unit.h"
#include "UnitController.h"
#include "UserInput.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "SpellSystem/MySpell.h"
#include "EnvironmentQuery/Generators/EnvQueryGenerator_OnCircle.h"

AUnitController::AUnitController()
{
	//Component Init
	behaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	blackboardComp = CreateDefaultSubobject<UBlackboardComponent>(FName("BlackboardComp"));
}

void AUnitController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	ownerRef = Cast<AUnit>(GetPawn());
	CPCRef = Cast<AUserInput>(GetWorld()->GetFirstPlayerController());
}

void AUnitController::FindBestAOELocation(int spellIndex)
{
	FEnvQueryRequest queryRequest = FEnvQueryRequest(findBestAOELocation, this);
	queryRequest.SetFloatParam("CircleRadius", GetUnitOwner()->abilities[spellIndex].GetDefaultObject()->GetAOE(GetUnitOwner()->GetAbilitySystemComponent()));
	spellToCastIndex = spellIndex; 
	queryRequest.Execute(EEnvQueryRunMode::SingleResult, this, &AUnitController::OnAOELocationFound);
}

void AUnitController::FindWeakestTarget(int spellIndex)
{
	FEnvQueryRequest queryRequest = FEnvQueryRequest(findWeakestTarget, this);
	spellToCastIndex = spellIndex;
	queryRequest.Execute(EEnvQueryRunMode::SingleResult, this, &AUnitController::OnWeakestTargetFound);
}

void AUnitController::OnAOELocationFound(TSharedPtr<FEnvQueryResult> result)
{
	FGameplayAbilityTargetingLocationInfo tInfo;
	tInfo.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	tInfo.LiteralTransform = FTransform(result->GetItemAsLocation(0));
	FGameplayAbilityTargetDataHandle targetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromLocations(tInfo, tInfo); //start point isn't really used so we can put this twice
	GetUnitOwner()->BeginCastSpell(spellToCastIndex, targetData);
}

void AUnitController::OnWeakestTargetFound(TSharedPtr<FEnvQueryResult> result)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, TEXT("FOUND WEAKEST TARGET!"));
	FGameplayAbilityTargetDataHandle targetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(result->GetItemAsActor(0));
	GetUnitOwner()->BeginCastSpell(spellToCastIndex, targetData);
}


