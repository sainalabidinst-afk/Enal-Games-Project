#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CalendarWorldSubsystem.h"
#include "TradeEconomySubsystem.generated.h"

UENUM(BlueprintType)
enum class ECommodityType : uint8
{
    Pala,
    Cengkeh,
    Lada,
    KapurBarus,
    Sutra,
    Candu
};

USTRUCT(BlueprintType)
struct FCommodityTradeData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
    ECommodityType CommodityType = ECommodityType::Lada;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade", meta = (ClampMin = "0.0"))
    float BasePrice = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade", meta = (ClampMin = "0.0"))
    float CurrentBuyPrice = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade", meta = (ClampMin = "0.0"))
    float CurrentSellPrice = 85.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade", meta = (ClampMin = "0"))
    int32 StockQuantity = 500;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade", meta = (ClampMin = "1"))
    int32 MaxStockCapacity = 1000;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade", meta = (ClampMin = "0.0"))
    float DemandMultiplier = 1.0f;
};

USTRUCT(BlueprintType)
struct FPortMarketData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
    FName PortID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
    FText PortName = FText::GetEmpty();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
    TMap<ECommodityType, FCommodityTradeData> MarketInventory;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMarketPricesUpdated, FName, PortID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCommodityTraded, FName, PortID, ECommodityType, Commodity, int32, Quantity, float, TotalGoldTransaction);

UCLASS()
class TIDES_OF_NUSANTARA_API UTradeEconomySubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Trade Economy")
    void RegisterPortMarket(const FPortMarketData& NewPortData);

    UFUNCTION(BlueprintCallable, Category = "Trade Economy")
    void LoadCommoditiesFromAssetManager();

    UFUNCTION(BlueprintPure, Category = "Trade Economy")
    bool GetPortMarketData(FName PortID, FPortMarketData& OutData) const;

    UFUNCTION(BlueprintCallable, Category = "Trade Economy")
    void RecalculateAllMarketPrices();

    UFUNCTION(BlueprintPure, Category = "Trade Economy")
    float GetEffectiveBuyPrice(FName PortID, ECommodityType Commodity) const;

    UFUNCTION(BlueprintPure, Category = "Trade Economy")
    float GetEffectiveSellPrice(FName PortID, ECommodityType Commodity) const;

    UFUNCTION(BlueprintCallable, Category = "Trade Economy")
    bool BuyCommodityFromPort(FName PortID, ECommodityType Commodity, int32 Quantity, UPARAM(ref) float& PlayerGold, int32& OutPurchasedQuantity);

    UFUNCTION(BlueprintCallable, Category = "Trade Economy")
    bool SellCommodityToPort(FName PortID, ECommodityType Commodity, int32 Quantity, UPARAM(ref) float& PlayerGold, int32& OutSoldQuantity);

    UPROPERTY(BlueprintAssignable, Category = "Trade Economy|Events")
    FOnMarketPricesUpdated OnMarketPricesUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Trade Economy|Events")
    FOnCommodityTraded OnCommodityTraded;

private:
    UPROPERTY()
    TMap<FName, FPortMarketData> PortRegistry;

    UPROPERTY()
    UCalendarWorldSubsystem* CachedCalendarSubsystem = nullptr;

    void PopulateDefaultPorts();

    UFUNCTION()
    void HandleDayPassed(int32 TotalDaysPassed, EPasaranJawa Pasaran, EMusimAngin Musim);
};
