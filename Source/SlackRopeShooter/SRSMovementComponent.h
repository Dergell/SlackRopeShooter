// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SRSMovementComponent.generated.h"

UENUM(BlueprintType)
enum ECustomMovementMode
{
	None UMETA(DisplayName = "None"),
	Grappling UMETA(DisplayName = "Grappling")
};

UCLASS()
class SLACKROPESHOOTER_API USRSMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
	// Process custom movement modes
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	// Called after MovementMode has changed.
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

public:
	/*
	 * Handles grappling movement mode. Movement update functions should only be called through StartNewPhysics()
	 *
	 * This is mostly a copy of the original PhysFalling() with an added velocity limit when the grapple cable length is reached. 
	 */
	void PhysGrappling(float deltaTime, int32 Iterations);

private:
	void HandleGrappleLimits();
};