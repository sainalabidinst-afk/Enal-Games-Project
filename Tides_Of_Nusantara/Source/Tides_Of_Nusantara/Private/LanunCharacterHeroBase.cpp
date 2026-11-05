#include "LanunCharacterHeroBase.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "LanunPlayerController.h"
#include "ShipPawnBase.h"

ALanunCharacterHeroBase::ALanunCharacterHeroBase()
{
    PrimaryActorTick.bCanEverTick = true;

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    AttributeSet = CreateDefaultSubobject<ULanunAttributeSetBase>(TEXT("AttributeSet"));

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
    GetCharacterMovement()->GetNavAgentPropertiesRef().bCanSwim = true;
}

UAbilitySystemComponent* ALanunCharacterHeroBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ALanunCharacterHeroBase::BeginPlay()
{
    Super::BeginPlay();

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

void ALanunCharacterHeroBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GetCharacterMovement()->IsSwimming())
    {
        UpdateSwimmingBuoyancyPhysics();
    }
}

void ALanunCharacterHeroBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!EnhancedInputComponent)
    {
        return;
    }

    if (MoveAction)
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALanunCharacterHeroBase::Move);
    }

    if (LookAction)
    {
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALanunCharacterHeroBase::Look);
    }

    if (JumpAction)
    {
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
    }

    if (InteractAction)
    {
        EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ALanunCharacterHeroBase::InteractInput);
    }

    if (ParryAction)
    {
        EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Started, this, &ALanunCharacterHeroBase::ParryInput);
    }
}

void ALanunCharacterHeroBase::Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();

    if (!Controller)
    {
        return;
    }

    const FRotator ControlRotation = Controller->GetControlRotation();
    const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);
    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, MovementVector.Y);
    AddMovementInput(RightDirection, MovementVector.X);
}

void ALanunCharacterHeroBase::Look(const FInputActionValue& Value)
{
    const FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller)
    {
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void ALanunCharacterHeroBase::InteractWithShip(AShipPawnBase* TargetShip)
{
    if (!TargetShip)
    {
        return;
    }

    if (ALanunPlayerController* PC = Cast<ALanunPlayerController>(GetController()))
    {
        PC->UnPossess();
        PC->Possess(TargetShip);

        PC->EnableSailingContext();

        if (UShipMovementComponent* ShipMovement = TargetShip->GetShipMovementComponent())
        {
            (void)ShipMovement;
        }

        if (USceneComponent* RudderComp = TargetShip->GetRudderInteractiveComponent())
        {
            AttachToComponent(RudderComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        }
    }
}

void ALanunCharacterHeroBase::InteractInput()
{
    // Input handler for Interact action. Override or extend to find nearby ship target.
}

void ALanunCharacterHeroBase::ParryInput()
{
    // Trigger Tangkas Salawaku parry ability via AbilitySystemComponent
    if (AbilitySystemComponent)
    {
        // Ability activation handled by AbilitySystemComponent input binding
        // This can also trigger gameplay events directly if needed
    }
}

void ALanunCharacterHeroBase::DisembarkFromShip()
{
    // Find current ship pawn we are possessing
    AShipPawnBase* CurrentShip = Cast<AShipPawnBase>(GetPawn());
    if (!CurrentShip)
    {
        return;
    }

    if (ALanunPlayerController* PC = Cast<ALanunPlayerController>(GetController()))
    {
        PC->UnPossess();
        PC->Possess(this);

        PC->EnableOnFootContext();

        // Move hero to disembark location
        if (USceneComponent* DisembarkComp = CurrentShip->GetDisembarkLocationComponent())
        {
            SetActorLocationAndRotation(DisembarkComp->GetComponentLocation(), DisembarkComp->GetComponentRotation());
        }
    }
}

void ALanunCharacterHeroBase::UpdateSwimmingBuoyancyPhysics()
{
    if (!AttributeSet)
    {
        return;
    }

    const float Buoyancy = AttributeSet->GetSwimmingBuoyancy();
    if (Buoyancy < 0.0f)
    {
        GetCharacterMovement()->AddImpulse(FVector(0.0f, 0.0f, Buoyancy * 500.0f), true);
    }
}
