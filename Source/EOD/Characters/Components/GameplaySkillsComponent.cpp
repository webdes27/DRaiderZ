// Copyright 2018 Moikkai Games. All Rights Reserved.

#include "EOD/Characters/Components/GameplaySkillsComponent.h"
#include "EOD/Characters/EODCharacterBase.h"
#include "EOD/Characters/PlayerCharacter.h"
#include "EOD/SaveSystem/PlayerSaveGame.h"
#include "EOD/Core/EODGameInstance.h"

#include "Kismet/GameplayStatics.h"


UGameplaySkillsComponent::UGameplaySkillsComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);

	ChainSkillResetDelay = 2.f;
	MaxNumSkills = 20;

	for (int i = 1; i <= MaxNumSkills; i++)
	{
		SBIndexToSGMap.Add(i, FString(""));
	}
}


void UGameplaySkillsComponent::PostLoad()
{
	Super::PostLoad();
	LoadSkillBarLayout();
}

void UGameplaySkillsComponent::BeginPlay()
{
	Super::BeginPlay();

	// Casting GetOwner() in BeginPlay because:
	// * GetOwner() in constructor is CDO of created character (e.g., Default_BP_FemaleCharacter, etc.)
	// * GetOwner() in PostLoad() was correct for AI characters spawned along with map, but was incorrect (NULL)  for player character
	// * GetOwner() has been found to be setup correctly in BeginPlay
	EODCharacterOwner = Cast<AEODCharacterBase>(GetOwner());
}

void UGameplaySkillsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UGameplaySkillsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UGameplaySkillsComponent::SetCurrentActiveSkill(const FName SkillID)
{
	// Although setting ActiveSkillID and ActiveSkill on local client is un-necessary from combat perspective (all combat events fire on server only),
	// these variables have been set up in local client as well for query purposes.

	if (SkillID == NAME_None)
	{
		ActiveSkillID = NAME_None;
		ActiveSkill = nullptr;
	}
	else
	{
		ActiveSkill = GetSkill(SkillID);
		ActiveSkillID = ActiveSkill ? SkillID : NAME_None;
	}

	if (IsValid(EODCharacterOwner) && EODCharacterOwner->Role < ROLE_Authority)
	{
		Server_SetCurrentActiveSkill(SkillID);
	}
}

void UGameplaySkillsComponent::OnPressingSkillKey(const int32 SkillKeyIndex)
{
	if (SkillKeyIndex <= 0 || SkillKeyIndex > SBIndexToSGMap.Num())
	{
		// Invalid skill key index
		return;
	}

	if (!IsValid(EODCharacterOwner) || !EODCharacterOwner->CanUseAnySkill())
	{
		return;
	}

	FString SGToUse = SBIndexToSGMap[SkillKeyIndex];
	if (SGToUse == FString("") || SGToCooldownMap.Contains(SGToUse))
	{
		// Either no skill equipped in given slot or it's in cooldown
		return;
	}
	
	APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(EODCharacterOwner);
	FString GenderPrefix = PlayerChar ? PlayerChar->GetGenderPrefix() : FString("");

	FString SkillIDString = GenderPrefix + SGToUse + FString("_") + FString::FromInt(1);
	FSkillTableRow* Skill = GetSkill(FName(*SkillIDString));
	if (Skill && Skill->AnimMontage.Get())
	{
		PlayerChar->PlayAnimationMontage(Skill->AnimMontage.Get(), Skill->SkillStartMontageSectionName);
	}

}

void UGameplaySkillsComponent::OnReleasingSkillKey(const int32 SkillKeyIndex)
{
}

void UGameplaySkillsComponent::LoadSkillBarLayout()
{
	UEODGameInstance* GameInstance = EODCharacterOwner ? Cast<UEODGameInstance>(EODCharacterOwner->GetGameInstance()) : nullptr;
	if (!IsValid(GameInstance) || !IsValid(GameInstance->GetCurrentPlayerSaveGameObject()))
	{
		return;
	}
	
	UPlayerSaveGame* PlayerSaveGame = GameInstance->GetCurrentPlayerSaveGameObject();
	TMap<int32, FString>& SBLayout = PlayerSaveGame->SkillBarLayout;
	TArray<int32> LayoutKeys;
	SBLayout.GetKeys(LayoutKeys);
	for (int32 Key : LayoutKeys)
	{
		if (SBLayout[Key] == FString(""))
		{
			continue;
		}

		if (SBIndexToSGMap.Contains(Key))
		{
			SBIndexToSGMap[Key] = SBLayout[Key];
		}
	}
}

void UGameplaySkillsComponent::SaveSkillBarLayout()
{
	UEODGameInstance* GameInstance = EODCharacterOwner ? Cast<UEODGameInstance>(EODCharacterOwner->GetGameInstance()) : nullptr;
	UPlayerSaveGame* PlayerSaveGame = GameInstance ? GameInstance->GetCurrentPlayerSaveGameObject() : nullptr;
	if (IsValid(PlayerSaveGame))
	{
		PlayerSaveGame->SkillBarLayout = SBIndexToSGMap;
		UGameplayStatics::SaveGameToSlot(PlayerSaveGame, GameInstance->GetCurrentPlayerSaveGameName(), GameInstance->PlayerIndex);
	}
}

void UGameplaySkillsComponent::Server_SetCurrentActiveSkill_Implementation(const FName SkillID)
{
	SetCurrentActiveSkill(SkillID);
}

bool UGameplaySkillsComponent::Server_SetCurrentActiveSkill_Validate(const FName SkillID)
{
	return true;
}