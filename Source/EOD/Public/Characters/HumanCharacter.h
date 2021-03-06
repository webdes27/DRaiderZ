// Copyright 2018 Moikkai Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PrimaryWeapon.h"
#include "SecondaryWeapon.h"
#include "CharacterLibrary.h"
#include "EODCharacterBase.h"
#include "ArmorLibrary.h"
#include "EODBlueprintFunctionLibrary.h"

#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "HumanCharacter.generated.h"


class UAudioComponent;
class USkeletalMeshComponent;

USTRUCT(BlueprintType)
struct EOD_API FWeaponSlot
{
	GENERATED_USTRUCT_BODY()
		
	UPROPERTY(EditAnywhere)
	FName PrimaryWeaponID;
	
	UPROPERTY(EditAnywhere)
	FName SecondaryWeaponID;
	
	FWeaponSlot() :
		PrimaryWeaponID(NAME_None),
		SecondaryWeaponID(NAME_None)
	{
	}
};

USTRUCT(BlueprintType)
struct EOD_API FArmorSlot
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FName ChestArmorID;
	
	UPROPERTY(EditAnywhere)
	FName HandsArmorID;
	
	UPROPERTY(EditAnywhere)
	FName LegsArmorID;
	
	UPROPERTY(EditAnywhere)
	FName FeetArmorID;

	FArmorSlot() :
		ChestArmorID(NAME_None),
		HandsArmorID(NAME_None),
		LegsArmorID(NAME_None),
		FeetArmorID(NAME_None)
	{
	}
};

USTRUCT(BlueprintType)
struct EOD_API FEquippedWeapons
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FWeaponSlot PrimaryWeaponSlot;
	
	UPROPERTY(EditAnywhere)
	FWeaponSlot SecondaryWeaponSlot;
	
	UPROPERTY(EditAnywhere)
	uint8 CurrentSlotIndex;
	
	const FWeaponSlot& GetCurrentWeaponSlot() const
	{
		return CurrentSlotIndex == 1 ? SecondaryWeaponSlot : PrimaryWeaponSlot;
	}

	void SetPrimaryWeaponID(FName WeaponID)
	{
		if (CurrentSlotIndex == 1)
		{
			SecondaryWeaponSlot.PrimaryWeaponID = WeaponID;
		}
		else
		{
			PrimaryWeaponSlot.PrimaryWeaponID = WeaponID;
		}
	}

	void SetSecondaryWeaponID(FName WeaponID)
	{
		if (CurrentSlotIndex == 1)
		{
			SecondaryWeaponSlot.SecondaryWeaponID = WeaponID;
		}
		else
		{
			PrimaryWeaponSlot.SecondaryWeaponID = WeaponID;
		}
	}

	FEquippedWeapons() :
		CurrentSlotIndex(0)
	{
	}
};

/**
 * 
 */
UCLASS()
class EOD_API AHumanCharacter : public AEODCharacterBase
{
	GENERATED_BODY()

public:

	// --------------------------------------
	//  UE4 Method Overrides
	// --------------------------------------

	/** Create and initialize skeletal armor mesh, camera, and inventory components. */
	AHumanCharacter(const FObjectInitializer& ObjectInitializer);

	/** Property replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Spawn default weapon(s) */
	virtual void PostInitializeComponents() override;

	/** Initializes player animation references. Creates player HUD widget and adds it to the viewport. */
	virtual void BeginPlay() override;

	/** Update character state every frame */
	virtual void Tick(float DeltaTime) override;

	/** Called once this actor has been deleted */
	virtual void Destroyed() override;

	// --------------------------------------
	//  Save/Load System
	// --------------------------------------

	/** Saves current player state */
	virtual void SaveCharacterState() override;

	// --------------------------------------
	//  Components
	// --------------------------------------

	static FName HairComponentName;

	static FName HatItemComponentName;

	static FName FaceItemComponentName;

	static FName ChestComponentName;

	static FName HandsComponentName;

	static FName LegsComponentName;

	static FName FeetComponentName;

	FORCEINLINE USkeletalMeshComponent* GetHair() const { return Hair; }

	FORCEINLINE USkeletalMeshComponent* GetHatItem() const { return HatItem; }

	FORCEINLINE USkeletalMeshComponent* GetFaceItem() const { return FaceItem; }

	FORCEINLINE USkeletalMeshComponent* GetChest() const { return Chest; }
	
	FORCEINLINE USkeletalMeshComponent* GetHands() const { return Hands; }

	FORCEINLINE USkeletalMeshComponent* GetLegs() const { return Legs; }
	
	FORCEINLINE USkeletalMeshComponent* GetFeet() const { return Feet; }

private:

	//~ @note The default skeletal mesh component inherited from ACharacter class will reference the skeletal mesh for player face

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Hair;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* HatItem;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FaceItem;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Chest;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Hands;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Legs;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Feet;

	/** [Constructor Only] A helper function that creates and returns new armor skeletal mesh component */
	USkeletalMeshComponent* CreateNewArmorComponent(const FName Name, const FObjectInitializer& ObjectInitializer);

	inline void SetMasterPoseComponentForMeshes();

public:

	// --------------------------------------
	//  Animations
	// --------------------------------------

	inline FPlayerAnimationReferencesTableRow* GetActiveAnimationReferences() const;

	inline FPlayerAnimationReferencesTableRow* GetEquippedWeaponAnimationReferences() const;

protected:

	inline void AddAnimationSoftObjectPathToArray(const TSoftObjectPtr<UAnimMontage>& MontageSoftPtr, TArray<FSoftObjectPath>& PathArray);

	/** Load animation references for the given weapon type */
	virtual void LoadAnimationReferencesForWeapon(EWeaponType WeaponType);

	/** Unload animation references for the given weapon type */
	virtual void UnloadAnimationReferencesForWeapon(EWeaponType WeaponType);

	TSharedPtr<FStreamableHandle> LoadAnimationReferences(FPlayerAnimationReferencesTableRow* AnimationReferences);

	/** Animation references by weapon type */
	TMap<EWeaponType, FPlayerAnimationReferencesTableRow*> AnimationReferencesMap;

	/** Streamable handle for animation references by weapon type */
	TMap<EWeaponType, TSharedPtr<FStreamableHandle>> AnimationReferencesStreamableHandles;

	FName GetAnimationReferencesRowID(EWeaponType WeaponType, ECharacterGender CharGender);

	/** Data table containing player animation references */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Data Table")
	UDataTable* PlayerAnimationReferencesDataTable;

public:

	// --------------------------------------
	//  Weapon System
	// --------------------------------------

	/** Returns true if primary weapon is equipped */
	inline bool IsPrimaryWeaponEquipped() const;

	/** Returns true if secondary weapon is equipped */
	inline bool IsSecondaryWeaponEquipped() const;

	/** Returns primary weapon actor */
	FORCEINLINE APrimaryWeapon* GetPrimaryWeapon() const;

	/** Returns secondary weapon actor */
	FORCEINLINE ASecondaryWeapon* GetSecondaryWeapon() const;

	FORCEINLINE bool CanToggleSheathe() const;

	FORCEINLINE bool IsSwitchingWeapon() const;

	/** Returns the weapon type of primary weapon */
	virtual EWeaponType GetEquippedWeaponType() const override;

	/** Replace primary weapon with a new weapon */
	virtual void SetCurrentPrimaryWeapon(const FName WeaponID);

	/** Replace secondary weapon with a new weapon */
	virtual void SetCurrentSecondaryWeapon(const FName WeaponID);

	/** Removes primary weapon if it is currently equipped */
	virtual void RemovePrimaryWeapon();

	/** Removes secondary weapon if it is currently equipped */
	virtual void RemoveSecondaryWeapon();

	//~ @note We're not using helm armor
	virtual void AddArmor(FName ArmorID);

	virtual void RemoveArmor(EArmorType ArmorType);

	virtual void ToggleWeapon();

protected:

	/** An actor for primary weapon equipped by the player */
	UPROPERTY(Transient)
	APrimaryWeapon* PrimaryWeapon;

	/** An actor for secondary weapon equipped by the player */
	UPROPERTY(Transient)
	ASecondaryWeapon* SecondaryWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_ArmorSlot, EditAnywhere, BlueprintReadOnly)
	FArmorSlot ArmorSlot;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapons, EditAnywhere, BlueprintReadOnly)
	FEquippedWeapons EquippedWeapons;

public:

	// --------------------------------------
	//  Character States
	// --------------------------------------

	/** Returns true if character can dodge */
	virtual bool CanDodge() const;

	/** Start dodging */
	virtual void StartDodge() override;

	/** Cancel dodging */
	virtual void CancelDodge() override;

	/** Finish dodging */
	virtual void FinishDodge() override;

	/** Returns true if character can use normal attack */
	virtual bool CanNormalAttack() const override;

	/** Start normal attacks */
	virtual void StartNormalAttack() override;

	/** Cancel normal attacks */
	virtual void CancelNormalAttack() override;

	/** Finish normal attacks and reset back to Idle-Walk-Run */
	virtual void FinishNormalAttack() override;

	/** Updates character normal attck state every frame if the character wants to normal attack */
	virtual void UpdateNormalAttackState(float DeltaTime) override;

	/** Get the name of next normal attack section that comes after CurrentSection */
	FName GetNextNormalAttackSectionName(const FName& CurrentSection) const;

	/** Switch between normal attack sections */
	void ChangeNormalAttackSection(FName OldSection, FName NewSection);

	/** Event called when a new normal attack section starts playing */
	void OnNormalAttackSectionStart(FName SectionName);

	/** Set whether normal attack section can change or not  */
	UFUNCTION(BlueprintCallable, Category = "Character State")
	void SetNormalAttackSectionChangeAllowed(bool bNewValue);

protected:

	UPROPERTY(Transient)
	uint32 bNormalAttackSectionChangeAllowed : 1;

public:

	// --------------------------------------
	//  Crowd Control Effect
	// --------------------------------------

	/** Flinch this character (visual feedback) */
	virtual bool CCEFlinch(const float BCAngle) override;

	/** Interrupt this character's current action */
	virtual bool CCEInterrupt(const float BCAngle) override;

	/** Applies stun to this character */
	virtual bool CCEStun(const float Duration) override;

	/** Removes 'stun' crowd control effect from this character */
	virtual void CCERemoveStun() override;

	/** Freeze this character */
	virtual bool CCEFreeze(const float Duration) override;

	/** Removes 'freeze' crowd control effect from this character */
	virtual void CCEUnfreeze() override;

	/** Knockdown this character */
	virtual bool CCEKnockdown(const float Duration) override;

	/** Removes 'knock-down' crowd control effect from this character */
	virtual void CCEEndKnockdown() override;

	/** Knockback this character */
	virtual bool CCEKnockback(const float Duration, const FVector& ImpulseDirection) override;

	// --------------------------------------
	//  Utility
	// --------------------------------------

	FORCEINLINE ECharacterGender GetCharacterGender() const;

	inline FName GetNormalAttackSectionName(uint8 AttackIndex);

	inline uint8 GetNormalAttackIndex(FName SectionName);

	/** Get the suffix string from the normal attack section */
	inline FString GetNormalAttackSuffix(FName NormalAttackSection) const;

	// --------------------------------------
	//  Combat Interface
	// --------------------------------------

	/** Returns the sound that should be played when this character hits a physical surface */
	virtual USoundBase* GetMeleeHitSound(const TEnumAsByte<EPhysicalSurface> HitSurface, const bool bCritHit) const;

	/** Returns the sound that should be played when this character fails to hit anything */
	virtual USoundBase* GetMeleeHitMissSound() const override;

	// --------------------------------------
	//  Input Handling
	// --------------------------------------

	/** Event called when forward key is pressed */
	virtual void OnPressedForward() override;

	/** Event called when backward key is pressed */
	virtual void OnPressedBackward() override;

	/** Event called when forward key is released */
	virtual void OnReleasedForward() override;

	/** Event called when backward key is released */
	virtual void OnReleasedBackward() override;

	virtual void OnPressedLeft() override;

	virtual void OnPressedRight() override;

	virtual void OnReleasedLeft() override;

	virtual void OnReleasedRight() override;


	UPROPERTY(Transient)
	uint32 bForwardPressed : 1;

	UPROPERTY(Transient)
	uint32 bBackwardPressed : 1;

	/** Timer handle needed for executing SP normal attacks */
	FTimerHandle SPAttackTimerHandle;

	UFUNCTION()
	void DisableForwardPressed();

	UFUNCTION()
	void DisableBackwardPressed();

	// --------------------------------------
	//  Materials
	// --------------------------------------

	virtual void TurnOnTargetSwitch() override;

	virtual void TurnOffTargetSwitch() override;

	// --------------------------------------
	//  Sounds
	// --------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	FWeaponHitSounds WeaponHitSounds;

	inline USoundBase* GetGreatswordHitSound(const TEnumAsByte<EPhysicalSurface> HitSurface, const bool bCritHit) const;
	inline USoundBase* GetWarhammerHitSound(const TEnumAsByte<EPhysicalSurface> HitSurface, const bool bCritHit) const;
	inline USoundBase* GetLongswordHitSound(const TEnumAsByte<EPhysicalSurface> HitSurface, const bool bCritHit) const;
	inline USoundBase* GetMaceHitSound(const TEnumAsByte<EPhysicalSurface> HitSurface, const bool bCritHit) const;
	inline USoundBase* GetStaffHitSound(const TEnumAsByte<EPhysicalSurface> HitSurface, const bool bCritHit) const;
	inline USoundBase* GetDaggerHitSound(const TEnumAsByte<EPhysicalSurface> HitSurface, const bool bCritHit) const;

protected:

	// --------------------------------------
	//  Network
	// --------------------------------------

	UFUNCTION()
	virtual void OnRep_PrimaryWeaponID();

	UFUNCTION()
	virtual	void OnRep_SecondaryWeaponID();

	UFUNCTION()
	virtual void OnRep_ArmorSlot();

	UFUNCTION()
	virtual void OnRep_EquippedWeapons();

};

inline void AHumanCharacter::SetMasterPoseComponentForMeshes()
{
	if (GetMesh())
	{
		if (Hair)		{ Hair->SetMasterPoseComponent(GetMesh()); }
		if (HatItem)	{ HatItem->SetMasterPoseComponent(GetMesh()); }
		if (FaceItem)	{ FaceItem->SetMasterPoseComponent(GetMesh()); }
		if (Chest)		{ Chest->SetMasterPoseComponent(GetMesh()); }
		if (Hands)		{ Hands->SetMasterPoseComponent(GetMesh()); }
		if (Legs)		{ Legs->SetMasterPoseComponent(GetMesh()); }
		if (Feet)		{ Feet->SetMasterPoseComponent(GetMesh()); }
	}
}

inline FPlayerAnimationReferencesTableRow* AHumanCharacter::GetActiveAnimationReferences() const
{
	EWeaponType WeaponType = IsWeaponSheathed() ? EWeaponType::None : GetEquippedWeaponType();
	if (AnimationReferencesMap.Contains(WeaponType))
	{
		return AnimationReferencesMap[WeaponType];
	}

	return nullptr;
}

inline FPlayerAnimationReferencesTableRow* AHumanCharacter::GetEquippedWeaponAnimationReferences() const
{
	EWeaponType WeaponType = GetEquippedWeaponType();
	if (AnimationReferencesMap.Contains(WeaponType))
	{
		return AnimationReferencesMap[WeaponType];
	}

	return nullptr;
}

inline void AHumanCharacter::AddAnimationSoftObjectPathToArray(const TSoftObjectPtr<UAnimMontage>& MontageSoftPtr, TArray<FSoftObjectPath>& PathArray)
{
	if (MontageSoftPtr.IsPending())
	{
		PathArray.Add(MontageSoftPtr.ToSoftObjectPath());
	}
}

inline bool AHumanCharacter::IsPrimaryWeaponEquipped() const
{
	const FWeaponSlot& WepSlot = EquippedWeapons.GetCurrentWeaponSlot();
	return WepSlot.PrimaryWeaponID != NAME_None && PrimaryWeapon && PrimaryWeapon->IsAttachedToCharacterOwner();
}

inline bool AHumanCharacter::IsSecondaryWeaponEquipped() const
{
	const FWeaponSlot& WepSlot = EquippedWeapons.GetCurrentWeaponSlot();
	return WepSlot.SecondaryWeaponID != NAME_None && SecondaryWeapon && SecondaryWeapon->IsAttachedToCharacterOwner();
}

FORCEINLINE APrimaryWeapon* AHumanCharacter::GetPrimaryWeapon() const
{
	return PrimaryWeapon;
}

FORCEINLINE ASecondaryWeapon* AHumanCharacter::GetSecondaryWeapon() const
{
	return SecondaryWeapon;
}

FORCEINLINE bool AHumanCharacter::CanToggleSheathe() const
{
	return IsIdleOrMoving() && IsPrimaryWeaponEquipped();
}

FORCEINLINE bool AHumanCharacter::IsSwitchingWeapon() const
{
	return CharacterStateInfo.CharacterState == ECharacterState::SwitchingWeapon;
}

FORCEINLINE ECharacterGender AHumanCharacter::GetCharacterGender() const
{
	return Gender;
}

inline FName AHumanCharacter::GetNormalAttackSectionName(uint8 AttackIndex)
{
	if (AttackIndex == 1)
	{
		return UCharacterLibrary::SectionName_FirstSwing;
	}
	else if (AttackIndex == 2)
	{
		return UCharacterLibrary::SectionName_SecondSwing;
	}
	else if (AttackIndex == 3)
	{
		return UCharacterLibrary::SectionName_ThirdSwing;
	}
	else if (AttackIndex == 4)
	{
		return UCharacterLibrary::SectionName_FourthSwing;
	}
	else if (AttackIndex == 5)
	{
		return UCharacterLibrary::SectionName_FifthSwing;
	}
	else if (AttackIndex == 11)
	{
		return UCharacterLibrary::SectionName_ForwardSPSwing;
	}
	else if (AttackIndex == 12)
	{
		return UCharacterLibrary::SectionName_BackwardSPSwing;
	}
	else
	{
		return NAME_None;
	}
}

inline uint8 AHumanCharacter::GetNormalAttackIndex(FName SectionName)
{
	if (SectionName == NAME_None)
	{
		return 0;
	}
	else if (SectionName == UCharacterLibrary::SectionName_FirstSwing)
	{
		return 1;
	}
	else if (SectionName == UCharacterLibrary::SectionName_SecondSwing)
	{
		return 2;
	}
	else if (SectionName == UCharacterLibrary::SectionName_ThirdSwing)
	{
		return 3;
	}
	else if (SectionName == UCharacterLibrary::SectionName_FourthSwing)
	{
		return 4;
	}
	else if (SectionName == UCharacterLibrary::SectionName_FifthSwing)
	{
		return 5;
	}
	else if (SectionName == UCharacterLibrary::SectionName_ForwardSPSwing)
	{
		return 11;
	}
	else if (SectionName == UCharacterLibrary::SectionName_BackwardSPSwing)
	{
		return 12;
	}
	else
	{
		return 0;
	}
}

inline FString AHumanCharacter::GetNormalAttackSuffix(FName NormalAttackSection) const
{
	if (NormalAttackSection == UCharacterLibrary::SectionName_FirstSwing)
	{
		return FString("1");
	}
	else if (NormalAttackSection == UCharacterLibrary::SectionName_SecondSwing)
	{
		return FString("2");
	}
	else if (NormalAttackSection == UCharacterLibrary::SectionName_ThirdSwing)
	{
		return FString("3");
	}
	else if (NormalAttackSection == UCharacterLibrary::SectionName_FourthSwing)
	{
		return FString("4");
	}
	else if (NormalAttackSection == UCharacterLibrary::SectionName_FifthSwing)
	{
		return FString("5");
	}
	else if (NormalAttackSection == UCharacterLibrary::SectionName_BackwardSPSwing)
	{
		return FString("BSP");
	}
	else if (NormalAttackSection == UCharacterLibrary::SectionName_ForwardSPSwing)
	{
		return FString("FSP");
	}
	else
	{
		return FString("");
	}
}

inline USoundBase* AHumanCharacter::GetGreatswordHitSound(const TEnumAsByte<EPhysicalSurface> HitSurface, const bool bCritHit) const
{
	// Slash2
	USoundBase* Sound = nullptr;

	if (bCritHit)
	{
		Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Slash2CritHitSounds);
	}
	else
	{
		if (HitSurface == SURFACETYPE_FLESH)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Slash2FleshHitSounds);
		}
		else if (HitSurface == SURFACETYPE_METAL)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Slash2MetalHitSounds);
		}
		else if (HitSurface == SURFACETYPE_STONE)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Slash2StoneHitSounds);
		}
		else if (HitSurface == SURFACETYPE_UNDEAD)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Slash2UndeadHitSounds);
		}
		else if (HitSurface == SURFACETYPE_WOOD)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Slash2WoodHitSounds);
		}
		else
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Slash2FleshHitSounds);
		}
	}

	return Sound;
}

inline USoundBase* AHumanCharacter::GetWarhammerHitSound(const TEnumAsByte<EPhysicalSurface> HitSurface, const bool bCritHit) const
{
	// blunt3
	USoundBase* Sound = nullptr;

	if (bCritHit)
	{
		Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Blunt3CritHitSounds);
	}
	else
	{
		if (HitSurface == SURFACETYPE_FLESH)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Blunt3FleshHitSounds);
		}
		else if (HitSurface == SURFACETYPE_METAL)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Blunt3MetalHitSounds);
		}
		else if (HitSurface == SURFACETYPE_STONE)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Blunt3StoneHitSounds);
		}
		else if (HitSurface == SURFACETYPE_UNDEAD)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Blunt3UndeadHitSounds);
		}
		else if (HitSurface == SURFACETYPE_WOOD)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Blunt3WoodHitSounds);
		}
		else
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Blunt3FleshHitSounds);
		}
	}

	return Sound;
}

inline USoundBase* AHumanCharacter::GetLongswordHitSound(const TEnumAsByte<EPhysicalSurface> HitSurface, const bool bCritHit) const
{
	// Slash1
	USoundBase* Sound = nullptr;

	if (bCritHit)
	{
		Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.SlashCritHitSounds);
	}
	else
	{
		if (HitSurface == SURFACETYPE_FLESH)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.SlashFleshHitSounds);
		}
		else if (HitSurface == SURFACETYPE_METAL)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.SlashMetalHitSounds);
		}
		else if (HitSurface == SURFACETYPE_STONE)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.SlashStoneHitSounds);
		}
		else if (HitSurface == SURFACETYPE_UNDEAD)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.SlashUndeadHitSounds);
		}
		else if (HitSurface == SURFACETYPE_WOOD)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.SlashWoodHitSounds);
		}
		else
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.SlashFleshHitSounds);
		}
	}

	return Sound;
}

inline USoundBase* AHumanCharacter::GetMaceHitSound(const TEnumAsByte<EPhysicalSurface> HitSurface, const bool bCritHit) const
{
	// blunt1
	USoundBase* Sound = nullptr;

	if (bCritHit)
	{
		Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.BluntCritHitSounds);
	}
	else
	{
		if (HitSurface == SURFACETYPE_FLESH)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.BluntFleshHitSounds);
		}
		else if (HitSurface == SURFACETYPE_METAL)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.BluntMetalHitSounds);
		}
		else if (HitSurface == SURFACETYPE_STONE)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.BluntStoneHitSounds);
		}
		else if (HitSurface == SURFACETYPE_UNDEAD)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.BluntUndeadHitSounds);
		}
		else if (HitSurface == SURFACETYPE_WOOD)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.BluntWoodHitSounds);
		}
		else
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.BluntFleshHitSounds);
		}
	}

	return Sound;
}

inline USoundBase* AHumanCharacter::GetStaffHitSound(const TEnumAsByte<EPhysicalSurface> HitSurface, const bool bCritHit) const
{
	// blunt2
	USoundBase* Sound = nullptr;

	if (bCritHit)
	{
		Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Blunt2CritHitSounds);
	}
	else
	{
		if (HitSurface == SURFACETYPE_FLESH)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Blunt2FleshHitSounds);
		}
		else if (HitSurface == SURFACETYPE_METAL)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Blunt2MetalHitSounds);
		}
		else if (HitSurface == SURFACETYPE_STONE)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Blunt2StoneHitSounds);
		}
		else if (HitSurface == SURFACETYPE_UNDEAD)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Blunt2UndeadHitSounds);
		}
		else if (HitSurface == SURFACETYPE_WOOD)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Blunt2WoodHitSounds);
		}
		else
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.Blunt2FleshHitSounds);
		}
	}

	return Sound;
}

inline USoundBase* AHumanCharacter::GetDaggerHitSound(const TEnumAsByte<EPhysicalSurface> HitSurface, const bool bCritHit) const
{
	// slice
	USoundBase* Sound = nullptr;

	if (bCritHit)
	{
		Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.SliceCritHitSounds);
	}
	else
	{
		if (HitSurface == SURFACETYPE_FLESH)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.SliceFleshHitSounds);
		}
		else if (HitSurface == SURFACETYPE_METAL)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.SliceMetalHitSounds);
		}
		else if (HitSurface == SURFACETYPE_STONE)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.SliceStoneHitSounds);
		}
		else if (HitSurface == SURFACETYPE_UNDEAD)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.SliceUndeadHitSounds);
		}
		else if (HitSurface == SURFACETYPE_WOOD)
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.SliceWoodHitSounds);
		}
		else
		{
			Sound = UEODBlueprintFunctionLibrary::GetRandomSound(WeaponHitSounds.SliceFleshHitSounds);
		}
	}

	return Sound;
}
