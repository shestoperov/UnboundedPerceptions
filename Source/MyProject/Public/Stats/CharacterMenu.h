// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/UserWidgetExtensions/MyUserWidget.h"
#include "CharacterMenu.generated.h"

/**
 * Widget for seeing character stats 
 */

class ABaseHero;

UCLASS()
class MYPROJECT_API UCharacterMenu : public UMyUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true), Category = "References")
	ABaseHero*			baseHeroRef = nullptr;

	bool				OnWidgetAddToViewport_Implementation() override;
};
