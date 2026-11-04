#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CrewOfficerSubsystem.generated.h"

UENUM(BlueprintType)
enum class EOfficerRole : uint8
{
    None,
    Nakhoda,
    JuruMudi,
    JuruMeriam,
    JuruLayar
};

USTRUCT(BlueprintType)
struct FOfficerData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crew Officer")
    FName OfficerID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crew Officer")
    FText OfficerName = FText::GetEmpty();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crew Officer")
    EOfficerRole PreferredRole = EOfficerRole::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crew Officer", meta = (ClampMin = "1", ClampMax = "100"))
    int32 LeadershipSkill = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crew Officer", meta = (ClampMin = "1", ClampMax = "100"))
    int32 NavigationSkill = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crew Officer", meta = (ClampMin = "1", ClampMax = "100"))
    int32 GunnerySkill = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crew Officer")
    FName AssignedShipID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crew Officer")
    EOfficerRole CurrentAssignedRole = EOfficerRole::None;
};

USTRUCT(BlueprintType)
struct FShipBonusStats
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Bonuses")
    float ThrustMultiplier = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Bonuses")
    float RudderTorqueMultiplier = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Bonuses")
    float CannonDamageMultiplier = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Bonuses")
    float CannonReloadTimeMultiplier = 1.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnOfficerAssigned, FName, OfficerID, FName, ShipID, EOfficerRole, Role);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOfficerUnassigned, FName, OfficerID, FName, ShipID);

UCLASS()
class TIDES_OF_NUSANTARA_API UCrewOfficerSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Crew Officer Management")
    void RegisterOfficer(const FOfficerData& NewOfficer);

    UFUNCTION(BlueprintCallable, Category = "Crew Officer Management")
    void LoadOfficersFromAssetManager();

    UFUNCTION(BlueprintCallable, Category = "Crew Officer Management")
    bool AssignOfficerToShip(FName OfficerID, FName ShipID, EOfficerRole Role);

    UFUNCTION(BlueprintCallable, Category = "Crew Officer Management")
    bool UnassignOfficer(FName OfficerID);

    UFUNCTION(BlueprintPure, Category = "Crew Officer Management")
    bool GetOfficerData(FName OfficerID, FOfficerData& OutOfficer) const;

    UFUNCTION(BlueprintPure, Category = "Crew Officer Management")
    TArray<FOfficerData> GetOfficersOnShip(FName ShipID) const;

    UFUNCTION(BlueprintPure, Category = "Crew Officer Management")
    TArray<FOfficerData> GetAllRegisteredOfficers() const;

    UFUNCTION(BlueprintPure, Category = "Crew Officer Management")
    FShipBonusStats CalculateShipBonusStats(FName ShipID) const;

    UPROPERTY(BlueprintAssignable, Category = "Crew Officer Management|Events")
    FOnOfficerAssigned OnOfficerAssigned;

    UPROPERTY(BlueprintAssignable, Category = "Crew Officer Management|Events")
    FOnOfficerUnassigned OnOfficerUnassigned;

private:
    UPROPERTY()
    TMap<FName, FOfficerData> OfficerRegistry;

    void PopulateDefaultOfficers();
};
