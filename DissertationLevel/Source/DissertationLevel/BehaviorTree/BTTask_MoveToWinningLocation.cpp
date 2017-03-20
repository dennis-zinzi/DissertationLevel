// Fill out your copyright notice in the Description page of Project Settings.

#include "../DissertationLevel.h"

#include "../Game/WinningLocation.h"
#include "../DissertationLevelGameMode.h"
#include "../AI/EnemyAIController.h"

#include "BTTask_MoveToWinningLocation.h"



EBTNodeResult::Type UBTTask_MoveToWinningLocation::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory){
    //Get AI controller
    AEnemyAIController *EnemyPC = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

    //Get winning location
    AWinningLocation *WinLoc = ((ADissertationLevelGameMode *)GetWorld()->GetAuthGameMode())->GetWinLoc();
    
    if(EnemyPC && WinLoc){
        EnemyPC->GoToWinningLocation(WinLoc);
        
        return EBTNodeResult::Succeeded;
    }
    
    return EBTNodeResult::Failed;
}
