#include "NavalCombatSubsystem.h"

#include "CrewOfficerSubsystem.h"
#include "GameFramework/Actor.h"
#include "ShipFleetSubsystem.h"
#include "ShipPawnBase.h"

void UNavalCombatSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    PopulateDefaultCannonsForShip(FName(TEXT("SHIP_Phinisi_Flagship")));
}

void UNavalCombatSubsystem::PopulateDefaultCannonsForShip(FName ShipID)
{
    FCannonGroupData StarboardLela;
    StarboardLela.CannonType = ECannonType::Lela;
    StarboardLela.Side = ECannonBroadsideSide::Starboard;
    StarboardLela.CannonCount = 6;
    StarboardLela.BaseDamagePerCannon = 130.0f;
    StarboardLela.BaseReloadTime = 5.5f;
    RegisterCannonGroup(ShipID, StarboardLela);

    FCannonGroupData PortLela = StarboardLela;
    PortLela.Side = ECannonBroadsideSide::Port;
    RegisterCannonGroup(ShipID, PortLela);

    FCannonGroupData BowLantaka;
    BowLantaka.CannonType = ECannonType::Lantaka;
    BowLantaka.Side = ECannonBroadsideSide::Bow;
    BowLantaka.CannonCount = 2;
    BowLantaka.BaseDamagePerCannon = 60.0f;
    BowLantaka.BaseReloadTime = 2.5f;
    RegisterCannonGroup(ShipID, BowLantaka);
}

void UNavalCombatSubsystem::RegisterCannonGroup(FName ShipID, const FCannonGroupData& NewCannonGroup)
{
    if (ShipID.IsNone())
    {
        return;
    }

    TArray<FCannonGroupData>& CannonList = ShipCannonRegistry.FindOrAdd(ShipID).CannonGroups;
    for (FCannonGroupData& ExistingGroup : CannonList)
    {
        if (ExistingGroup.Side == NewCannonGroup.Side)
        {
            ExistingGroup = NewCannonGroup;
            return;
        }
    }

    CannonList.Add(NewCannonGroup);
}

bool UNavalCombatSubsystem::GetCannonGroup(FName ShipID, ECannonBroadsideSide Side, FCannonGroupData& OutGroup) const
{
   const FShipCannonRegistryData* RegistryData = ShipCannonRegistry.Find(ShipID);
    if (!RegistryData)
    {
        return false;
    }

   for (const FCannonGroupData& Group : RegistryData->CannonGroups)
    {
        if (Group.Side == Side)
        {
            OutGroup = Group;
            return true;
        }
    }

    return false;
}

bool UNavalCombatSubsystem::FireBroadside(FName ShipID, ECannonBroadsideSide Side, AActor* FiringActor, FVector TargetLocation)
{
    if (ShipID.IsNone() || !FiringActor)
    {
        return false;
    }

    FShipCannonRegistryData* RegistryData = ShipCannonRegistry.Find(ShipID);
    if (!RegistryData)
    {
        return false;
    }

    FCannonGroupData* TargetGroup = nullptr;
    for (FCannonGroupData& Group : RegistryData->CannonGroups)
    {
        if (Group.Side == Side)
        {
            TargetGroup = &Group;
            break;
        }
    }

    if (!TargetGroup || TargetGroup->CurrentCooldown > 0.0f || TargetGroup->CannonCount <= 0)
    {
        return false;
    }

    float ReloadMultiplier = 1.0f;
    UWorld* World = GetWorld();
    if (World)
    {
        UCrewOfficerSubsystem* OfficerSubsystem = World->GetSubsystem<UCrewOfficerSubsystem>();
        if (OfficerSubsystem)
        {
            const FShipBonusStats Bonuses = OfficerSubsystem->CalculateShipBonusStats(ShipID);
            ReloadMultiplier = Bonuses.CannonReloadTimeMultiplier;
        }
    }

    TargetGroup->CurrentCooldown = FMath::Max(0.0f, TargetGroup->BaseReloadTime * ReloadMultiplier);

    // Eksekusi LineTrace Hit & Kalkulasi Damage
    const float DamageMultiplier = 1.0f;
    if (World && FiringActor)
    {
        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(FiringActor);

        if (World->LineTraceSingleByChannel(HitResult, FiringActor->GetActorLocation(), TargetLocation, ECC_Pawn, QueryParams))
        {
            AActor* TargetActor = HitResult.GetActor();
            float Distance = FVector::Distance(FiringActor->GetActorLocation(), TargetActor->GetActorLocation());

            FNavalImpactResult Impact = CalculateNavalDamage(TargetGroup->CannonType, TargetGroup->CannonCount, Distance, 40.0f, DamageMultiplier);

            // Terapkan damage ke UShipFleetSubsystem
            if (UShipFleetSubsystem* FleetSubsystem = World->GetSubsystem<UShipFleetSubsystem>())
            {
                FName TargetShipID = TargetActor->GetFName();
                FShipFleetData TargetShipData;
                if (FleetSubsystem->GetShipData(TargetShipID, TargetShipData))
                {
                    TargetShipData.HullHealth = FMath::Max(0.0f, TargetShipData.HullHealth - Impact.HullDamage);
                    FleetSubsystem->RegisterShip(TargetShipData); // Update state

                    OnShipStructuralDamageTaken.Broadcast(TargetShipID, Impact.HullDamage, Impact);
                }
            }
        }
    }

    OnBroadsideFired.Broadcast(ShipID, Side, TargetGroup->CannonType, TargetGroup->CannonCount);
    return true;
}

void UNavalCombatSubsystem::UpdateCannonCooldowns(float DeltaTime)
{
    if (DeltaTime <= 0.0f)
    {
        return;
    }

 for (TPair<FName, FShipCannonRegistryData>& ShipPair : ShipCannonRegistry)
    {
      for (FCannonGroupData& Group : ShipPair.Value.CannonGroups)
        {
            Group.CurrentCooldown = FMath::Max(0.0f, Group.CurrentCooldown - DeltaTime);
        }
    }
}

bool UNavalCombatSubsystem::IsTargetInBroadsideArc(const AActor* FiringShip, const AActor* TargetShip, ECannonBroadsideSide Side, float ArcAngleDegrees) const
{
    if (!FiringShip || !TargetShip || ArcAngleDegrees <= 0.0f)
    {
        return false;
    }

    const FVector DirectionToTarget = (TargetShip->GetActorLocation() - FiringShip->GetActorLocation()).GetSafeNormal2D();
    if (DirectionToTarget.IsNearlyZero())
    {
        return true;
    }

    FVector FireVector = FiringShip->GetActorForwardVector();
    switch (Side)
    {
    case ECannonBroadsideSide::Port:
        FireVector = -FiringShip->GetActorRightVector();
        break;
    case ECannonBroadsideSide::Starboard:
        FireVector = FiringShip->GetActorRightVector();
        break;
    case ECannonBroadsideSide::Bow:
        break;
    case ECannonBroadsideSide::Stern:
        FireVector = -FiringShip->GetActorForwardVector();
        break;
    }

    const float DotProduct = FMath::Clamp(
        FVector::DotProduct(FireVector.GetSafeNormal2D(), DirectionToTarget),
        -1.0f,
        1.0f);
    const float AngleToTarget = FMath::RadiansToDegrees(FMath::Acos(DotProduct));
    return AngleToTarget <= ArcAngleDegrees * 0.5f;
}

FNavalImpactResult UNavalCombatSubsystem::CalculateNavalDamage(ECannonType CannonType, int32 CannonCount, float Distance, float TargetArmor, float CannonDamageMultiplier) const
{
    FNavalImpactResult Impact;
    if (CannonCount <= 0)
    {
        return Impact;
    }

    float MaxEffectiveDistance = 3500.0f;
    if (CannonType == ECannonType::Lantaka)
    {
        MaxEffectiveDistance = 1500.0f;
    }
    else if (CannonType == ECannonType::MeriamCetak)
    {
        MaxEffectiveDistance = 5000.0f;
    }

    const float SafeDistance = FMath::Max(Distance, 0.0f);
    const float DistanceRatio = FMath::Clamp(SafeDistance / MaxEffectiveDistance, 0.0f, 1.5f);
    const float DistanceFactor = FMath::Clamp(1.0f - DistanceRatio * 0.4f, 0.1f, 1.0f);

    float BaseSingleDamage = 130.0f;
    if (CannonType == ECannonType::Lantaka)
    {
        BaseSingleDamage = 50.0f;
    }
    else if (CannonType == ECannonType::MeriamCetak)
    {
        BaseSingleDamage = 260.0f;
    }

    const float SafeDamageMultiplier = FMath::Max(CannonDamageMultiplier, 0.0f);
    const float RawTotalDamage = BaseSingleDamage * CannonCount * SafeDamageMultiplier * DistanceFactor;
    const float ArmorMitigation = FMath::Clamp(1.0f - FMath::Max(TargetArmor, 0.0f) * 0.003f, 0.2f, 1.0f);

    Impact.HullDamage = RawTotalDamage * ArmorMitigation;
    Impact.bArmorPenetrated = TargetArmor < BaseSingleDamage * 0.8f;

    if (CannonType == ECannonType::Lantaka)
    {
        Impact.SailDamage = RawTotalDamage * 1.5f;
        Impact.CrewCasualties = FMath::RandRange(2, 5) * CannonCount;
    }
    else if (CannonType == ECannonType::Lela)
    {
        Impact.SailDamage = RawTotalDamage * 0.8f;
        Impact.CrewCasualties = FMath::RandRange(1, 3) * CannonCount;
    }
    else
    {
        Impact.SailDamage = RawTotalDamage * 0.4f;
        Impact.CrewCasualties = FMath::RandRange(1, 2) * CannonCount;
    }

    return Impact;
}

bool UNavalCombatSubsystem::InitiateBoarding(FName BoardingShipID, FName TargetShipID, AShipPawnBase* BoardingShip, AShipPawnBase* TargetShip)
{
    if (!BoardingShip || !TargetShip || BoardingShipID.IsNone() || TargetShipID.IsNone())
    {
        return false;
    }

    if (BoardingShip->GetBoardState() != EBordState::None)
    {
        return false;
    }

    return BoardingShip->GrappleShip(TargetShip);
}

bool UNavalCombatSubsystem::CompleteBoarding(FName BoardingShipID, FName TargetShipID)
{
    if (BoardingShipID.IsNone() || TargetShipID.IsNone())
    {
        return false;
    }

    OnShipBoarded.Broadcast(BoardingShipID, TargetShipID, true);
    return true;
}
