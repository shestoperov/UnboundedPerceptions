// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "MinigameManager.h"
#include "RTSPawn.h"
#include "MinigamePawn.h"
#include "RTSGameMode.h"
#include "UI/HUDManager.h"
#include "UserInput.h"

void UMinigameManager::Init()
{
   controllerRef = Cast<AUserInput>(GetOuter()->GetWorld()->GetFirstPlayerController());
}
void UMinigameManager::StartMiniGame(EMinigameType minigameType, const FMinigameData& minigameData)
{
   controllerRef->GetHUDManager()->AddHUD(static_cast<uint8>(HUDs::HS_Ingame));
   // Spawn the game pawn.  The game pawn will create a hud for itself
   switch (minigameType) {
      case EMinigameType::DrawingMiniGame: StartDrawingGame(minigameData); break;
      default: break;
   }
}

void UMinigameManager::StartDrawingGame(const FMinigameData& minigameData)
{
   minigamePawn        = controllerRef->GetWorld()->SpawnActorDeferred<AMinigamePawn>(drawingPawn.Get(), FTransform::Identity, controllerRef);
   minigamePawn->mData = minigameData;
   UGameplayStatics::FinishSpawningActor(minigamePawn, FTransform::Identity);
   controllerRef->Possess(minigamePawn);
}

void UMinigameManager::EndMiniGame()
{
   if (minigamePawn) {
      // Close remaining hud and repossess camerapawn
      controllerRef->Possess(controllerRef->GetCameraPawn());
      bool bSuccess = minigamePawn->EndMinigame();

      if (bSuccess) {
         for (FTriggerData& trigger : minigamePawn->mData.endTriggers) {
            controllerRef->GetGameMode()->GetTriggerManager()->ActivateTrigger(trigger);
         }
      } else {
         for (FTriggerData& trigger : minigamePawn->mData.failTriggers) {
            controllerRef->GetGameMode()->GetTriggerManager()->ActivateTrigger(trigger);
         }
      }

      minigamePawn->Destroy();
      minigamePawn = nullptr;
      controllerRef->GetHUDManager()->AddHUD(static_cast<uint8>(HUDs::HS_Ingame));
   }
}

void UMinigameManager::AbortMinigame()
{
   if (minigamePawn) {

      minigamePawn->EndMinigame();
      minigamePawn->Destroy();
      minigamePawn = nullptr;

      controllerRef->GetHUDManager()->AddHUD(static_cast<uint8>(HUDs::HS_Ingame));
      controllerRef->Possess(controllerRef->GetCameraPawn());
   }
}