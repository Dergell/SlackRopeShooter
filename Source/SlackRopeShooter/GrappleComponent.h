// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrappleComponent.generated.h"

class ASlackRopeShooterCharacter;
class UCableComponent;
class UInputAction;
class UInputMappingContext;
class UPhysicsConstraintComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLACKROPESHOOTER_API UGrappleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGrappleComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Ends gameplay for this component
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Check if grapple is active
	bool IsGrappling() const { return bIsGrappleActive; }

	// Check if the cable length is exceeded
	bool IsExceedingCableLength() const;

	// Get cable direction
	FVector GetCableVector() const;

	// Get vector from current cable start location to where it should be limited by length
	FVector GetGrappleOffset() const;

protected:
	// Called from grappling input
	void Grapple();

	// Attach the grapple to a location
	void ApplyGrapple(const FHitResult HitResult);

	// Immediately stop the grappling
	void CancelGrapple();

private:
	// Trace to find a grapple target
	FHitResult GetGrappleTarget() const;

	// Applies a force to the grappled object towards the cable
	void DragGrappledObject(UPrimitiveComponent* Object) const;

private:
	// Grapple Input Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* GrappleMappingContext;

	// Grapple Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* GrappleAction;

	// Max reach of grapple
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Grapple, meta=(AllowPrivateAccess = "true"))
	float GrappleReach = 3000.f;

	// The character using this component
	UPROPERTY()
	ASlackRopeShooterCharacter* Character;

	// The character using this component
	UPROPERTY()
	UCableComponent* Cable;

	// Are we current grappling something
	bool bIsGrappleActive = false;
};