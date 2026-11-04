#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "NavalCombatSubsystem.generated.h"

class AActor;

UENUM(BlueprintType)
enum class ECannonType : uint8
{
    Lantaka,
    Lela,
    MeriamCetak
};

UENUM(BlueprintType)
enum class ECannonBroadsideSide : uint8
{
    Port,
    Starboard,
    Bow,
    Stern
};

USTRUCT(BlueprintType)
struct FCannonGroupData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Naval Combat")
    ECannonType CannonType = ECannonType::Lela;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Naval Combat")
    ECannonBroadsideSide Side = ECannonBroadsideSide::Starboard;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Naval Combat", meta = (ClampMin = "0"))
    int32 CannonCount = 4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Naval Combat", meta = (ClampMin = "0.0"))
    float BaseDamagePerCannon = 120.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Naval Combat", meta = (ClampMin = "0.0"))
    float BaseReloadTime = 6.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Naval Combat", meta = (ClampMin = "0.0"))
    float CurrentCooldown = 0.0f;
};

USTRUCT(BlueprintType)
struct FNavalImpactResult
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Naval Impact")
    float HullDamage = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Naval Impact")
    float SailDamage = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Naval Impact")
    int32 CrewCasualties = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Naval Impact")
    bool bArmorPenetrated = false;
};

USTRUCT()
struct FShipCannonRegistryData
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FCannonGroupData> CannonGroups;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnBroadsideFired, FName, ShipID, ECannonBroadsideSide, Side, ECannonType, CannonType, int32, CannonsFired);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnShipStructuralDamageTaken, FName, TargetShipID, float, TotalHullDamageTaken, const FNavalImpactResult&, ImpactResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnShipBoarded, FName, BoardingShipID, FName, TargetShipID, bool, bSuccess);

UCLASS()
class TIDES_OF_NUSANTARA_API UNavalCombatSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Naval Combat")
    bool FireBroadside(FName ShipID, ECannonBroadsideSide Side, AActor* FiringActor, FVector TargetLocation);

    UFUNCTION(BlueprintCallable, Category = "Naval Combat")
    void UpdateCannonCooldowns(float DeltaTime);

    UFUNCTION(BlueprintPure, Category = "Naval Combat")
    bool IsTargetInBroadsideArc(const AActor* FiringShip, const AActor* TargetShip, ECannonBroadsideSide Side, float ArcAngleDegrees = 60.0f) const;

    UFUNCTION(BlueprintPure, Category = "Naval Combat")
    FNavalImpactResult CalculateNavalDamage(ECannonType CannonType, int32 CannonCount, float Distance, float TargetArmor, float CannonDamageMultiplier = 1.0f) const;

    UFUNCTION(BlueprintCallable, Category = "Naval Combat")
    void RegisterCannonGroup(FName ShipID, const FCannonGroupData& NewCannonGroup);

    UFUNCTION(BlueprintPure, Category = "Naval Combat")
    bool GetCannonGroup(FName ShipID, ECannonBroadsideSide Side, FCannonGroupData& OutGroup) const;

    UFUNCTION(BlueprintCallable, Category = "Boarding")
    bool InitiateBoarding(FName BoardingShipID, FName TargetShipID, AShipPawnBase* BoardingShip, AShipPawnBase* TargetShip);

    UFUNCTION(BlueprintCallable, Category = "Boarding")
    bool CompleteBoarding(FName BoardingShipID, FName TargetShipID);

    UPROPERTY(BlueprintAssignable, Category = "Naval Combat|Events")
    FOnBroadsideFired OnBroadsideFired;

    UPROPERTY(BlueprintAssignable, Category = "Naval Combat|Events")
    FOnShipStructuralDamageTaken OnShipStructuralDamageTaken;

    UPROPERTY(BlueprintAssignable, Category = "Boarding|Events")
    FOnShipBoarded OnShipBoarded;

private:
    UPROPERTY()
    TMap<FName, FShipCannonRegistryData> ShipCannonRegistry;

    void PopulateDefaultCannonsForShip(FName ShipID);
};
