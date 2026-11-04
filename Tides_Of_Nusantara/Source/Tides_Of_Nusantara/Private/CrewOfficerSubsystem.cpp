#include "CrewOfficerSubsystem.h"

#include "Engine/AssetManager.h"
#include "LanunOfficerDataAsset.h"

void UCrewOfficerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    LoadOfficersFromAssetManager();

    if (OfficerRegistry.Num() == 0)
    {
        PopulateDefaultOfficers();
    }
}

void UCrewOfficerSubsystem::PopulateDefaultOfficers()
{
    FOfficerData Marewa;
    Marewa.OfficerID = FName(TEXT("OFFICER_Marewa"));
    Marewa.OfficerName = NSLOCTEXT("Crew", "Marewa", "Daeng Marewa");
    Marewa.PreferredRole = EOfficerRole::Nakhoda;
    Marewa.LeadershipSkill = 85;
    Marewa.NavigationSkill = 70;
    Marewa.GunnerySkill = 50;
    Marewa.AssignedShipID = FName(TEXT("SHIP_Phinisi_Flagship"));
    Marewa.CurrentAssignedRole = EOfficerRole::Nakhoda;
    RegisterOfficer(Marewa);

    FOfficerData Lontara;
    Lontara.OfficerID = FName(TEXT("OFFICER_Lontara"));
    Lontara.OfficerName = NSLOCTEXT("Crew", "Lontara", "Panglima Lontara");
    Lontara.PreferredRole = EOfficerRole::JuruMudi;
    Lontara.LeadershipSkill = 60;
    Lontara.NavigationSkill = 90;
    Lontara.GunnerySkill = 40;
    Lontara.AssignedShipID = FName(TEXT("SHIP_Phinisi_Flagship"));
    Lontara.CurrentAssignedRole = EOfficerRole::JuruMudi;
    RegisterOfficer(Lontara);

    FOfficerData Sangsang;
    Sangsang.OfficerID = FName(TEXT("OFFICER_Sangsang"));
    Sangsang.OfficerName = NSLOCTEXT("Crew", "Sangsang", "Ki Sangsang");
    Sangsang.PreferredRole = EOfficerRole::JuruMeriam;
    Sangsang.LeadershipSkill = 40;
    Sangsang.NavigationSkill = 35;
    Sangsang.GunnerySkill = 95;
    Sangsang.AssignedShipID = FName(TEXT("SHIP_Phinisi_Flagship"));
    Sangsang.CurrentAssignedRole = EOfficerRole::JuruMeriam;
    RegisterOfficer(Sangsang);
}

void UCrewOfficerSubsystem::RegisterOfficer(const FOfficerData& NewOfficer)
{
    if (!NewOfficer.OfficerID.IsNone())
    {
        OfficerRegistry.Add(NewOfficer.OfficerID, NewOfficer);
    }
}

bool UCrewOfficerSubsystem::AssignOfficerToShip(FName OfficerID, FName ShipID, EOfficerRole Role)
{
    FOfficerData* Officer = OfficerRegistry.Find(OfficerID);
    if (!Officer || ShipID.IsNone() || Role == EOfficerRole::None)
    {
        return false;
    }

    for (TPair<FName, FOfficerData>& OfficerPair : OfficerRegistry)
    {
        if (OfficerPair.Key != OfficerID &&
            OfficerPair.Value.AssignedShipID == ShipID &&
            OfficerPair.Value.CurrentAssignedRole == Role)
        {
            OfficerPair.Value.AssignedShipID = NAME_None;
            OfficerPair.Value.CurrentAssignedRole = EOfficerRole::None;
        }
    }

    Officer->AssignedShipID = ShipID;
    Officer->CurrentAssignedRole = Role;
    OnOfficerAssigned.Broadcast(OfficerID, ShipID, Role);
    return true;
}

bool UCrewOfficerSubsystem::UnassignOfficer(FName OfficerID)
{
    FOfficerData* Officer = OfficerRegistry.Find(OfficerID);
    if (!Officer || Officer->AssignedShipID.IsNone())
    {
        return false;
    }

    const FName PreviousShip = Officer->AssignedShipID;
    Officer->AssignedShipID = NAME_None;
    Officer->CurrentAssignedRole = EOfficerRole::None;
    OnOfficerUnassigned.Broadcast(OfficerID, PreviousShip);
    return true;
}

bool UCrewOfficerSubsystem::GetOfficerData(FName OfficerID, FOfficerData& OutOfficer) const
{
    const FOfficerData* Found = OfficerRegistry.Find(OfficerID);
    if (!Found)
    {
        return false;
    }

    OutOfficer = *Found;
    return true;
}

TArray<FOfficerData> UCrewOfficerSubsystem::GetOfficersOnShip(FName ShipID) const
{
    TArray<FOfficerData> Result;
    for (const TPair<FName, FOfficerData>& OfficerPair : OfficerRegistry)
    {
        if (OfficerPair.Value.AssignedShipID == ShipID)
        {
            Result.Add(OfficerPair.Value);
        }
    }

    return Result;
}

TArray<FOfficerData> UCrewOfficerSubsystem::GetAllRegisteredOfficers() const
{
    TArray<FOfficerData> Result;
    OfficerRegistry.GenerateValueArray(Result);
    return Result;
}

FShipBonusStats UCrewOfficerSubsystem::CalculateShipBonusStats(FName ShipID) const
{
    FShipBonusStats CumulativeBonuses;
    const TArray<FOfficerData> ShipOfficers = GetOfficersOnShip(ShipID);

    for (const FOfficerData& Officer : ShipOfficers)
    {
        const float EfficiencyMultiplier = Officer.CurrentAssignedRole == Officer.PreferredRole ? 1.25f : 1.0f;

        switch (Officer.CurrentAssignedRole)
        {
        case EOfficerRole::Nakhoda:
            CumulativeBonuses.ThrustMultiplier += Officer.LeadershipSkill * 0.002f * EfficiencyMultiplier;
            CumulativeBonuses.RudderTorqueMultiplier += Officer.LeadershipSkill * 0.0015f * EfficiencyMultiplier;
            break;
        case EOfficerRole::JuruMudi:
            CumulativeBonuses.RudderTorqueMultiplier += Officer.NavigationSkill * 0.005f * EfficiencyMultiplier;
            CumulativeBonuses.ThrustMultiplier += Officer.NavigationSkill * 0.001f * EfficiencyMultiplier;
            break;
        case EOfficerRole::JuruMeriam:
            CumulativeBonuses.CannonDamageMultiplier += Officer.GunnerySkill * 0.005f * EfficiencyMultiplier;
            CumulativeBonuses.CannonReloadTimeMultiplier -= Officer.GunnerySkill * 0.003f * EfficiencyMultiplier;
            break;
        case EOfficerRole::JuruLayar:
            CumulativeBonuses.ThrustMultiplier += Officer.NavigationSkill * 0.006f * EfficiencyMultiplier;
            break;
        default:
            break;
        }
    }

    CumulativeBonuses.ThrustMultiplier = FMath::Clamp(CumulativeBonuses.ThrustMultiplier, 0.5f, 2.5f);
    CumulativeBonuses.RudderTorqueMultiplier = FMath::Clamp(CumulativeBonuses.RudderTorqueMultiplier, 0.5f, 2.5f);
    CumulativeBonuses.CannonDamageMultiplier = FMath::Clamp(CumulativeBonuses.CannonDamageMultiplier, 0.5f, 3.0f);
    CumulativeBonuses.CannonReloadTimeMultiplier = FMath::Clamp(CumulativeBonuses.CannonReloadTimeMultiplier, 0.4f, 1.0f);
    return CumulativeBonuses;
}

void UCrewOfficerSubsystem::LoadOfficersFromAssetManager()
{
    UAssetManager& AssetManager = UAssetManager::Get();
    TArray<FAssetData> AssetDataList;
    AssetManager.GetPrimaryAssetDataList(FPrimaryAssetType("OfficerData"), AssetDataList);

    for (const FAssetData& AssetData : AssetDataList)
    {
        if (ULanunOfficerDataAsset* OfficerAsset = Cast<ULanunOfficerDataAsset>(AssetData.GetAsset()))
        {
            FOfficerData Officer;
            Officer.OfficerID = OfficerAsset->OfficerID;
            Officer.OfficerName = OfficerAsset->OfficerName;
            Officer.PreferredRole = OfficerAsset->PreferredRole;
            Officer.LeadershipSkill = OfficerAsset->LeadershipSkill;
            Officer.NavigationSkill = OfficerAsset->NavigationSkill;
            Officer.GunnerySkill = OfficerAsset->GunnerySkill;

            RegisterOfficer(Officer);
        }
    }
}
