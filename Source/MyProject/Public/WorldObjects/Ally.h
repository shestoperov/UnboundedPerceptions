// Fill out your copyright notice in the Description page of Project Settings.

//RIGHT NOW WE'rE DEALING WITH A PROBLEM IF THEY STOP WIHLE CASTING THEY COULD HAVE A NULL SPELL FIX IT!!!
 
#pragma once

#include "Unit.h"
#include "Ally.generated.h"

/*
 * Class for all controlllable allied units
*/

class AUserInput;
class AAllyAIController;
class AEnemy;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpellCasted); //delegate for reacting to spell casts, including setting cd animations

/*
union FunctorParams
{
	FVector moveLocation;
	AUnit* target;
};*/

UCLASS()
class MYPROJECT_API AAlly : public AUnit
{
	GENERATED_BODY()

	/*---Help Text---*/
	static const FText							notEnoughManaText;
	static const FText							invalidTargetText;
	static const FText							onCooldownText;
	static const FText							stunnedText;
	static const FText							filledQueueText;

	TQueue<TFunction<void()>, EQueueMode::Spsc> commandQueue; //queue of actions this unit has queued up
	int											queueCount = 0; //just a soft check for buffer overflow

	//object query that only includes visionblockers
	FCollisionObjectQueryParams					queryParamVision;

	UPROPERTY(BlueprintSetter = SetSpellIndex, BlueprintGetter = GetSpellIndex, Category = "Spells")
	int											spellIndex = -1; //spellIndex to remember what slot was used so we can set it on CD after casting spell.  Don't make it part of begincast because items don't need it

	AAllyAIController*							allyControllerRef;

public:

	static const int							MAX_NUM_SPELLS = 6; //max spells an ally can have in their actionbar

	UPROPERTY(BlueprintReadOnly)
	AUserInput*									controllerRef = nullptr;

	AAlly(const FObjectInitializer& oI);

	void										BeginPlay() override;
	void										Tick(float deltaSeconds) override;
	void										Destroyed() override;
	void										PossessedBy(AController* newAllyControllerRef) override;
#pragma region accessors
	
	//polymorphic selection override for caching units in basePlayer
	UFUNCTION(BlueprintCallable, Category = "Accessors") 
	void										SetSelected(bool value) override; 
	
	//Get the class of the spell at this slot (CHECKED INDEX ACCESS)
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Spells") 
	TSubclassOf<UMySpell>						GetSpellAtSlot(int index) const { if (index >= 0 && index < abilities.Num()) return abilities[index]; return TSubclassOf<UMySpell>(); }
	
	//Gets CDO of any spell.  CDOs of objects can only be given a struct, this function lets us get the CDO as the type of the object.  Also checks to see if hero has spell, else it won't work
	//Trying to get instances only returns active instances, that is, spells that are marked "Active" and haven't been committed because they are on pause.  Use GetSpellInstance to get actual instance
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Spells") 
	UMySpell*									GetSpellCDO(TSubclassOf<UMySpell> spellClass) const;

	//Sees if there's any active instances of a spell and gets them -- Used to get current spell CD timer
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Spells") 
	UGameplayAbility*							GetSpellInstance(TSubclassOf<UMySpell> spellClass) const;

	UFUNCTION(BlueprintPure, BlueprintGetter, Category = "Spells")
	int											GetSpellIndex() const { return spellIndex; }

	UFUNCTION(BlueprintSetter, Category = "Spells") 
	void										SetSpellIndex(int index);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "AI")
	AAllyAIController*							GetAllyAIController();
#pragma endregion
/*---Utility---*/

	/**Check to see if things are above us so we know to make the roofs transparent as we walk underneath them*/
	UFUNCTION(BlueprintCallable, Category = "Overlap")
	bool										GetOverlappingObjects(TArray<FHitResult>& hits);

	/**From a raycast (left click) we can test to see if the target clicked on is a proper target for our spell*/
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool										SetupSpellTargetting(FHitResult result, TSubclassOf<UMySpell> spellClass);

	void										QueueAction(TFunction<void()> actionToQueue); //Queues an action to our action queue

	//Accessor to clear command queue.  
	void										ClearCommandQueue() { commandQueue.Empty(); }


/*---Actions---*/
#pragma region actions

	///Stop everything we're doing...  Doesn't have any extra effects in allies
	UFUNCTION(BlueprintCallable, Category = "Misc") 
	virtual void								Stop() override;

	///Like CastSpell but triggers cd on actionbar
	bool										CastSpell(TSubclassOf<UMySpell> spellToCast) override;

	//When spell hotkey is presssed down.  Returns true when spell sucessfully set up or casted
	UFUNCTION(BlueprintCallable, Category = "Spells") 
	bool										PressedCastSpell(TSubclassOf<UMySpell> spellToCast); 

	UFUNCTION(BlueprintCallable, Category = "Spells")
	virtual bool								BeginCastSpell(int spellToCastIndex, FGameplayAbilityTargetDataHandle targetData) override;
#pragma endregion 

//Allies use a mechanic of vision to see what they can target.  Enemies may use some kind of trigger/perception component to start doing something
#pragma region Vision

	/**What enemies are in our radius determined via sphere overlap events*/
	UPROPERTY(BlueprintReadWrite, Category = "Vision")
	TArray<AEnemy*>								possibleEnemiesInRadius; 
#pragma endregion
	/*
	FHitResult visionTestRes;

	TArray<AActor*> hitEnemies;
	
	TArray<AActor*> hiddenEnemies;

	//only check corners of our building that are in range
	TArray<FVector> inRangeCorners;

	//length multiplier to see if triangle side is 
	TArray<float> triangleTrueLengthMultiplier;

	void UpdateVisibleEnemies();

	//XY corners of buildings used since we don't worry about vision blocking in Z.
	const FVector buildingCorners[] = {
		FVector(1.f, 1.f, 0.f),
		FVector(1.f, -1.f, 0.f),
		FVector(-1.f, -1.f, 0.f),
		FVector(-1.f, 1.f, 0.f)
	};

	*/


	friend void									SetupAlliedUnits();
};



