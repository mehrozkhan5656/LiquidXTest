// Copyright Epic Games, Inc. All Rights Reserved.

#include "LiquidXTestCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"


//////////////////////////////////////////////////////////////////////////
// ALiquidXTestCharacter

ALiquidXTestCharacter::ALiquidXTestCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ALiquidXTestCharacter::BeginPlay()
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
	SetActorTickEnabled(true);
	//PhysicsHandleComponent = NewObject<UPhysicsHandleComponent>(this,UPhysicsHandleComponent::StaticClass(),TEXT("GrabHandler"));
}

void ALiquidXTestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(IsJetpackEnable)
		AddMovementInput(FVector(0,0,1.0f));
	
	//Just To Show I tried to Do in c++ but I know in Tick I was getting an issue to assign the position
	//Due to lack of time I am undoing it.
	// if(IsObjectGrabbed)
	// {
	// 	PhysicsHandleComponent->SetTargetLocation
	// 	(GetFollowCamera()->GetRelativeLocation()+GetFollowCamera()->GetRelativeRotation().Vector().ForwardVector * 150);
	// }
}

//////////////////////////////////////////////////////////////////////////
// Input

void ALiquidXTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALiquidXTestCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALiquidXTestCharacter::Look);

	}

}

void ALiquidXTestCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ALiquidXTestCharacter::Look(const FInputActionValue& Value)
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
void ALiquidXTestCharacter::EnableJetPack(bool IsEnable)
{
	IsJetpackEnable = IsEnable;
	GetCharacterMovement()->SetMovementMode(IsJetpackEnable ? MOVE_Flying:MOVE_Falling);
	GetCharacterMovement()->AirControl = IsJetpackEnable ? 0.6f:0.2f;
}
void ALiquidXTestCharacter::HideActor()
{
	FHitResult HitResult;
	if(GetActorByLineTrace(HitResult))
	{
		HitResult.GetActor()->SetActorHiddenInGame(true);
	}
}

bool ALiquidXTestCharacter::GetActorByLineTrace(FHitResult& OutHitResult)
{
	FCollisionQueryParams TraceParms;
	FVector StartPoint = GetCameraBoom()->GetComponentLocation();
	FVector EndPoint = StartPoint + UKismetMathLibrary::GetForwardVector(GetCameraBoom()->GetComponentRotation()) * 500;
	DrawDebugLine(GetWorld(),StartPoint,EndPoint,FColor(255,0,0),true,1,0,2.5f);
	return GetWorld()->LineTraceSingleByChannel(OutHitResult,StartPoint,EndPoint,ECC_Visibility, TraceParms);
}

bool ALiquidXTestCharacter::CheckForDoor(FHitResult& OutHitResult)
{
	//FHitResult HitResult;
	if(GetActorByLineTrace(OutHitResult))
	{
		UE_LOG(LogTemp, Warning, TEXT("Mehroz %s"), *OutHitResult.GetActor()->GetName());
		if(OutHitResult.GetComponent()->ComponentHasTag("Door"))
			return true;
	}
	return false;
}

#pragma region JustToShowTheEffort
//Just To Show I tried to Do in c++ but I know in Tick I was getting an issue to assign the position
//Due to lack of time I am undoing it.
// void ALiquidXTestCharacter::GrabObject(FVector StartPoint, FVector EndPoint)
// {
// 	UE_LOG(LogTemp, Warning, TEXT("Mehroz Grab Called"));
//
// 	if(PhysicsHandleComponent == nullptr) return;
// 	FHitResult HitResult;
// 	FCollisionQueryParams TraceParms;
// 	DrawDebugLine(GetWorld(),StartPoint,EndPoint,FColor(255,0,0),true,5,0,12.33f);
// 	UE_LOG(LogTemp, Warning, TEXT("Mehroz Object Grabbed"));
// 	 IsObjectGrabbed = GetWorld()->LineTraceSingleByChannel(HitResult,StartPoint,EndPoint,ECC_Visibility, TraceParms);
// 	if(IsObjectGrabbed)
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("Mehroz Object Grabbed Name is::: %s"),*HitResult.GetComponent()->GetName());
// 		
// 		PhysicsHandleComponent->GrabComponentAtLocationWithRotation
// 		(HitResult.GetComponent(),"None",StartPoint,FRotator::ZeroRotator);
// 		
// 	}
// }
// void ALiquidXTestCharacter::ReleaseObject()
// {
// 	if(IsObjectGrabbed)
// 	{
// 		PhysicsHandleComponent->ReleaseComponent();
// 		IsObjectGrabbed = false;
// 	}
// }
#pragma endregion


