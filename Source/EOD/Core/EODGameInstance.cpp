// Copyright 2018 Moikkai Games. All Rights Reserved.

#include "EODGameInstance.h"
#include "EOD/SaveSystem/MetaSaveGame.h"
#include "EOD/Statics/EODGlobalNames.h"

#include "Kismet/GameplayStatics.h"

const int32 UEODGameInstance::PlayerIndex(0);
const FString UEODGameInstance::MetaSaveSlotName(TEXT("DefaultSlot"));

UEODGameInstance::UEODGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UEODGameInstance::Init()
{
	Super::Init();

	// Load the meta save game object
	if (UGameplayStatics::DoesSaveGameExist(UEODGameInstance::MetaSaveSlotName, UEODGameInstance::PlayerIndex))
	{
		MetaSaveGame = Cast<UMetaSaveGame>(UGameplayStatics::LoadGameFromSlot(UEODGameInstance::MetaSaveSlotName, UEODGameInstance::PlayerIndex));
	}
	// If the meta save game object doesn't exist yet, create and save one (it will happen during first run)
	else
	{
		MetaSaveGame = Cast<UMetaSaveGame>(UGameplayStatics::CreateSaveGameObject(UMetaSaveGame::StaticClass()));
		if (IsValid(MetaSaveGame))
		{
			UGameplayStatics::SaveGameToSlot(MetaSaveGame, UEODGameInstance::MetaSaveSlotName, UEODGameInstance::PlayerIndex);
		}
	}

	if (IsValid(MetaSaveGame))
	{
		if (MetaSaveGame->LastUsedSlotName != FString("") && UGameplayStatics::DoesSaveGameExist(MetaSaveGame->LastUsedSlotName, UEODGameInstance::PlayerIndex))
		{
			CurrentProfileSaveGame = Cast<UPlayerSaveGame>(UGameplayStatics::LoadGameFromSlot(MetaSaveGame->LastUsedSlotName, UEODGameInstance::PlayerIndex));
		}
	}



}
