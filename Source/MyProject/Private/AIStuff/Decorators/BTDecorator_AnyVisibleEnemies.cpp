// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "Decorators/BTDecorator_AnyVisibleEnemies.h"
#include "Unit.h"
#include "UserInput.h"


UBTDecorator_AnyVisibleEnemies::UBTDecorator_AnyVisibleEnemies()
{
	NodeName = "Any Visible Enemies";
	// For now, don't allow users to select any "Abort Observers", because it's currently not supported.
	bAllowAbortNone = false;
	bAllowAbortLowerPri = false;
	bAllowAbortChildNodes = false;
}

bool UBTDecorator_AnyVisibleEnemies::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{

	//if we see at least one enemy we've seen something
	//this also means one enemy sees us (if enemy has some kind of ward we'll treat it as a visible enemy)
	return Cast<AUserInput>(GetWorld()->GetFirstPlayerController())->visibleEnemies.Num();

}

void UBTDecorator_AnyVisibleEnemies::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);
}

FString UBTDecorator_AnyVisibleEnemies::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *CachedDescription);
}

#if WITH_EDITOR

void UBTDecorator_AnyVisibleEnemies::BuildDescription()
{
	CachedDescription = "";
}

void UBTDecorator_AnyVisibleEnemies::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property == NULL)
	{
		return;
	}

	BuildDescription();
}

#endif

