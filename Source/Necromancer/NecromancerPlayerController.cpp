#include "NecromancerPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NecromancerCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

#include "GameFramework/SpringArmComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ANecromancerPlayerController::ANecromancerPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void ANecromancerPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void ANecromancerPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ANecromancerPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ANecromancerPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ANecromancerPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ANecromancerPlayerController::OnSetDestinationReleased);
		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &ANecromancerPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &ANecromancerPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &ANecromancerPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &ANecromancerPlayerController::OnTouchReleased);

        // Zoom In input events
        EnhancedInputComponent->BindAction(SetZoomInBindingAction, ETriggerEvent::Triggered, this, &ANecromancerPlayerController::OnSetZoomInTriggered);
		// EnhancedInputComponent->BindAction(SetZoomInBindingAction, ETriggerEvent::Completed, this, &ANecromancerPlayerController::OnSetZoomInReleased);
		// EnhancedInputComponent->BindAction(SetZoomInBindingAction, ETriggerEvent::Canceled, this, &ANecromancerPlayerController::OnSetZoomInReleased);
        // // Zoom In gesture events
        // EnhancedInputComponent->BindAction(SetZoomInGestureAction, ETriggerEvent::Triggered, this, &ANecromancerPlayerController::OnGestureZoomInTriggered);
		// EnhancedInputComponent->BindAction(SetZoomInGestureAction, ETriggerEvent::Completed, this, &ANecromancerPlayerController::OnGestureZoomInReleased);
		// EnhancedInputComponent->BindAction(SetZoomInGestureAction, ETriggerEvent::Canceled, this, &ANecromancerPlayerController::OnGestureZoomInReleased);

        // Zoom Out input events
        EnhancedInputComponent->BindAction(SetZoomOutBindingAction, ETriggerEvent::Triggered, this, &ANecromancerPlayerController::OnSetZoomOutTriggered);
		// EnhancedInputComponent->BindAction(SetZoomOutBindingAction, ETriggerEvent::Completed, this, &ANecromancerPlayerController::OnSetZoomOutReleased);
		// EnhancedInputComponent->BindAction(SetZoomOutBindingAction, ETriggerEvent::Canceled, this, &ANecromancerPlayerController::OnSetZoomOutReleased);
        // // Zoom Out gesture events
        // EnhancedInputComponent->BindAction(SetZoomOutGestureAction, ETriggerEvent::Triggered, this, &ANecromancerPlayerController::OnGestureZoomOutTriggered);
		// EnhancedInputComponent->BindAction(SetZoomOutGestureAction, ETriggerEvent::Completed, this, &ANecromancerPlayerController::OnGestureZoomOutReleased);
		// EnhancedInputComponent->BindAction(SetZoomOutGestureAction, ETriggerEvent::Canceled, this, &ANecromancerPlayerController::OnGestureZoomOutReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ANecromancerPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void ANecromancerPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void ANecromancerPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void ANecromancerPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void ANecromancerPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void ANecromancerPlayerController::OnSetZoomInTriggered()
{
    UE_LOG(LogTemplateCharacter, Verbose, TEXT("'%s' Zoom in triggered."), *GetNameSafe(this));

    USpringArmComponent* CameraBoom = GetPawn()->FindComponentByClass<USpringArmComponent>();
    if (CameraBoom)
    {
        if (CameraBoom->TargetArmLength > fMinZoom)
        {
            CameraBoom->TargetArmLength -= fZoomStep;
        }
        if (CameraBoom->TargetArmLength < fMinZoom)
        {
            CameraBoom->TargetArmLength = fMinZoom;
        }
    }
}

void ANecromancerPlayerController::OnSetZoomOutTriggered()
{
    UE_LOG(LogTemplateCharacter, Verbose, TEXT("'%s' Zoom out triggered."), *GetNameSafe(this));

    USpringArmComponent* CameraBoom = GetPawn()->FindComponentByClass<USpringArmComponent>();
    if (CameraBoom)
    {
        if (CameraBoom->TargetArmLength < fMaxZoom)
        {
            CameraBoom->TargetArmLength += fZoomStep;
        }
        if (CameraBoom->TargetArmLength > fMaxZoom)
        {
            CameraBoom->TargetArmLength = fMaxZoom;
        }
    }
}