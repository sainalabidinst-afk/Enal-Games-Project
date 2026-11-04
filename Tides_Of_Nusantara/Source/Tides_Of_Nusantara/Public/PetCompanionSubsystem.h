#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PetCompanionSubsystem.generated.h"

UENUM(BlueprintType)
enum class EPetSpecies : uint8
{
    HarimauSumatra,
    ElangLaut,
    BuayaMuara
};

UENUM(BlueprintType)
enum class EPetGrowthStage : uint8
{
    Cub,
    Juvenile,
    ApexAdult
};

UENUM(BlueprintType)
enum class EPetBehaviorMode : uint8
{
    Passive,
    Guard,
    Aggressive,
    UtilityHunt
};

USTRUCT(BlueprintType)
struct FPetCompanionData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pet")
    FName PetID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pet")
    FText PetName = FText::GetEmpty();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pet")
    EPetSpecies Species = EPetSpecies::HarimauSumatra;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pet")
    EPetGrowthStage GrowthStage = EPetGrowthStage::Cub;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pet")
    EPetBehaviorMode BehaviorMode = EPetBehaviorMode::Guard;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pet", meta = (ClampMin = "0.0"))
    float CurrentXP = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pet", meta = (ClampMin = "1.0"))
    float MaxXPForNextStage = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pet", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float LoyaltyScore = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pet")
    bool bIsActiveCompanion = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPetGrowthStageChanged, FName, PetID, EPetGrowthStage, NewStage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPetLoyaltyChanged, FName, PetID, float, NewLoyalty);

UCLASS()
class TIDES_OF_NUSANTARA_API UPetCompanionSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Pet Companion")
    void RegisterPet(const FPetCompanionData& NewPet);

    UFUNCTION(BlueprintCallable, Category = "Pet Companion")
    bool SetActivePet(FName PetID);

    UFUNCTION(BlueprintPure, Category = "Pet Companion")
    bool GetActivePet(FPetCompanionData& OutPet) const;

    UFUNCTION(BlueprintPure, Category = "Pet Companion")
    bool GetPetData(FName PetID, FPetCompanionData& OutPet) const;

    UFUNCTION(BlueprintPure, Category = "Pet Companion")
    TArray<FPetCompanionData> GetAllPets() const;

    UFUNCTION(BlueprintCallable, Category = "Pet Companion")
    void AddPetExperience(FName PetID, float XPAmount);

    UFUNCTION(BlueprintCallable, Category = "Pet Companion")
    void AdjustLoyalty(FName PetID, float DeltaLoyalty);

    UFUNCTION(BlueprintCallable, Category = "Pet Companion")
    bool SetBehaviorMode(FName PetID, EPetBehaviorMode NewMode);

    UFUNCTION(BlueprintPure, Category = "Pet Companion")
    float GetCompanionStatBonus(EPetSpecies Species, FName AttributeName) const;

    UPROPERTY(BlueprintAssignable, Category = "Pet Companion|Events")
    FOnPetGrowthStageChanged OnPetGrowthStageChanged;

    UPROPERTY(BlueprintAssignable, Category = "Pet Companion|Events")
    FOnPetLoyaltyChanged OnPetLoyaltyChanged;

private:
    UPROPERTY()
    TMap<FName, FPetCompanionData> PetRegistry;

    void PopulateDefaultPets();
    void EvaluateGrowthStage(FPetCompanionData& Pet);
};
