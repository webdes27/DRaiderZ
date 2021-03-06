// Copyright 2018 Moikkai Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InteractionInterface.h"

#include "GameFramework/Actor.h"
#include "InteractiveActor.generated.h"

UCLASS()
class EOD_API AInteractiveActor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	

	// --------------------------------------
	//  UE4 Method Overrides
	// --------------------------------------

	AInteractiveActor(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// --------------------------------------
	//  Interaction Interface
	// --------------------------------------

	/** This event is called to highlight interactive actor's meshes */
	virtual void EnableCustomDepth_Implementation() override;

	/** This event is called to disable highlight on interactive actor's meshes */
	virtual void DisableCustomDepth_Implementation() override;

	virtual void OnGainFocus_Implementation(AEODCharacterBase* Character) override;

	virtual void OnLoseFocus_Implementation(AEODCharacterBase* Character) override;

};
