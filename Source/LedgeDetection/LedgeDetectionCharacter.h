// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Components/ArrowComponent.h"
#include "LedgeDetectionCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class ALedgeDetectionCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALedgeDetectionCharacter();

protected:
	// Camera components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;


	///////// COPY FROM HERE START /////////

	// Movement variables
	float InterpSpeed = 10.0f;
	float AddFlt = 1.f;

	// simple do once
	bool stopped = false;
	bool played = false;

	// Arrow Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* NArrowLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* NArrowRight;


	// Raycast timer and related variables
	FTimerHandle NTimerHandle;
	void RaycastToBottom();

	FCollisionQueryParams RParams;
	const FName RTraceTag = "line1";
	FHitResult RHitResult;
	FVector RStart;
	FVector REnd;
	float RArrowToImpact;

	FCollisionQueryParams LParams;
	const FName LTraceTag = "line2";
	FHitResult LHitResult;
	FVector LStart;
	FVector LEnd;
	float LArrowToImpact;

	FCollisionQueryParams CenterParams;
	const FName CenterTraceTag = "line3";
	FHitResult CenterHitResult;
	float CenterArrowToImpact;
	FVector CenterStart;
	FVector CenterEnd;

	// Control the block and detection
	float ArrowInBetweenDistance = 40; // Tweak this value for cast trace further away from each other i.e left and right 
	float ArrowToPlayerDistance = 40; // Tweak this value for cast trace further away from player
	float TimerDelay = 0.01; // Tweak this value to LESS for faster more precise trace
	float FallThreshHold = 170.0f; // Tweak this value to change ledge height threshhold

	// Trigger once on input pressed and released
	void TriggerStartMove(const FInputActionValue& Value);
	void TriggerEndMove(const FInputActionValue& Value);

	///////// COPY FROM HERE END /////////

	// Input callbacks
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	// ACharacter overrides
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;

public:
	// Getters
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
