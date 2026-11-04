#include "TerritoryBuyoutSubsystem.h"

void UTerritoryBuyoutSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    PopulateDefaultVillages();
}

void UTerritoryBuyoutSubsystem::PopulateDefaultVillages()
{
    FVillageBuyoutData KampongKuala;
    KampongKuala.VillageID = FName(TEXT("VIL_KualaMelaka"));
    KampongKuala.VillageName = NSLOCTEXT("Territory", "KualaMelaka", "Kampung Kuala Melaka");
    KampongKuala.BasePrice = 15000.0f;
    KampongKuala.bIsUnlocked = true;
    KampongKuala.bDebtQuestCleared = true;
    RegisterVillage(KampongKuala);

    FVillageBuyoutData KampongTeluk;
    KampongTeluk.VillageID = FName(TEXT("VIL_TelukDuyung"));
    KampongTeluk.VillageName = NSLOCTEXT("Territory", "TelukDuyung", "Kampung Teluk Duyung");
    KampongTeluk.BasePrice = 32000.0f;
    RegisterVillage(KampongTeluk);

    FVillageBuyoutData KampongBatuPahat;
    KampongBatuPahat.VillageID = FName(TEXT("VIL_BatuPahat"));
    KampongBatuPahat.VillageName = NSLOCTEXT("Territory", "BatuPahat", "Pesisir Batu Pahat");
    KampongBatuPahat.BasePrice = 50000.0f;
    RegisterVillage(KampongBatuPahat);
}

void UTerritoryBuyoutSubsystem::RegisterVillage(const FVillageBuyoutData& NewVillageData)
{
    if (!NewVillageData.VillageID.IsNone())
    {
        VillageRegistry.Add(NewVillageData.VillageID, NewVillageData);
    }
}

bool UTerritoryBuyoutSubsystem::GetVillageData(FName VillageID, FVillageBuyoutData& OutData) const
{
    const FVillageBuyoutData* Found = VillageRegistry.Find(VillageID);
    if (!Found)
    {
        return false;
    }

    OutData = *Found;
    return true;
}

TArray<FVillageBuyoutData> UTerritoryBuyoutSubsystem::GetAllVillages() const
{
    TArray<FVillageBuyoutData> Result;
    VillageRegistry.GenerateValueArray(Result);
    return Result;
}

float UTerritoryBuyoutSubsystem::CalculateEffectivePrice(FName VillageID, float OfficerDiscountPercent) const
{
    const FVillageBuyoutData* Found = VillageRegistry.Find(VillageID);
    if (!Found)
    {
        return 0.0f;
    }

    const float ClampedDiscount = FMath::Clamp(OfficerDiscountPercent, 0.0f, 50.0f);
    return Found->BasePrice * (1.0f - ClampedDiscount / 100.0f);
}

bool UTerritoryBuyoutSubsystem::CanPurchaseVillage(FName VillageID, float PlayerGold, float OfficerDiscountPercent, FText& OutReason) const
{
    const FVillageBuyoutData* Found = VillageRegistry.Find(VillageID);
    if (!Found)
    {
        OutReason = NSLOCTEXT("TerritoryErr", "NotFound", "Wilayah tidak terdaftar.");
        return false;
    }

    if (Found->bIsPurchased)
    {
        OutReason = NSLOCTEXT("TerritoryErr", "AlreadyPurchased", "Wilayah ini sudah Anda kuasai.");
        return false;
    }

    if (!Found->bIsUnlocked)
    {
        OutReason = NSLOCTEXT("TerritoryErr", "Locked", "Wilayah belum terbuka di peta navigasi.");
        return false;
    }

    if (!Found->bDebtQuestCleared)
    {
        OutReason = NSLOCTEXT("TerritoryErr", "DebtPending", "Selesaikan quest pembebasan utang tetua kampung terlebih dahulu.");
        return false;
    }

    const float EffectivePrice = CalculateEffectivePrice(VillageID, OfficerDiscountPercent);
    if (PlayerGold < EffectivePrice)
    {
        OutReason = NSLOCTEXT("TerritoryErr", "InsufficientGold", "Emas / Koin Real tidak mencukupi untuk transaksi ini.");
        return false;
    }

    OutReason = FText::GetEmpty();
    return true;
}

bool UTerritoryBuyoutSubsystem::PurchaseVillage(FName VillageID, float& PlayerGold, float OfficerDiscountPercent)
{
    FText FailReason;
    if (!CanPurchaseVillage(VillageID, PlayerGold, OfficerDiscountPercent, FailReason))
    {
        return false;
    }

    FVillageBuyoutData* Found = VillageRegistry.Find(VillageID);
    if (!Found)
    {
        return false;
    }

    const float FinalPrice = CalculateEffectivePrice(VillageID, OfficerDiscountPercent);
    PlayerGold -= FinalPrice;
    Found->bIsPurchased = true;
    OnVillagePurchased.Broadcast(VillageID, FinalPrice);
    return true;
}

bool UTerritoryBuyoutSubsystem::SetDebtQuestStatus(FName VillageID, bool bCleared)
{
    FVillageBuyoutData* Found = VillageRegistry.Find(VillageID);
    if (!Found)
    {
        return false;
    }

    Found->bDebtQuestCleared = bCleared;
    return true;
}

bool UTerritoryBuyoutSubsystem::UnlockVillage(FName VillageID)
{
    FVillageBuyoutData* Found = VillageRegistry.Find(VillageID);
    if (!Found || Found->bIsUnlocked)
    {
        return false;
    }

    Found->bIsUnlocked = true;
    OnVillageUnlocked.Broadcast(VillageID);
    return true;
}

TMap<FName, bool> UTerritoryBuyoutSubsystem::GetPurchasedVillagesMap() const
{
    TMap<FName, bool> StatusMap;
    for (const TPair<FName, FVillageBuyoutData>& VillagePair : VillageRegistry)
    {
        StatusMap.Add(VillagePair.Key, VillagePair.Value.bIsPurchased);
    }

    return StatusMap;
}

void UTerritoryBuyoutSubsystem::LoadPurchasedState(const TMap<FName, bool>& SavedPurchasedMap)
{
    for (const TPair<FName, bool>& SavedVillage : SavedPurchasedMap)
    {
        FVillageBuyoutData* Found = VillageRegistry.Find(SavedVillage.Key);
        if (Found)
        {
            Found->bIsPurchased = SavedVillage.Value;
        }
    }
}
