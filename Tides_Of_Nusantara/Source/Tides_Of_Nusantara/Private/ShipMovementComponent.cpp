#include "ShipMovementComponent.h"
#include "ShipMovementComponent.h"
#include "CalendarWorldSubsystem.h"
#include "Components/PrimitiveComponent.h"
#include "CrewOfficerSubsystem.h"
#include "GameFramework/Actor.h"

UShipMovementComponent::UShipMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UShipMovementComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UShipMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return;
    }

    UPrimitiveComponent* PrimitiveRoot = Cast<UPrimitiveComponent>(Owner->GetRootComponent());
    if (!PrimitiveRoot || !PrimitiveRoot->IsSimulatingPhysics())
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    FShipBonusStats BonusStats;
    UCrewOfficerSubsystem* OfficerSubsystem = World->GetSubsystem<UCrewOfficerSubsystem>();
    if (OfficerSubsystem && !ShipID.IsNone())
    {
        BonusStats = OfficerSubsystem->CalculateShipBonusStats(ShipID);
    }

    FVector WindDirection = FVector::ForwardVector;
    UCalendarWorldSubsystem* CalendarSubsystem = World->GetSubsystem<UCalendarWorldSubsystem>();
    if (CalendarSubsystem)
    {
        WindDirection = CalendarSubsystem->GetWindDirectionVector();
    }

    float SailFactor = 0.0f;
    if (CurrentSailState == ESailState::HalfSails)
    {
        SailFactor = 0.5f;
    }
    else if (CurrentSailState == ESailState::FullSails)
    {
        SailFactor = 1.0f;
    }

    const FVector ForwardVector = Owner->GetActorForwardVector();
    const FVector EffectiveSailForward = ForwardVector.RotateAngleAxis(CurrentSailAngleInput * 45.0f, FVector::UpVector);
    const float WindDot = FMath::Max(0.1f, FVector::DotProduct(EffectiveSailForward.GetSafeNormal(), WindDirection.GetSafeNormal()));

    const float FinalThrust = MaxBaseThrust * SailFactor * WindDot * BonusStats.ThrustMultiplier;
    if (!FMath::IsNearlyZero(FinalThrust))
    {
        PrimitiveRoot->AddForce(ForwardVector * FinalThrust);
    }

    if (!FMath::IsNearlyZero(CurrentRudderInput))
    {
        const float FinalTorque = BaseRudderTorque * CurrentRudderInput * BonusStats.RudderTorqueMultiplier;
        PrimitiveRoot->AddTorqueInRadians(FVector::UpVector * FinalTorque);
    }
}

void UShipMovementComponent::AdjustRudderInput(float Value)
{
    CurrentRudderInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void UShipMovementComponent::AdjustSailAngleInput(float Value)
{
    CurrentSailAngleInput = FMath::Clamp(CurrentSailAngleInput + Value, -1.0f, 1.0f);
}

void UShipMovementComponent::SetSailState(ESailState NewSailState)
{
    CurrentSailState = NewSailState;
}
