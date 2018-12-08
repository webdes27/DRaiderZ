// Copyright 2018 Moikkai Games. All Rights Reserved.

#include "EODLevelScriptActor.h"
#include "EOD/Player/PlayerCharacter.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

AEODLevelScriptActor::AEODLevelScriptActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void AEODLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC && PC->GetPawn())
	{
		APlayerCharacter* PlayerPawn = Cast<APlayerCharacter>(PC->GetPawn());
		if (PlayerPawn)
		{
			PlayerPawn->OnInitiatingCombat.AddDynamic(this, &AEODLevelScriptActor::OnCombatStarted);
			PlayerPawn->OnLeavingCombat.AddDynamic(this, &AEODLevelScriptActor::OnCombatEnded);
		}
	}
}

void AEODLevelScriptActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEODLevelScriptActor::OnCombatStarted_Implementation(APlayerCharacter* PlayerCharacter)
{
}

void AEODLevelScriptActor::OnCombatEnded_Implementation(APlayerCharacter* PlayerCharacter)
{
}
