#include "ShipPawnBase.h"

#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LanunCharacterHeroBase.h"
#include "LanunPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"

AShipPawnBase::AShipPawnBase()
{
    PrimaryActorTick.bCanEverTick = true;

    ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
    RootComponent = ShipMesh;
    ShipMesh->SetSimulatePhysics(true);
    ShipMesh->SetCollisionProfileName(TEXT("PhysicsActor"));

    ShipMovementComponent = CreateDefaultSubobject<UShipMovementComponent>(TEXT("ShipMovementComponent"));

    RudderInteractiveComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RudderInteractiveComponent"));
    RudderInteractiveComponent->SetupAttachment(RootComponent);

    DisembarkLocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DisembarkLocationComponent"));
    DisembarkLocationComponent->SetupAttachment(RootComponent);

    BoardingGrappleComponent = CreateDefaultSubobject<USceneComponent>(TEXT("BoardingGrappleComponent"));
    BoardingGrappleComponent->SetupAttachment(RootComponent);
}

void AShipPawnBase::BeginPlay()
{
    Super::BeginPlay();
}

void AShipPawnBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (BoardState == EShipBoardState::Grappling && GrapplingTarget)
    {
        const FVector CurrentLoc = GetActorLocation();
        const FVector TargetLoc = GrapplingTarget->GetActorLocation();
        const FVector Direction = (TargetLoc - CurrentLoc).GetSafeNormal();
        const float Distance = FVector::Distance(CurrentLoc, TargetLoc);

        if (Distance > MaxGrappleDistance * 0.3f)
        {
            const FVector NewLoc = CurrentLoc + Direction * GrappleSpeed * DeltaTime;
            SetActorLocation(NewLoc);
        }
        else
        {
            BoardState = EShipBoardState::Boarded;
            OnBoardingEnded.Broadcast(ShipID, true);
        }
    }
}

void AShipPawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (!PlayerInputComponent)
    {
        return;
    }

    PlayerInputComponent->BindAxis(TEXT("TurnRudder"), this, &AShipPawnBase::InputRudder);
    PlayerInputComponent->BindAxis(TEXT("AdjustSailAngle"), this, &AShipPawnBase::InputAdjustSailAngle);
    PlayerInputComponent->BindAction(TEXT("RaiseSails"), IE_Pressed, this, &AShipPawnBase::InputRaiseSails);
    PlayerInputComponent->BindAction(TEXT("LowerSails"), IE_Pressed, this, &AShipPawnBase::InputLowerSails);
}

void AShipPawnBase::InputRudder(float Value)
{
    if (ShipMovementComponent)
    {
        ShipMovementComponent->AdjustRudderInput(Value);
    }
}

void AShipPawnBase::InputRaiseSails()
{
    if (!ShipMovementComponent)
    {
        return;
    }

    switch (ShipMovementComponent->CurrentSailState)
    {
    case ESailState::NoSails:
        ShipMovementComponent->SetSailState(ESailState::HalfSails);
        break;
    case ESailState::HalfSails:
        ShipMovementComponent->SetSailState(ESailState::FullSails);
        break;
    default:
        break;
    }
}

void AShipPawnBase::InputLowerSails()
{
    if (!ShipMovementComponent)
    {
        return;
    }

    switch (ShipMovementComponent->CurrentSailState)
    {
    case ESailState::FullSails:
        ShipMovementComponent->SetSailState(ESailState::HalfSails);
        break;
    case ESailState::HalfSails:
        ShipMovementComponent->SetSailState(ESailState::NoSails);
        break;
    default:
        break;
    }
}

void AShipPawnBase::InputAdjustSailAngle(float Value)
{
    if (ShipMovementComponent && !FMath::IsNearlyZero(Value))
    {
        ShipMovementComponent->AdjustSailAngleInput(Value);
    }
}

void AShipPawnBase::DisembarkShip(ALanunCharacterHeroBase* HeroCharacter)
{
    if (!HeroCharacter)
    {
        return;
    }

    if (ALanunPlayerController* PC = Cast<ALanunPlayerController>(GetController()))
    {
        HeroCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

        FVector DisembarkLocation = GetActorLocation();
        if (DisembarkLocationComponent)
        {
            DisembarkLocation = DisembarkLocationComponent->GetComponentLocation();
        }
        HeroCharacter->SetActorLocation(DisembarkLocation);

        PC->UnPossess();
        PC->Possess(HeroCharacter);
        PC->EnableOnFootContext();
    }
}

bool AShipPawnBase::GrappleShip(AShipPawnBase* TargetShip)
{
    if (!TargetShip || TargetShip == this)
    {
        return false;
    }

    if (BoardState != EShipBoardState::None)
    {
        return false;
    }

    const float Distance = FVector::Distance(GetActorLocation(), TargetShip->GetActorLocation());
    if (Distance > MaxGrappleDistance)
    {
        return false;
    }

    BoardState = EShipBoardState::Grappling;
    GrapplingTarget = TargetShip;
    OnBoardingStarted.Broadcast(TargetShip->ShipID, TargetShip);
    return true;
}

bool AShipPawnBase::BoardShip()
{
    if (BoardState != EShipBoardState::Grappling || !GrapplingTarget)
    {
        return false;
    }

    BoardState = EShipBoardState::Boarded;
    OnBoardingEnded.Broadcast(ShipID, true);
    return true;
}

void AShipPawnBase::DisembarkFromBoarding(ALanunCharacterHeroBase* HeroCharacter)
{
    if (!HeroCharacter)
    {
        return;
    }

    HeroCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    FVector DisembarkLocation = GetActorLocation();
    if (DisembarkLocationComponent)
    {
        DisembarkLocation = DisembarkLocationComponent->GetComponentLocation();
    }
    HeroCharacter->SetActorLocation(DisembarkLocation);

    BoardState = EShipBoardState::None;
    GrapplingTarget = nullptr;
    OnBoardingEnded.Broadcast(ShipID, true);

    if (ALanunPlayerController* PC = Cast<ALanunPlayerController>(GetController()))
    {
        PC->UnPossess();
        PC->Possess(HeroCharacter);
        PC->EnableOnFootContext();
    }
}
