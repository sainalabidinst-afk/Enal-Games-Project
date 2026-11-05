#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TerritoryBuyoutSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FVillageBuyoutData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
    FName VillageID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
    FText VillageName = FText::GetEmpty();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
    float BasePrice = 10000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
    bool bIsUnlocked = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
    bool bIsPurchased = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Territory")
    bool bDebtQuestCleared = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVillagePurchased, FName, VillageID, int64, FinalPricePaid);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVillageUnlocked, FName, VillageID);

UCLASS()
class TIDES_OF_NUSANTARA_API UTerritoryBuyoutSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Territory Buyout")
    void RegisterVillage(const FVillageBuyoutData& NewVillageData);

    UFUNCTION(BlueprintPure, Category = "Territory Buyout")
    bool GetVillageData(FName VillageID, FVillageBuyoutData& OutData) const;

    UFUNCTION(BlueprintPure, Category = "Territory Buyout")
    TArray<FVillageBuyoutData> GetAllVillages() const;

    UFUNCTION(BlueprintPure, Category = "Territory Buyout")
    float CalculateEffectivePrice(FName VillageID, float OfficerDiscountPercent) const;

    UFUNCTION(BlueprintPure, Category = "Territory Buyout")
    bool CanPurchaseVillage(FName VillageID, int64 PlayerRupiah, float OfficerDiscountPercent, FText& OutReason) const;

    UFUNCTION(BlueprintCallable, Category = "Territory Buyout")
    bool PurchaseVillage(FName VillageID, UPARAM(ref) int64& PlayerRupiah, float OfficerDiscountPercent);

    UFUNCTION(BlueprintCallable, Category = "Territory Buyout")
    bool SetDebtQuestStatus(FName VillageID, bool bCleared);

    UFUNCTION(BlueprintCallable, Category = "Territory Buyout")
    bool UnlockVillage(FName VillageID);

    UFUNCTION(BlueprintPure, Category = "Territory Buyout")
    TMap<FName, bool> GetPurchasedVillagesMap() const;

    UFUNCTION(BlueprintCallable, Category = "Territory Buyout")
    void LoadPurchasedState(const TMap<FName, bool>& SavedPurchasedMap);

    UPROPERTY(BlueprintAssignable, Category = "Territory Buyout|Events")
    FOnVillagePurchased OnVillagePurchased;

    UPROPERTY(BlueprintAssignable, Category = "Territory Buyout|Events")
    FOnVillageUnlocked OnVillageUnlocked;

private:
    UPROPERTY()
    TMap<FName, FVillageBuyoutData> VillageRegistry;

    void PopulateDefaultVillages();
};
