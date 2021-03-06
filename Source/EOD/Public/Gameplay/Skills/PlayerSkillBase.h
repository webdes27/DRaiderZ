// Copyright 2018 Moikkai Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Skills/GameplaySkillBase.h"
#include "PlayerSkillBase.generated.h"

class UContainerWidgetBase;

/**
 * The base class of skills that can be used by player, added to skill bar, upgraded by player, etc.
 */
UCLASS()
class EOD_API UPlayerSkillBase : public UGameplaySkillBase
{
	GENERATED_BODY()
	
public:

	UPlayerSkillBase(const FObjectInitializer& ObjectInitializer);

	// --------------------------------------
	//  Gameplay Skill Interface
	// --------------------------------------

	virtual void UnlockSkill(int32 UnlockLevel);
	virtual void LockSkill();

	virtual bool ShouldUIActivate();

	virtual bool CanPlayerActivateThisSkill() const;

	virtual bool CanTriggerSkill() const override;

	/** Event called when player changes it's equipped weapon */
	virtual void OnWeaponChange(EWeaponType NewWeaponType, EWeaponType OldWeaponType);

	inline bool IsActiveAsChainSkill() { return bActiveAsChainSkill; }

	/** Event called when this skill is placed in SupersedingChainSkillGroup */
	virtual void OnActivatedAsChainSkill();

	/** Event called when this skill is removed from SupersedingChainSkillGroup */
	virtual void OnDeactivatedAsChainSkill();

	// --------------------------------------
	//  Pseudo Constants
	// --------------------------------------

	FORCEINLINE int32 GetMaxUpgradeLevel() const { return MaxUpgrades; }

	FORCEINLINE FName GetSupersedingSkillGroup() const { return SupersedingSkillGroup; }

	FORCEINLINE TArray<FName> GetPrecedingSkillGroups() const { return PrecedingSkillGroups; }

protected:

	/** Maximum upgrades available for this skill */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Information")
	int32 MaxUpgrades;

	/** Skill that can be used after using this skill (skill chaining) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Information")
	FName SupersedingSkillGroup;

	/** Skills, any of which MUST be used before using this skill */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Information")
	TArray<FName> PrecedingSkillGroups;

	/** Weapons that this skill can be used with */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat Information", meta = (Bitmask, BitmaskEnum = "EWeaponType"))
	uint8 SupportedWeapons;

public:

	// --------------------------------------
	//  Skill State
	// --------------------------------------

	/** Returns true if this skill is currently unlocked on skill tree */
	inline bool IsUnlocked() const { return CurrentUpgrade > 0; }

	/** Returns true if this skill is currently in cooldown */
	inline bool IsSkillInCooldown() const { return CooldownRemaining > 0.f; }

	inline float GetRemainingCooldown() const { return CooldownRemaining; }

	FORCEINLINE int32 GetCurrentUpgrade() const { return CurrentUpgrade; }

	inline void SetCurrentUpgrade(int32 Value) { CurrentUpgrade = Value; }

protected:

	bool bActiveAsChainSkill;

	int32 CurrentUpgrade;

	UPROPERTY()
	FTimerHandle CooldownTimerHandle;

	float CooldownRemaining;

	UFUNCTION()
	virtual void StartCooldown();

	UFUNCTION()
	virtual void FinishCooldown();

	UFUNCTION()
	virtual void CancelCooldown();

	UFUNCTION()
	virtual void UpdateCooldown();

	// --------------------------------------
	//  Utility
	// --------------------------------------

public:

	/** Returns the skill duration  */
	virtual float GetSkillDuration() const;

	/** Returns true if this skill can be used with the given weapon type */
	inline bool IsWeaponTypeSupported(EWeaponType WeaponType) const;

	void LinkToWidget(UContainerWidgetBase* ContainerWidget);

	void UnlinkFromWidget(UContainerWidgetBase* ContainerWidget);

	void RefreshWidgets();

	TArray<UContainerWidgetBase*> GetRegisteredWidgets() const { return RegisteredWidgets; }

protected:

	float SkillDuration;

	UPROPERTY(Transient)
	TArray<UContainerWidgetBase*> RegisteredWidgets;

};

inline bool UPlayerSkillBase::IsWeaponTypeSupported(EWeaponType WeaponType) const
{
	return (SupportedWeapons & (1 << (uint8)WeaponType));
}
