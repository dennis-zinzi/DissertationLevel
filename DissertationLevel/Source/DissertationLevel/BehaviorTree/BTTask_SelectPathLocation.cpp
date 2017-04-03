// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"

#include "../DissertationLevelGameMode.h"
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
            
            //Update Full Path value
            FullPath = AIController->GetPathLocations();
        }

        //Get Blackboard Component
        UBlackboardComponent *BlackboardComp = OwnerComp.GetBlackboardComponent();

        //Vector holding next position to travel to
        FVector NextPathLoc;
        
        //Current index
        int CurrentIndex = AIController->GetCurrentPathLocIndex();
        
        if(CurrentIndex != FullPath.Num() - 1){
            AIController->SetCurrentPathLocIndex(CurrentIndex + 1);
            NextPathLoc = FullPath[CurrentIndex + 1];
        }
        else{
            return EBTNodeResult::Succeeded;
        }
        
        //Set new location to move to
        BlackboardComp->SetValueAsVector(FName("NextPathLocation"), NextPathLoc);
        
        return EBTNodeResult::Succeeded;
    }
    
    return EBTNodeResult::Failed;
}
