// IBSZhu 


#include "Player/AuraPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);
	if(!CursorHit.bBlockingHit) return;//未命中

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	/**
	 * Line trace from cursor. there are several scenarios:
	 * A. LastActor is null & ThisActor is null
	 *		-Do nothing.
	 * B. LastActor is null & ThisActor is valid
	 *		-Highlight ThisActor.
	 * C. LastActor is valid & ThisActor is null
	 *		-UnHighlight LastActor.
	 * D. Both Actors are valid, but LastActor != ThisActor
	 *		-Highlight ThisActor & UnHighlight LastActor.
	 * E. Both Actors are valid, and LastActor = ThisActor
	 *		-Do nothing.
	 */

	/*if(LastActor == nullptr && ThisActor == nullptr)
	{
		
	}
	else if(LastActor == nullptr && ThisActor != nullptr)
	{
		ThisActor->HighlightActor();
	}
	else if(LastActor != nullptr && ThisActor == nullptr)
	{
		LastActor->UnHighlightActor();
	}
	else if(LastActor != nullptr && ThisActor != nullptr && LastActor != ThisActor)
	{
		ThisActor->HighlightActor();
		LastActor->UnHighlightActor();
	}
	else if(LastActor != nullptr && ThisActor != nullptr && LastActor != ThisActor)
	{
		
	}*/
	if(LastActor == nullptr)
	{
		if(ThisActor != nullptr)
		{
			//case B
			ThisActor->HighlightActor();
		}
		//case A, do nothing
	}
	else //LastActor is valid
	{
		if(ThisActor == nullptr)
		{
			//case C
			LastActor->UnHighlightActor();
		}
		else //Both are valid
		{
			if(LastActor != ThisActor)
			{
				//case D
				ThisActor->HighlightActor();
				LastActor->UnHighlightActor();
			}
			//case E, do nothing
		}
	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext,0);
	}
	

	//Cursor Settings
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent =CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);
	
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2d InputAxisVector = InputActionValue.Get<FVector2d>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f,Rotation.Yaw,0.f);
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection,InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection,InputAxisVector.X); 
	}
}


