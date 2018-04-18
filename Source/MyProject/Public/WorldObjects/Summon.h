// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldObjects/Ally.h"
#include "Summon.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ASummon : public AAlly
{
	GENERATED_BODY()
	
public:

	static const int		MAX_NUM_SUMMONS = 25; //max number of summons that can act as allies

	/**Time until this unit expires*/
	UPROPERTY(EditAnywhere, Category = "Summon Info")
	int						timeLeft;
	
	void					BeginPlay() override;
};