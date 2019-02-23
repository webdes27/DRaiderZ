// Copyright 2018 Moikkai Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EODCharacterBase.h"
#include "RideBase.generated.h"

/**
 * 
 */
UCLASS()
class EOD_API ARideBase : public AEODCharacterBase
{
	GENERATED_BODY()
	
public:
	ARideBase(const FObjectInitializer& ObjectInitializer);

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;


	////////////////////////////////////////////////////////////////////////////////
	// Ride System
public:

protected:

private:




};
