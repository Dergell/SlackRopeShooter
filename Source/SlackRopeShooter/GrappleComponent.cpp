// Fill out your copyright notice in the Description page of Project Settings.

#include "GrappleComponent.h"

#include "CableComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SlackRopeShooterCharacter.h"
#include "SRSMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UGrappleComponent::UGrappleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrappleComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = GetOwner<ASlackRopeShooterCharacter>();
	if (!Character)
	{
		return;
	}

	// Set up action bindings
	const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(GrappleMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(GrappleAction, ETriggerEvent::Triggered, this, &UGrappleComponent::Grapple);
		}
	}
}

void UGrappleComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!Character)
	{
		return;
	}

	const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(GrappleMappingContext);
		}
	}
}

// Called every frame
void UGrappleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(Cable) || !IsValid(Cable->GetAttachedComponent()))
	{
		return;
	}

	UPrimitiveComponent* AttachedPrimitive = Cast<UPrimitiveComponent>(Cable->GetAttachedComponent());
	if (AttachedPrimitive && AttachedPrimitive->IsSimulatingPhysics(Cable->AttachEndToSocketName))
	{
		DragGrappledObject(AttachedPrimitive);
	}
}

bool UGrappleComponent::IsExceedingCableLength() const
{
	if (GetCableVector().Length() > Cable->CableLength)
	{
		return true;
	}

	return false;
}

FVector UGrappleComponent::GetCableVector() const
{
	const FVector CableStart = Cable->GetComponentLocation();
	const FVector CableEnd = UKismetMathLibrary::TransformLocation(Cable->GetAttachedComponent()->GetComponentTransform(), Cable->EndLocation);

	return CableEnd - CableStart;
}

FVector UGrappleComponent::GetGrappleOffset() const
{
	const FVector CableEnd = UKismetMathLibrary::TransformLocation(Cable->GetAttachedComponent()->GetComponentTransform(), Cable->EndLocation);
	const FVector InvertedCableVector = Cable->GetComponentLocation() - CableEnd;
	const FVector LimitedCableStart = CableEnd + InvertedCableVector.GetSafeNormal() * Cable->CableLength;
	return LimitedCableStart - Cable->GetComponentLocation();
}

void UGrappleComponent::Grapple()
{
	if (!Character)
	{
		return;
	}

	if (bIsGrappleActive)
	{
		CancelGrapple();
		return;
	}

	const FHitResult GrappleTarget = GetGrappleTarget();
	if (GrappleTarget.IsValidBlockingHit())
	{
		ApplyGrapple(GrappleTarget);
	}
}

void UGrappleComponent::ApplyGrapple(const FHitResult HitResult)
{
	Cable = Cast<UCableComponent>(Character->AddComponentByClass(
		UCableComponent::StaticClass(), false, FTransform::Identity, true));

	if (!Cable)
	{
		return;
	}

	Cable->NumSegments = 1;
	Cable->CableWidth = 5;
	Cable->CableLength = FVector::Distance(HitResult.ImpactPoint, Character->GetActorLocation());
	Cable->EndLocation = UKismetMathLibrary::InverseTransformLocation(HitResult.Component->GetComponentTransform(), HitResult.ImpactPoint);
	Cable->SetAttachEndToComponent(HitResult.Component.Get(), HitResult.BoneName);

	Character->GetCharacterMovement()->SetMovementMode(MOVE_Custom, Grappling);

	Character->FinishAddComponent(Cable, false, FTransform::Identity);
	bIsGrappleActive = true;
}

void UGrappleComponent::CancelGrapple()
{
	if (!Cable)
	{
		return;
	}

	Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	Cable->DestroyComponent();
	bIsGrappleActive = false;
}

FHitResult UGrappleComponent::GetGrappleTarget() const
{
	const FVector TraceStart = Character->GetFirstPersonCameraComponent()->GetComponentLocation();
	const FVector TraceEnd = TraceStart + Character->GetFirstPersonCameraComponent()->GetForwardVector() * GrappleReach;
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	const TArray<AActor*> IgnoredActors;

	FHitResult HitResult;
	const bool Hit = UKismetSystemLibrary::SphereTraceSingle(
		this, TraceStart, TraceEnd, 10.f, TraceType,
		false, IgnoredActors, EDrawDebugTrace::None, HitResult, true
	);

	return HitResult;
}

void UGrappleComponent::DragGrappledObject(UPrimitiveComponent* Object) const
{
	if (IsExceedingCableLength())
	{
		const FVector CableEnd = UKismetMathLibrary::TransformLocation(Cable->GetAttachedComponent()->GetComponentTransform(), Cable->EndLocation);
		const FVector Direction = -GetCableVector().GetSafeNormal();
		FVector Force = Direction * (GetCableVector().Length() - Cable->CableLength);

		Object->WakeRigidBody(Cable->AttachEndToSocketName);
		Object->AddVelocityChangeImpulseAtLocation(Force, CableEnd, Cable->AttachEndToSocketName);
	}
}