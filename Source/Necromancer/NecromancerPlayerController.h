#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "NecromancerPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class ANecromancerPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANecromancerPlayerController();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	/** Mouse Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;
	/** Touch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationTouchAction;

    /** Camera Actions */
    /** Mouse Move Camera Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetCameraMoveClickAction;
    /** Gesture Move Camera Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetCameraMoveTouchAction;

    /** Keybind Zoom In Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetZoomInBindingAction;
	/** Spread Zoom In Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetZoomInGestureAction;

    /** Keybind Zoom Out Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetZoomOutBindingAction;
	/** Pinch Zoom Out Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetZoomOutGestureAction;

    // Limits
    float CamMinZoom = 800.f;  
    float CamMaxZoom = 3000.f;
    float CamPitchMin = -70.f;
    float CamPitchMax = -20.f;

    // Amounts
    float CamZoomStep = 100.f;
    FVector2f CamMoveMag { 3.0f, 3.0f };
    

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;
	
	// To add mapping context
	virtual void BeginPlay();

	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();

    /** Section: Camera **/
	void OnSetZoomInTriggered();
	// void OnGestureZoomInTriggered();

	void OnSetZoomOutTriggered();
	// void OnGestureZoomOutTriggered();

    void OnCameraMoveStarted();
    void OnCameraMoveTriggered();
    void OnCameraMoveReleased();
    void OnGestureCameraMoveStarted();
    void OnGestureCameraMoveReleased();

private:
	FVector CachedDestination;
    FVector2f CachedScreenInputPos;

	bool bIsTouch; // Is it a touch device
	float FollowTime; // For how long it has been pressed
};


