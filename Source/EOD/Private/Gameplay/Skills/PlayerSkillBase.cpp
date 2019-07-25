// Copyright 2018 Moikkai Games. All Rights Reserved.


#include "PlayerSkillBase.h"
#include "EODCharacterBase.h"
#include "ContainerWidget.h"
#include "PlayerSkillsComponent.h"
#include "ContainerWidgetBase.h"

#include "Components/Image.h"

UPlayerSkillBase::UPlayerSkillBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CurrentUpgrade = 0;
	MaxUpgrades = 1;
}

void UPlayerSkillBase::UnlockSkill(int32 UnlockLevel)
{
	check(UnlockLevel > 0 && UnlockLevel <= MaxUpgrades);
	SetCurrentUpgrade(UnlockLevel);


}

void UPlayerSkillBase::LockSkill()
{
}

bool UPlayerSkillBase::CanPlayerActivateThisSkill() const
{
	AEODCharacterBase* Instigator = SkillInstigator.Get();
	EWeaponType EquippedWeaponType = Instigator ? Instigator->GetEquippedWeaponType() : EWeaponType::None;

	// bool bUnlocked = IsUnlocked();
	bool bHasValidWeapon = EquippedWeaponType != EWeaponType::None && IsWeaponTypeSupported(EquippedWeaponType) && !Instigator->IsWeaponSheathed();
	bool bInCooldown = IsSkillInCooldown();
	bool bHasNoPrecedingSkillGroups = PrecedingSkillGroups.Num() == 0;

	bool bOwnerHasTags = true;
	if (!ActivationRequiredTags.IsEmpty() && Instigator)
	{
		bOwnerHasTags = Instigator->GameplayTagContainer.HasAll(ActivationRequiredTags);
	}

	// return bHasValidWeapon && !bInCooldown && bHasNoPrecedingSkillGroups && bUnlocked;
	return bHasValidWeapon && !bInCooldown && bHasNoPrecedingSkillGroups && bOwnerHasTags;
}

bool UPlayerSkillBase::CanTriggerSkill() const
{
	AEODCharacterBase* Instigator = SkillInstigator.Get();
	EWeaponType EquippedWeaponType = Instigator ? Instigator->GetEquippedWeaponType() : EWeaponType::None;

	bool bHasValidWeapon = EquippedWeaponType != EWeaponType::None && IsWeaponTypeSupported(EquippedWeaponType) && !Instigator->IsWeaponSheathed();
	bool bInCooldown = IsSkillInCooldown();

	bool bInstigatorCanUseSkill = Instigator ? Instigator->IsIdleOrMoving() || Instigator->IsBlocking() || Instigator->IsNormalAttacking() : false;
	if (!bInstigatorCanUseSkill && Instigator->IsUsingAnySkill())
	{
		UGameplaySkillsComponent* SkillsComp = InstigatorSkillComponent.Get();
		if (SkillsComp && SkillsComp->CanUseChainSkill() && SkillsComp->GetSupersedingChainSkillGroup().Value == this->SkillIndex)
		{
			bInstigatorCanUseSkill = true;
		}
	}

	bool bOwnerHasTags = true;
	if (!ActivationRequiredTags.IsEmpty() && Instigator)
	{
		bOwnerHasTags = Instigator->GameplayTagContainer.HasAll(ActivationRequiredTags);
	}

	return bHasValidWeapon && !bInCooldown && bInstigatorCanUseSkill && bOwnerHasTags;
}

void UPlayerSkillBase::OnWeaponChange(EWeaponType NewWeaponType, EWeaponType OldWeaponType)
{
}

void UPlayerSkillBase::OnActivatedAsChainSkill()
{
	for (UContainerWidgetBase* Widget : RegisteredWidgets)
	{
		check(Widget);
		Widget->EnableContainer();
	}
}

void UPlayerSkillBase::OnDeactivatedAsChainSkill()
{
	for (UContainerWidgetBase* Widget : RegisteredWidgets)
	{
		check(Widget);
		Widget->DisableContainer();
	}
}

void UPlayerSkillBase::StartCooldown()
{
}

void UPlayerSkillBase::FinishCooldown()
{
}

void UPlayerSkillBase::CancelCooldown()
{
}

void UPlayerSkillBase::UpdateCooldown()
{
}

float UPlayerSkillBase::GetSkillDuration() const
{
	return SkillDuration;
}

void UPlayerSkillBase::LinkToWidget(UContainerWidgetBase* ContainerWidget)
{
	RegisteredWidgets.AddUnique(ContainerWidget);
}

void UPlayerSkillBase::UnlinkFromWidget(UContainerWidgetBase* ContainerWidget)
{
	RegisteredWidgets.Remove(ContainerWidget);
}

void UPlayerSkillBase::RefreshWidgets()
{
	for (UContainerWidgetBase* Widget : RegisteredWidgets)
	{
		check(Widget);
		Widget->RefreshContainer();
	}
}
