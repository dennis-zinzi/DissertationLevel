// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"

#include "../DissertationLevelGameMode.h"
#include "../AI/EnemyCharacter.h"
#include "../AI/EnemyAIController.h"
#include "../Game/WinningLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "../AI/PathNode.h"

#include "BTTask_SelectPathLocation.h"




EBTNodeResult::Type UBTTask_SelectPathLocation::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory){
    //Get AI Controller
    AEnemyAIController *AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
    
    //Get Winning Location
    AWinningLocation *WinLoc = ((ADissertationLevelGameMode *)GetWorld()->GetAuthGameMode())->GetWinLoc();
    
    if(AIController && WinLoc){
        TArray<FVector> FullPath = AIController->GetPathLocations();

        if(FullPath.Num() == 0){
            //Get Level NodeMap
            TArray<PathNode*> NodeMap = ((ADissertationLevelGameMode *)GetWorld()->GetAuthGameMode())->GetMapNodes();
            
            //Create the path to the winning location
            AIController->GoToWinningLocation(WinLoc, NodeMap);
            
            if(AIController->GetPathLocations().Num() == 0){
                return EBTNodeResult::Succeeded;
            }
            
            //Update Full Path value
            FullPath = AIController->GetPathLocations();
            UE_LOG(LogClass, Log, TEXT("Path Size: %s"), *FString::FromInt(FullPath.Num()));
        }
        else if(FullPath.Num() > 0 && FullPath.Num() <= 2){
            //Set Game to Won
            ((ADissertationLevelGameMode *)GetWorld()->GetAuthGameMode())->SetCurrentState(EPlayState::EWin);
            return EBTNodeResult::Succeeded;
        }

        //Get Blackboard Component
        UBlackboardComponent *BlackboardComp = OwnerComp.GetBlackboardComponent();

        //Vector holding next position to travel to
        FVector NextPathLoc;
        
        //Current index
        int CurrentIndex = AIController->GetCurrentPathLocIndex();
        
        if(CurrentIndex != FullPath.Num() - 1){
            AIController->SetCurrentPathLocIndex(CurrentIndex + 1);
            
            /* Uncomment next line if single AI */
//            NextPathLoc = FullPath[CurrentIndex + 1];
            
            /* Uncomment next lines if flock of AIs */
            AEnemyCharacter *AIChar = Cast<AEnemyCharacter>(AIController->GetPawn());
            if(AIChar){
                NextPathLoc = ((ADissertationLevelGameMode *)GetWorld()->GetAuthGameMode())->GetAIFlock()->UpdateAI(AIChar, FullPath[CurrentIndex + 1]);
            }
        }
        else{
            /* Comment if using single AI */
//            UE_LOG(LogClass, Log, TEXT("Win POS: %s"), *WinLoc->GetActorLocation().ToString());
//            UE_LOG(LogClass, Log, TEXT("AI POS: %s\n"), *FullPath[CurrentIndex].ToString());
//            UE_LOG(LogClass, Log, TEXT("AI REAL POS: %s\n"), *AIController->GetPawn()->GetActorLocation().ToString());
//            UE_LOG(LogClass, Log, TEXT("AI Center POS: %s\n"), *(((ADissertationLevelGameMode *)GetWorld()->GetAuthGameMode())->GetAIFlock()->GetAICenter().ToString()));
            
            if(FMath::Abs(FVector::Dist(AIController->GetPawn()->GetActorLocation(), WinLoc->GetActorLocation())) < 250.0f){
                //Reached final location, so stop moving
                BlackboardComp->SetValueAsBool(FName("Reached"), true);
            }
            else{
//                //Get Level NodeMap
//                TArray<PathNode*> NodeMap = ((ADissertationLevelGameMode *)GetWorld()->GetAuthGameMode())->GetMapNodes();
//    
//                //Create the path to the winning location
//                AIController->GoToWinningLocation(WinLoc, NodeMap);
                
                //Recalculate
                ((ADissertationLevelGameMode *)GetWorld()->GetAuthGameMode())->GetAIFlock()->CalculateBoidsPaths();
            }
            
            
            return EBTNodeResult::Succeeded;
        }
        
        //Set new location to move to
        BlackboardComp->SetValueAsVector(FName("NextPathLocation"), NextPathLoc);
        
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}
