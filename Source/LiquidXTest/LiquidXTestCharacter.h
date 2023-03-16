// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "LiquidXTestCharacter.generated.h"


UCLASS(config=Game)
class ALiquidXTestCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

public:
	ALiquidXTestCharacter();
	//For Jetpack
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsJetpackEnable;
	//For Grab and release objects
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Grab Component")
	UPhysicsHandleComponent* PhysicsHandleComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsObjectGrabbed;
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	// TO enable and disable Jetpack
	UFUNCTION(BlueprintCallable)
	void EnableJetPack(bool IsEnable);

#pragma region JustToShowTheEffort
	//Just To Show I tried to Do in c++ but I know in Tick I was getting an issue to assign the position
	//Due to lack of time I am undoing it.
	// TO Grab Object
	// UFUNCTION(BlueprintCallable)
	// void GrabObject(FVector StartPoint, FVector EndPoint);
	// //To Release Object
	// UFUNCTION(BlueprintCallable)
	// void ReleaseObject();
#pragma endregion
	
	// To Hide an Actor
	UFUNCTION(BlueprintCallable)
	void HideActor();

	// TO Get The Hitting Actor By LineTrace
	UFUNCTION(BlueprintCallable)
	bool GetActorByLineTrace(FHitResult& OutHitResult);

	//To Detect The Door Actor
	UFUNCTION(BlueprintCallable)
	bool CheckForDoor(FHitResult& OutHitResult);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();
	
	//To Move Jetpack UP.
	virtual void Tick(float DeltaTime);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

