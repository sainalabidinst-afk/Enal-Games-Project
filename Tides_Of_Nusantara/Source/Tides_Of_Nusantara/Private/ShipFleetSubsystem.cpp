#include "ShipFleetSubsystem.h"

void UShipFleetSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    PopulateInitialFleet();
}

void UShipFleetSubsystem::PopulateInitialFleet()
{
    FShipFleetData Flagship;
    Flagship.ShipID = FName(TEXT("SHIP_Phinisi_Flagship"));
    Flagship.ShipName = NSLOCTEXT("Fleet", "FlagshipName", "Naga Jaladri");
    Flagship.ShipClass = EShipClassType::Phinisi;
    Flagship.HullHealth = 1200.0f;
    Flagship.MaxHullHealth = 1500.0f;
    Flagship.CargoCapacityTon = 800.0f;
    Flagship.bIsFlagship = true;
    RegisterShip(Flagship);

    FShipFleetData ScoutShip;
    ScoutShip.ShipID = FName(TEXT("SHIP_Pencalang_Scout"));
    ScoutShip.ShipName = NSLOCTEXT("Fleet", "ScoutName", "Angin Barat");
    ScoutShip.ShipClass = EShipClassType::Pencalang;
    ScoutShip.HullHealth = 600.0f;
    ScoutShip.MaxHullHealth = 600.0f;
    ScoutShip.CargoCapacityTon = 250.0f;
    RegisterShip(ScoutShip);
}

void UShipFleetSubsystem::RegisterShip(const FShipFleetData& NewShip)
{
    if (!NewShip.ShipID.IsNone())
    {
        FleetRegistry.Add(NewShip.ShipID, NewShip);
    }
}

bool UShipFleetSubsystem::SetFlagship(FName ShipID)
{
    if (!FleetRegistry.Contains(ShipID))
    {
        return false;
    }

    for (TPair<FName, FShipFleetData>& ShipPair : FleetRegistry)
    {
        ShipPair.Value.bIsFlagship = ShipPair.Key == ShipID;
    }

    OnFlagshipChanged.Broadcast(ShipID);
    return true;
}

bool UShipFleetSubsystem::GetFlagship(FShipFleetData& OutFlagship) const
{
    for (const TPair<FName, FShipFleetData>& ShipPair : FleetRegistry)
    {
        if (ShipPair.Value.bIsFlagship)
        {
            OutFlagship = ShipPair.Value;
            return true;
        }
    }

    return false;
}

bool UShipFleetSubsystem::GetShipData(FName ShipID, FShipFleetData& OutShip) const
{
    const FShipFleetData* Found = FleetRegistry.Find(ShipID);
    if (!Found)
    {
        return false;
    }

    OutShip = *Found;
    return true;
}

TArray<FShipFleetData> UShipFleetSubsystem::GetAllShipsInFleet() const
{
    TArray<FShipFleetData> Result;
    FleetRegistry.GenerateValueArray(Result);
    return Result;
}

bool UShipFleetSubsystem::RepairShipHull(FName ShipID, float RepairPoints, float& PlayerGold, float GoldCostPerPoint)
{
    FShipFleetData* Ship = FleetRegistry.Find(ShipID);
    if (!Ship || RepairPoints <= 0.0f || GoldCostPerPoint <= 0.0f || Ship->HullHealth >= Ship->MaxHullHealth)
    {
        return false;
    }

    const float NeededHealth = Ship->MaxHullHealth - Ship->HullHealth;
    float ActualRepair = FMath::Min(RepairPoints, NeededHealth);
    float TotalCost = ActualRepair * GoldCostPerPoint;

    if (PlayerGold < TotalCost)
    {
        ActualRepair = PlayerGold / GoldCostPerPoint;
        if (ActualRepair <= 0.0f)
        {
            return false;
        }

        TotalCost = ActualRepair * GoldCostPerPoint;
    }

    PlayerGold -= TotalCost;
    Ship->HullHealth = FMath::Clamp(Ship->HullHealth + ActualRepair, 0.0f, Ship->MaxHullHealth);
    OnShipRepaired.Broadcast(ShipID, Ship->HullHealth);
    return true;
}
