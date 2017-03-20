// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"

#include "../Game/WinningLocation.h"
#include "../DissertationLevelGameMode.h"
#include "../AI/EnemyAIController.h"
#include "../AI/PathNode.h"

#include "BTTask_MoveToWinningLocation.h"



EBTNodeResult::Type UBTTask_MoveToWinningLocation::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory){
    //Get AI controller
    AEnemyAIController *EnemyPC = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

    //Get winning location
    AWinningLocation *WinLoc = ((ADissertationLevelGameMode *)GetWorld()->GetAuthGameMode())->GetWinLoc();
    
    if(EnemyPC && WinLoc){
		//Get the Node Map
		TArray<PathNode*> MapNodes = ((ADissertationLevelGameMode *)GetWorld()->GetAuthGameMode())->GetMapNodes();

		//If Node Map populated, go to the goal
		if(MapNodes.Num() > 0){
			EnemyPC->GoToWinningLocation(WinLoc, MapNodes);
		}
        
        return EBTNodeResult::Succeeded;
    }
    
    return EBTNodeResult::Failed;
}
