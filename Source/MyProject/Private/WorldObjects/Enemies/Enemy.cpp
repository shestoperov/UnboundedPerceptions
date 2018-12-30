// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "AIController.h"
#include "RTSGameMode.h"
#include "RTSGameState.h"
#include "UserInput.h"
#include "BasePlayer.h"
#include "Quests/QuestManager.h"
#include "Enemy.h"
#include "BaseCharacter.h"
#include "SpellSystem/MySpell.h"
#include "AIStuff/AIControllers/EnemyAIController.h"
#include "SpellSystem/MyAbilitySystemComponent.h"
#include "NavArea_EnemySpot.h"
#include "../BaseHero.h"

#include "ResourceManager.h"

AEnemy::AEnemy(const FObjectInitializer& oI) : AUnit(oI)
{
   aggroRange = 20;
   state      = TUniquePtr<StateMachine>(new StateMachine(this));
   SetActorHiddenInGame(true); //Set hidden by default so won't be revealed by vision
   GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);
   
   GetCapsuleComponent()->AreaClass = UNavArea_EnemySpot::StaticClass(); //Custom area class so navigation filter for defensive movement will avoid this
   GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel8, ECollisionResponse::ECR_Block); //TraceEnemyOnly
  
   visionSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnVisionSphereOverlap);
   visionSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OnVisionSphereEndOverlap);
   visionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel9, ECollisionResponse::ECR_Overlap); //Friendly
   visionSphere->SetCanEverAffectNavigation(true);
   visionSphere->AreaClass = UNavArea_EnemySpot::StaticClass();
   visionSphere->SetAbsolute(false, false, true);
   visionSphere->bDynamicObstacle = true;
}

void AEnemy::BeginPlay()
{
   Super::BeginPlay();
   controllerRef = Cast<AUserInput>(GetWorld()->GetFirstPlayerController());
   gameModeRef   = Cast<ARTSGameMode>(GetWorld()->GetAuthGameMode());
   gameStateRef  = Cast<ARTSGameState>(GetWorld()->GetGameState());
   gameStateRef->enemyList.Add(this);

   InitializeStats();
}

void AEnemy::Tick(float deltaSeconds)
{
   Super::Tick(deltaSeconds);
}

void AEnemy::Die_Implementation()
{
   gameModeRef->GetQuestManager()->OnEnemyDie(this);
   controllerRef->GetBasePlayer()->UpdateEXP(expGiven);
   controllerRef->GetBasePlayer()->UpdateGold(moneyGiven);
   gameStateRef->enemyList.Remove(this);
   Super::Die_Implementation();
}

void AEnemy::SetSelected(bool value)
{
   Super::SetSelected(value);
   if (value) {
      controllerRef->GetBasePlayer()->focusedUnit = this;
   } else {
      controllerRef->GetBasePlayer()->focusedUnit = this;
   }
}

void AEnemy::OnVisionSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent, int otherBodyIndex, bool fromSweep, const FHitResult& sweepRes)
{
   if (otherActor->IsA(AAlly::StaticClass())) {
      AAlly* ally = Cast<AAlly>(otherActor);
      // TODO: Check for invisibility
      possibleEnemiesInRadius.Add(ally);
      ally->IncVisionCount();
   }
}

void AEnemy::OnVisionSphereEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
   AAlly* ally = Cast<AAlly>(otherActor);
   if (IsValid(ally)) {
      possibleEnemiesInRadius.Remove(ally);
      ally->DecVisionCount();
      if (!ally->GetVisionCount()) { gameState->visibleAllies.Remove(ally); }
   }
}

void AEnemy::InitializeStats()
{
   int index = -1;
   for (Attributes x : initialStats.defaultAttributes) {
      baseC->GetAttribute(static_cast<int>(x))->SetBaseValue(initialStats.defaultValues[++index]);
      baseC->GetAttribute(static_cast<int>(x))->SetCurrentValue(initialStats.defaultValues[index]);
   }

   for (UnitStats x : initialStats.defaultUnitStats) {
      baseC->GetSkill(static_cast<int>(x))->SetBaseValue(initialStats.defaultValues[++index]);
   }

   for (Vitals x : initialStats.defaultVitals) {
      baseC->GetVital(static_cast<int>(x))->SetBaseValue(initialStats.defaultValues[++index]);
      baseC->GetVital(static_cast<int>(x))->SetCurrValue(initialStats.defaultValues[index]);
   }

   for (Mechanics x : initialStats.defaultMechanics) {
      baseC->GetMechanic(static_cast<int>(x))->SetBaseValue(initialStats.defaultValues[++index]);
      baseC->GetMechanic(static_cast<int>(x))->SetCurrentValue(initialStats.defaultValues[index]);
   }
}

float AEnemy::CalculateTargetRisk()
{
   int targetNum = 0;
   for (AAlly* a : controllerRef->GetGameState()->visibleAllies) {
      if (a->GetTarget() == this) targetNum += 1;
   }

   const float targetRiskValue = FMath::Clamp(diminishFunc(targetNum), 0.f, 1.f);
   return targetRiskValue;
}
