#include "TradeEconomySubsystem.h"

#include "Engine/AssetManager.h"
#include "LanunCommodityDataAsset.h"

void UTradeEconomySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LoadCommoditiesFromAssetManager();

    if (PortRegistry.Num() == 0)
    {
        PopulateDefaultPorts();
    }

    UWorld* World = GetWorld();
    if (World)
    {
        CachedCalendarSubsystem = World->GetSubsystem<UCalendarWorldSubsystem>();
        if (CachedCalendarSubsystem)
        {
            CachedCalendarSubsystem->OnDayChanged.AddDynamic(this, &UTradeEconomySubsystem::HandleDayPassed);
        }
    }
}

void UTradeEconomySubsystem::PopulateDefaultPorts()
{
    FPortMarketData BandarMelaka;
    BandarMelaka.PortID = FName(TEXT("PORT_Melaka"));
    BandarMelaka.PortName = NSLOCTEXT("Trade", "Melaka", "Bandar Utama Melaka");

    FCommodityTradeData LadaMelaka;
    LadaMelaka.CommodityType = ECommodityType::Lada;
    LadaMelaka.BasePrice = 120.0f;
    LadaMelaka.StockQuantity = 800;
    LadaMelaka.DemandMultiplier = 1.2f;
    BandarMelaka.MarketInventory.Add(ECommodityType::Lada, LadaMelaka);

    FCommodityTradeData PalaMelaka;
    PalaMelaka.CommodityType = ECommodityType::Pala;
    PalaMelaka.BasePrice = 350.0f;
    PalaMelaka.StockQuantity = 200;
    PalaMelaka.DemandMultiplier = 2.0f;
    BandarMelaka.MarketInventory.Add(ECommodityType::Pala, PalaMelaka);
    RegisterPortMarket(BandarMelaka);

    FPortMarketData BandaNeira;
    BandaNeira.PortID = FName(TEXT("PORT_Banda"));
    BandaNeira.PortName = NSLOCTEXT("Trade", "Banda", "Kepulauan Banda Neira");

    FCommodityTradeData PalaBanda;
    PalaBanda.CommodityType = ECommodityType::Pala;
    PalaBanda.BasePrice = 80.0f;
    PalaBanda.StockQuantity = 2500;
    PalaBanda.DemandMultiplier = 0.4f;
    BandaNeira.MarketInventory.Add(ECommodityType::Pala, PalaBanda);
    RegisterPortMarket(BandaNeira);

    RecalculateAllMarketPrices();
}

void UTradeEconomySubsystem::RegisterPortMarket(const FPortMarketData& NewPortData)
{
    if (!NewPortData.PortID.IsNone())
    {
        PortRegistry.Add(NewPortData.PortID, NewPortData);
    }
}

bool UTradeEconomySubsystem::GetPortMarketData(FName PortID, FPortMarketData& OutData) const
{
    const FPortMarketData* Found = PortRegistry.Find(PortID);
    if (!Found)
    {
        return false;
    }

    OutData = *Found;
    return true;
}

void UTradeEconomySubsystem::RecalculateAllMarketPrices()
{
    EMusimAngin CurrentSeason = EMusimAngin::Pancaroba;
    if (CachedCalendarSubsystem)
    {
        CurrentSeason = CachedCalendarSubsystem->GetMusimAngin();
    }

    for (TPair<FName, FPortMarketData>& PortPair : PortRegistry)
    {
        FPortMarketData& Port = PortPair.Value;
        for (TPair<ECommodityType, FCommodityTradeData>& ItemPair : Port.MarketInventory)
        {
            FCommodityTradeData& Item = ItemPair.Value;
            const float SafeCapacity = FMath::Max(static_cast<float>(Item.MaxStockCapacity), 1.0f);
            const float StockRatio = FMath::Clamp(static_cast<float>(Item.StockQuantity) / SafeCapacity, 0.05f, 1.0f);
            const float ScarcityFactor = 1.0f + (1.0f - StockRatio);

            float SeasonFactor = 1.0f;
            if (CurrentSeason == EMusimAngin::MusonBarat && Port.PortID == FName(TEXT("PORT_Melaka")))
            {
                SeasonFactor = 1.35f;
            }
            else if (CurrentSeason == EMusimAngin::MusonTimur && Port.PortID == FName(TEXT("PORT_Banda")))
            {
                SeasonFactor = 0.85f;
            }

            Item.CurrentBuyPrice = Item.BasePrice * Item.DemandMultiplier * ScarcityFactor * SeasonFactor;
            Item.CurrentSellPrice = Item.CurrentBuyPrice * 0.82f;
        }

        OnMarketPricesUpdated.Broadcast(PortPair.Key);
    }
}

float UTradeEconomySubsystem::GetEffectiveBuyPrice(FName PortID, ECommodityType Commodity) const
{
    const FPortMarketData* Port = PortRegistry.Find(PortID);
    const FCommodityTradeData* Item = Port ? Port->MarketInventory.Find(Commodity) : nullptr;
    return Item ? Item->CurrentBuyPrice : 0.0f;
}

float UTradeEconomySubsystem::GetEffectiveSellPrice(FName PortID, ECommodityType Commodity) const
{
    const FPortMarketData* Port = PortRegistry.Find(PortID);
    const FCommodityTradeData* Item = Port ? Port->MarketInventory.Find(Commodity) : nullptr;
    return Item ? Item->CurrentSellPrice : 0.0f;
}

bool UTradeEconomySubsystem::BuyCommodityFromPort(FName PortID, ECommodityType Commodity, int32 Quantity, float& PlayerGold, int32& OutPurchasedQuantity)
{
    OutPurchasedQuantity = 0;
    if (Quantity <= 0 || PlayerGold <= 0.0f)
    {
        return false;
    }

    FPortMarketData* Port = PortRegistry.Find(PortID);
    FCommodityTradeData* Item = Port ? Port->MarketInventory.Find(Commodity) : nullptr;
    if (!Item || Item->StockQuantity <= 0 || Item->CurrentBuyPrice <= 0.0f)
    {
        return false;
    }

    int32 ActualQuantity = FMath::Min(Quantity, Item->StockQuantity);
    ActualQuantity = FMath::Min(ActualQuantity, FMath::FloorToInt(PlayerGold / Item->CurrentBuyPrice));
    if (ActualQuantity <= 0)
    {
        return false;
    }

    const float TotalCost = Item->CurrentBuyPrice * ActualQuantity;
    PlayerGold -= TotalCost;
    Item->StockQuantity -= ActualQuantity;
    OutPurchasedQuantity = ActualQuantity;

    RecalculateAllMarketPrices();
    OnCommodityTraded.Broadcast(PortID, Commodity, ActualQuantity, TotalCost);
    return true;
}

bool UTradeEconomySubsystem::SellCommodityToPort(FName PortID, ECommodityType Commodity, int32 Quantity, float& PlayerGold, int32& OutSoldQuantity)
{
    OutSoldQuantity = 0;
    if (Quantity <= 0)
    {
        return false;
    }

    FPortMarketData* Port = PortRegistry.Find(PortID);
    FCommodityTradeData* Item = Port ? Port->MarketInventory.Find(Commodity) : nullptr;
    if (!Item)
    {
        return false;
    }

    const int32 AvailableCapacity = FMath::Max(Item->MaxStockCapacity - Item->StockQuantity, 0);
    const int32 ActualQuantity = FMath::Min(Quantity, AvailableCapacity);
    if (ActualQuantity <= 0)
    {
        return false;
    }

    const float TotalPayout = Item->CurrentSellPrice * ActualQuantity;
    PlayerGold += TotalPayout;
    Item->StockQuantity += ActualQuantity;
    OutSoldQuantity = ActualQuantity;

    RecalculateAllMarketPrices();
    OnCommodityTraded.Broadcast(PortID, Commodity, ActualQuantity, TotalPayout);
    return true;
}

void UTradeEconomySubsystem::HandleDayPassed(int32 TotalDaysPassed, EPasaranJawa Pasaran, EMusimAngin Musim)
{
    for (TPair<FName, FPortMarketData>& PortPair : PortRegistry)
    {
        for (TPair<ECommodityType, FCommodityTradeData>& ItemPair : PortPair.Value.MarketInventory)
        {
            FCommodityTradeData& Item = ItemPair.Value;
            const int32 DailyProduction = FMath::RandRange(10, 30);
            Item.StockQuantity = FMath::Clamp(Item.StockQuantity + DailyProduction, 0, Item.MaxStockCapacity);
        }
    }

    RecalculateAllMarketPrices();
}

void UTradeEconomySubsystem::LoadCommoditiesFromAssetManager()
{
    UAssetManager& AssetManager = UAssetManager::Get();
    TArray<FAssetData> AssetDataList;
    AssetManager.GetPrimaryAssetDataList(FPrimaryAssetType("CommodityData"), AssetDataList);

    TMap<FName, FPortMarketData> PortMarketMap;

    for (const FAssetData& AssetData : AssetDataList)
    {
        if (ULanunCommodityDataAsset* CommodityAsset = Cast<ULanunCommodityDataAsset>(AssetData.GetAsset()))
        {
            FCommodityTradeData TradeData;
            TradeData.CommodityType = CommodityAsset->CommodityType;
            TradeData.BasePrice = CommodityAsset->BasePricePerTon;
            TradeData.CurrentBuyPrice = CommodityAsset->BasePricePerTon;
            TradeData.CurrentSellPrice = CommodityAsset->BasePricePerTon * 0.82f;
            TradeData.StockQuantity = 500;
            TradeData.MaxStockCapacity = 1000;
            TradeData.DemandMultiplier = 1.0f + CommodityAsset->PriceVolatilityRate;

            FName PortID = CommodityAsset->PrimaryOriginPortID;
            if (PortID.IsNone())
            {
                PortID = FName("PORT_Default");
            }

            FPortMarketData* PortData = PortMarketMap.Find(PortID);
            if (!PortData)
            {
                FPortMarketData NewPort;
                NewPort.PortID = PortID;
                NewPort.PortName = FText::FromName(PortID);
                PortData = &PortMarketMap.Add(PortID, NewPort);
            }

            PortData->MarketInventory.Add(CommodityAsset->CommodityType, TradeData);
        }
    }

    for (const TPair<FName, FPortMarketData>& Pair : PortMarketMap)
    {
        RegisterPortMarket(Pair.Value);
    }

    RecalculateAllMarketPrices();
}
