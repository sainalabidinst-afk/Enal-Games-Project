#include "PetCompanionSubsystem.h"

void UPetCompanionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    PopulateDefaultPets();
}

void UPetCompanionSubsystem::PopulateDefaultPets()
{
    FPetCompanionData Maung;
    Maung.PetID = FName(TEXT("PET_Harimau_Maung"));
    Maung.PetName = NSLOCTEXT("Pet", "Maung", "Si Maung");
    Maung.Species = EPetSpecies::HarimauSumatra;
    Maung.GrowthStage = EPetGrowthStage::Juvenile;
    Maung.CurrentXP = 450.0f;
    Maung.MaxXPForNextStage = 1000.0f;
    Maung.LoyaltyScore = 90.0f;
    Maung.bIsActiveCompanion = true;
    RegisterPet(Maung);

    FPetCompanionData Elang;
    Elang.PetID = FName(TEXT("PET_Elang_RajaLangit"));
    Elang.PetName = NSLOCTEXT("Pet", "Elang", "Raja Langit");
    Elang.Species = EPetSpecies::ElangLaut;
    Elang.GrowthStage = EPetGrowthStage::Cub;
    Elang.CurrentXP = 50.0f;
    Elang.MaxXPForNextStage = 500.0f;
    Elang.LoyaltyScore = 100.0f;
    RegisterPet(Elang);
}

void UPetCompanionSubsystem::RegisterPet(const FPetCompanionData& NewPet)
{
    if (!NewPet.PetID.IsNone())
    {
        FPetCompanionData PetData = NewPet;
        PetData.CurrentXP = FMath::Max(PetData.CurrentXP, 0.0f);
        PetData.MaxXPForNextStage = FMath::Max(PetData.MaxXPForNextStage, 1.0f);
        PetData.LoyaltyScore = FMath::Clamp(PetData.LoyaltyScore, 0.0f, 100.0f);
        PetRegistry.Add(PetData.PetID, PetData);
    }
}

bool UPetCompanionSubsystem::SetActivePet(FName PetID)
{
    if (!PetRegistry.Contains(PetID))
    {
        return false;
    }

    for (TPair<FName, FPetCompanionData>& PetPair : PetRegistry)
    {
        PetPair.Value.bIsActiveCompanion = PetPair.Key == PetID;
    }

    return true;
}

bool UPetCompanionSubsystem::GetActivePet(FPetCompanionData& OutPet) const
{
    for (const TPair<FName, FPetCompanionData>& PetPair : PetRegistry)
    {
        if (PetPair.Value.bIsActiveCompanion)
        {
            OutPet = PetPair.Value;
            return true;
        }
    }

    return false;
}

bool UPetCompanionSubsystem::GetPetData(FName PetID, FPetCompanionData& OutPet) const
{
    const FPetCompanionData* Found = PetRegistry.Find(PetID);
    if (!Found)
    {
        return false;
    }

    OutPet = *Found;
    return true;
}

TArray<FPetCompanionData> UPetCompanionSubsystem::GetAllPets() const
{
    TArray<FPetCompanionData> Result;
    PetRegistry.GenerateValueArray(Result);
    return Result;
}

void UPetCompanionSubsystem::AddPetExperience(FName PetID, float XPAmount)
{
    FPetCompanionData* Pet = PetRegistry.Find(PetID);
    if (!Pet || XPAmount <= 0.0f || Pet->GrowthStage == EPetGrowthStage::ApexAdult)
    {
        return;
    }

    Pet->CurrentXP += XPAmount;
    EvaluateGrowthStage(*Pet);
}

void UPetCompanionSubsystem::EvaluateGrowthStage(FPetCompanionData& Pet)
{
    while (Pet.GrowthStage != EPetGrowthStage::ApexAdult && Pet.CurrentXP >= Pet.MaxXPForNextStage)
    {
        Pet.CurrentXP -= Pet.MaxXPForNextStage;

        if (Pet.GrowthStage == EPetGrowthStage::Cub)
        {
            Pet.GrowthStage = EPetGrowthStage::Juvenile;
            Pet.MaxXPForNextStage = 2500.0f;
        }
        else
        {
            Pet.GrowthStage = EPetGrowthStage::ApexAdult;
            Pet.CurrentXP = 0.0f;
        }

        OnPetGrowthStageChanged.Broadcast(Pet.PetID, Pet.GrowthStage);
    }
}

void UPetCompanionSubsystem::AdjustLoyalty(FName PetID, float DeltaLoyalty)
{
    FPetCompanionData* Pet = PetRegistry.Find(PetID);
    if (!Pet || FMath::IsNearlyZero(DeltaLoyalty))
    {
        return;
    }

    Pet->LoyaltyScore = FMath::Clamp(Pet->LoyaltyScore + DeltaLoyalty, 0.0f, 100.0f);
    OnPetLoyaltyChanged.Broadcast(PetID, Pet->LoyaltyScore);
}

bool UPetCompanionSubsystem::SetBehaviorMode(FName PetID, EPetBehaviorMode NewMode)
{
    FPetCompanionData* Pet = PetRegistry.Find(PetID);
    if (!Pet)
    {
        return false;
    }

    Pet->BehaviorMode = NewMode;
    return true;
}

float UPetCompanionSubsystem::GetCompanionStatBonus(EPetSpecies Species, FName AttributeName) const
{
    FPetCompanionData ActivePet;
    if (!GetActivePet(ActivePet) || ActivePet.Species != Species)
    {
        return 0.0f;
    }

    float StageMultiplier = 1.0f;
    if (ActivePet.GrowthStage == EPetGrowthStage::Juvenile)
    {
        StageMultiplier = 1.8f;
    }
    else if (ActivePet.GrowthStage == EPetGrowthStage::ApexAdult)
    {
        StageMultiplier = 3.0f;
    }

    const float LoyaltyFactor = ActivePet.LoyaltyScore / 100.0f;
    if (Species == EPetSpecies::HarimauSumatra && AttributeName == FName(TEXT("BonusMeleeDamage")))
    {
        return 15.0f * StageMultiplier * LoyaltyFactor;
    }

    if (Species == EPetSpecies::ElangLaut && AttributeName == FName(TEXT("PerceptionRadius")))
    {
        return 500.0f * StageMultiplier * LoyaltyFactor;
    }

    if (Species == EPetSpecies::BuayaMuara && AttributeName == FName(TEXT("SwimmingBuoyancyBonus")))
    {
        return 0.15f * StageMultiplier * LoyaltyFactor;
    }

    return 0.0f;
}
