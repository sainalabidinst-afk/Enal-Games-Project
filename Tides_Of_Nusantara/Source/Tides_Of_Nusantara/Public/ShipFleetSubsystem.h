#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ShipFleetSubsystem.generated.h"

UENUM(BlueprintType)
enum class EShipClassType : uint8
{
    Pencalang,
    Lancang,
    Phinisi,
    JungNusantara
};

USTRUCT(BlueprintType)
struct FShipFleetData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fleet")
    FName ShipID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fleet")
    FText ShipName = FText::GetEmpty();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fleet")
    EShipClassType ShipClass = EShipClassType::Phinisi;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fleet", meta = (ClampMin = "0.0"))
    float HullHealth = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fleet", meta = (ClampMin = "0.0"))
    float MaxHullHealth = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fleet", meta = (ClampMin = "0.0"))
    float CargoCapacityTon = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fleet")
    bool bIsFlagship = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fleet")
    FName AssignedCaptainID = NAME_None;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFlagshipChanged, FName, NewFlagshipID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShipRepaired, FName, ShipID, float, NewHealth);

UCLASS()
class TIDES_OF_NUSANTARA_API UShipFleetSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Fleet Management")
    void RegisterShip(const FShipFleetData& NewShip);

    UFUNCTION(BlueprintCallable, Category = "Fleet Management")
    bool SetFlagship(FName ShipID);

    UFUNCTION(BlueprintPure, Category = "Fleet Management")
    bool GetFlagship(FShipFleetData& OutFlagship) const;

    UFUNCTION(BlueprintPure, Category = "Fleet Management")
    bool GetShipData(FName ShipID, FShipFleetData& OutShip) const;

    UFUNCTION(BlueprintPure, Category = "Fleet Management")
    TArray<FShipFleetData> GetAllShipsInFleet() const;

    UFUNCTION(BlueprintCallable, Category = "Fleet Management")
    bool RepairShipHull(FName ShipID, float RepairPoints, UPARAM(ref) float& PlayerGold, float GoldCostPerPoint = 2.0f);

    UPROPERTY(BlueprintAssignable, Category = "Fleet Management|Events")
    FOnFlagshipChanged OnFlagshipChanged;

    UPROPERTY(BlueprintAssignable, Category = "Fleet Management|Events")
    FOnShipRepaired OnShipRepaired;

private:
    UPROPERTY()
    TMap<FName, FShipFleetData> FleetRegistry;

    void PopulateInitialFleet();
};
