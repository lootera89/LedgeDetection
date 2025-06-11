// Copyright Epic Games, Inc. All Rights Reserved.

#include "LedgeDetectionCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ALedgeDetectionCharacter

ALedgeDetectionCharacter::ALedgeDetectionCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(20.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxAcceleration = 1500.f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	///////// COPY FROM HERE START /////////

    // Create front left arrow component
	NArrowLeft = CreateDefaultSubobject<UArrowComponent>(TEXT("NArrowLeft"));
	NArrowLeft->SetupAttachment(RootComponent);
	NArrowLeft->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	NArrowLeft->SetWorldLocation(FVector(ArrowToPlayerDistance, -ArrowInBetweenDistance, -80.0f));
	NArrowLeft->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));

    // Create front right arrow component
    NArrowRight = CreateDefaultSubobject<UArrowComponent>(TEXT("NArrowRight"));
    NArrowRight->SetupAttachment(RootComponent);
    NArrowRight->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
    NArrowRight->SetWorldLocation(FVector(ArrowToPlayerDistance, ArrowInBetweenDistance, -80.0f));
    NArrowRight->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));

	///////// COPY FROM HERE END /////////

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ALedgeDetectionCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ALedgeDetectionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALedgeDetectionCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALedgeDetectionCharacter::Look);

		///////// COPY FROM HERE START /////////

		// Trigger Start Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &ALedgeDetectionCharacter::TriggerStartMove);

		// Trigger End Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ALedgeDetectionCharacter::TriggerEndMove);

		///////// COPY FROM HERE END /////////
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ALedgeDetectionCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		///////// COPY FROM HERE START (ONLY COPY THIS IF YOU HAVE OrientRotationToMovement DISABLED IN PLAYER MOVEMENT) /////////

		AddMovementInput(ForwardDirection, MovementVector.Y * AddFlt);
		AddMovementInput(RightDirection, MovementVector.X * AddFlt);

		// Rotate player with input
		if (!MovementVector.IsNearlyZero())
		{
			const FVector MoveDir = (ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X).GetSafeNormal();
			if (!MoveDir.IsNearlyZero())
			{
				FRotator TargetRot = MoveDir.Rotation();
				TargetRot.Pitch = 0.f;
				TargetRot.Roll = 0.f;

				// Smooth interpolation
				FRotator CurrentRot = GetActorRotation();
				FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, GetWorld()->GetDeltaSeconds(), InterpSpeed);
				SetActorRotation(NewRot);

			}
		}
	} 	///////// COPY FROM HERE END /////////
}


void ALedgeDetectionCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

///////// COPY FROM HERE START /////////

//////////////////////////////////////////////////////////////////////////
// Line Trace

//Start Timer on input begin
void ALedgeDetectionCharacter::TriggerStartMove(const FInputActionValue& Value)
{
	if (GetWorld()->GetTimerManager().IsTimerPaused(NTimerHandle))
	{
		GetWorld()->GetTimerManager().UnPauseTimer(NTimerHandle);
	}
	else{
	GetWorld()->GetTimerManager().SetTimer(NTimerHandle, this, &ALedgeDetectionCharacter::RaycastToBottom, TimerDelay, true);
	}
}
//Pause Timer on input end
void ALedgeDetectionCharacter::TriggerEndMove(const FInputActionValue& Value)
{
	GetWorld()->GetTimerManager().PauseTimer(NTimerHandle);
	if (!played) {
		AddFlt = 1.f;
		stopped = false;
		played = true;
	}
}

void ALedgeDetectionCharacter::RaycastToBottom()
{
	// Set line Left trace values
	LStart = NArrowLeft->GetComponentLocation();
	LEnd = FVector(LStart.X, LStart.Y, LStart.Z - 1000);
	LParams.AddIgnoredActor(this);

	//// Debug lines (uncomment to visualize)
	//LParams.TraceTag = LTraceTag;
	//GetWorld()->DebugDrawTraceTag = LTraceTag;

	// Initiate trace
	if (GetWorld()->LineTraceSingleByChannel(LHitResult, LStart, LEnd, ECollisionChannel::ECC_Pawn, LParams)) {
		LArrowToImpact = NArrowLeft->GetComponentLocation().Z - LHitResult.ImpactPoint.Z;
	}

	// Set line Right trace values
	RStart = NArrowRight->GetComponentLocation();
	REnd = FVector(RStart.X, RStart.Y, RStart.Z - 1000);
	RParams.AddIgnoredActor(this);

	//// Debug lines (uncomment to visualize)
	//RParams.TraceTag = RTraceTag;
	//GetWorld()->DebugDrawTraceTag = RTraceTag;

	// Initiate trace
	if (GetWorld()->LineTraceSingleByChannel(RHitResult, RStart, REnd, ECollisionChannel::ECC_Pawn, RParams)) {
		RArrowToImpact = NArrowRight->GetComponentLocation().Z - RHitResult.ImpactPoint.Z;
		// Draw debug line
	}

	// Set line Center trace values
    CenterStart = (NArrowLeft->GetComponentLocation() + NArrowRight->GetComponentLocation()) * 0.5f;
    FVector Forward = GetActorForwardVector();
    CenterStart += Forward * 20.0f;
	CenterEnd = FVector(CenterStart.X, CenterStart.Y, CenterStart.Z - 1000);
	CenterParams.AddIgnoredActor(this);

	//// Debug lines (uncomment to visualize)
	//CenterParams.TraceTag = CenterTraceTag;
	//GetWorld()->DebugDrawTraceTag = CenterTraceTag;

	// Initiate trace
	if (GetWorld()->LineTraceSingleByChannel(CenterHitResult, CenterStart, CenterEnd, ECollisionChannel::ECC_Pawn, CenterParams)) {
		CenterArrowToImpact = CenterStart.Z - CenterHitResult.ImpactPoint.Z;
	}

	//Handle input
	if (!GetCharacterMovement()->IsFalling()) {
		//Is not Jumping
		if (LArrowToImpact >= FallThreshHold || RArrowToImpact >= FallThreshHold || CenterArrowToImpact >= FallThreshHold) {
            //In Front of a ledge
			if(!stopped){
			AddFlt = 0.00001f;
			GetCharacterMovement()->StopMovementImmediately();
			stopped = true;
			played = false;
			}
		}
		else {
			//Walking on normal plane
			if (!played) {
			AddFlt = 1.f;
			stopped = false;
			played = true;
			}
		}
	}
	else {
		//Is Jumping
		if (!played) {
			AddFlt = 1.f;
			stopped = false;
			played = true;
		}
	}
	
}
///////// COPY FROM HERE END /////////