#include "LanunSaveGameSubsystem.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "CrewOfficerSubsystem.h"
#include "PetCompanionSubsystem.h"
#include "ShipFleetSubsystem.h"
#include "TerritoryBuyoutSubsystem.h"

bool ULanunSaveGameSubsystem::SaveGameState(APlayerController* PC)
{
    ULanunSaveGame* SaveGameInstance = Cast<ULanunSaveGame>(
        UGameplayStatics::CreateSaveGameObject(ULanunSaveGame::StaticClass()));

    if (!SaveGameInstance)
    {
        return false;
    }

    UWorld* World = GetWorld();
    if (World)
    {
        UCalendarWorldSubsystem* CalendarSubsystem = World->GetSubsystem<UCalendarWorldSubsystem>();
        if (CalendarSubsystem)
        {
            FLanunWorldSaveData& SaveData = SaveGameInstance->SaveData;
            SaveData.SavedYear = CalendarSubsystem->CurrentYear;
            SaveData.SavedMonth = CalendarSubsystem->CurrentMonth;
            SaveData.SavedDayOfMonth = CalendarSubsystem->CurrentDayOfMonth;
            SaveData.SavedHour = CalendarSubsystem->CurrentHour;
            SaveData.SavedMinute = CalendarSubsystem->CurrentMinute;
            SaveData.TotalDaysPassed = CalendarSubsystem->TotalDaysPassed;
        }

        UTerritoryBuyoutSubsystem* TerritorySubsystem = World->GetSubsystem<UTerritoryBuyoutSubsystem>();
        if (TerritorySubsystem)
        {
            SaveGameInstance->SaveData.PurchasedVillages = TerritorySubsystem->GetPurchasedVillagesMap();
        }

        UShipFleetSubsystem* FleetSubsystem = World->GetSubsystem<UShipFleetSubsystem>();
        if (FleetSubsystem)
        {
            FShipFleetData Flagship;
            if (FleetSubsystem->GetFlagship(Flagship))
            {
                SaveGameInstance->SaveData.SavedFlagshipID = Flagship.ShipID;
            }

            TMap<FName, float>& HullHealthMap = SaveGameInstance->SaveData.SavedShipHullHealthMap;
            HullHealthMap.Reset();
            const TArray<FShipFleetData> FleetShips = FleetSubsystem->GetAllShipsInFleet();
            for (const FShipFleetData& Ship : FleetShips)
            {
                HullHealthMap.Add(Ship.ShipID, Ship.HullHealth);
            }
        }

        UCrewOfficerSubsystem* OfficerSubsystem = World->GetSubsystem<UCrewOfficerSubsystem>();
        if (OfficerSubsystem)
        {
            TMap<FName, FOfficerSaveState>& OfficerStates = SaveGameInstance->SaveData.SavedOfficerStates;
            OfficerStates.Reset();
            const TArray<FOfficerData> Officers = OfficerSubsystem->GetAllRegisteredOfficers();
            for (const FOfficerData& Officer : Officers)
            {
                FOfficerSaveState& State = OfficerStates.Add(Officer.OfficerID);
                State.AssignedShipID = Officer.AssignedShipID;
                State.CurrentRole = Officer.CurrentAssignedRole;
            }
        }

        UPetCompanionSubsystem* PetSubsystem = World->GetSubsystem<UPetCompanionSubsystem>();
        if (PetSubsystem)
        {
            FPetCompanionData ActivePet;
            SaveGameInstance->SaveData.SavedActivePetID = NAME_None;
            if (PetSubsystem->GetActivePet(ActivePet))
            {
                SaveGameInstance->SaveData.SavedActivePetID = ActivePet.PetID;
            }

            TMap<FName, FPetSaveState>& PetStates = SaveGameInstance->SaveData.SavedPetStates;
            PetStates.Reset();
            const TArray<FPetCompanionData> Pets = PetSubsystem->GetAllPets();
            for (const FPetCompanionData& Pet : Pets)
            {
                FPetSaveState& State = PetStates.Add(Pet.PetID);
                State.GrowthStage = Pet.GrowthStage;
                State.CurrentXP = Pet.CurrentXP;
                State.LoyaltyScore = Pet.LoyaltyScore;
                State.BehaviorMode = Pet.BehaviorMode;
            }
        }
    }

    if (PC && PC->GetPawn())
    {
        const APawn* Pawn = PC->GetPawn();
        SaveGameInstance->SaveData.PlayerLocation = Pawn->GetActorLocation();
        SaveGameInstance->SaveData.PlayerRotation = Pawn->GetActorRotation();
    }

    return UGameplayStatics::SaveGameToSlot(SaveGameInstance, DefaultSlotName, DefaultUserIndex);
}

bool ULanunSaveGameSubsystem::LoadGameState(APlayerController* PC)
{
    if (!DoesSaveSlotExist())
    {
        return false;
    }

    ULanunSaveGame* SaveGameInstance = Cast<ULanunSaveGame>(
        UGameplayStatics::LoadGameFromSlot(DefaultSlotName, DefaultUserIndex));

    if (!SaveGameInstance)
    {
        return false;
    }

    UWorld* World = GetWorld();
    if (World)
    {
        UCalendarWorldSubsystem* CalendarSubsystem = World->GetSubsystem<UCalendarWorldSubsystem>();
        if (CalendarSubsystem)
        {
            const FLanunWorldSaveData& SaveData = SaveGameInstance->SaveData;
            CalendarSubsystem->CurrentYear = SaveData.SavedYear;
            CalendarSubsystem->CurrentMonth = SaveData.SavedMonth;
            CalendarSubsystem->CurrentDayOfMonth = SaveData.SavedDayOfMonth;
            CalendarSubsystem->CurrentHour = SaveData.SavedHour;
            CalendarSubsystem->CurrentMinute = SaveData.SavedMinute;
            CalendarSubsystem->TotalDaysPassed = SaveData.TotalDaysPassed;
        }

        UTerritoryBuyoutSubsystem* TerritorySubsystem = World->GetSubsystem<UTerritoryBuyoutSubsystem>();
        if (TerritorySubsystem)
        {
            TerritorySubsystem->LoadPurchasedState(SaveGameInstance->SaveData.PurchasedVillages);
        }

        UShipFleetSubsystem* FleetSubsystem = World->GetSubsystem<UShipFleetSubsystem>();
        if (FleetSubsystem)
        {
            const FName& SavedFlagshipID = SaveGameInstance->SaveData.SavedFlagshipID;
            if (!SavedFlagshipID.IsNone())
            {
                FleetSubsystem->SetFlagship(SavedFlagshipID);
            }

            for (const TPair<FName, float>& HullHealthPair : SaveGameInstance->SaveData.SavedShipHullHealthMap)
            {
                FShipFleetData ShipData;
                if (FleetSubsystem->GetShipData(HullHealthPair.Key, ShipData))
                {
                    ShipData.HullHealth = FMath::Clamp(HullHealthPair.Value, 0.0f, ShipData.MaxHullHealth);
                    FleetSubsystem->RegisterShip(ShipData);
                }
            }
        }

        UCrewOfficerSubsystem* OfficerSubsystem = World->GetSubsystem<UCrewOfficerSubsystem>();
        if (OfficerSubsystem)
        {
            for (const TPair<FName, FOfficerSaveState>& OfficerPair : SaveGameInstance->SaveData.SavedOfficerStates)
            {
                if (!OfficerPair.Value.AssignedShipID.IsNone() && OfficerPair.Value.CurrentRole != EOfficerRole::None)
                {
                    OfficerSubsystem->AssignOfficerToShip(
                        OfficerPair.Key,
                        OfficerPair.Value.AssignedShipID,
                        OfficerPair.Value.CurrentRole);
                }
                else
                {
                    OfficerSubsystem->UnassignOfficer(OfficerPair.Key);
                }
            }
        }

        UPetCompanionSubsystem* PetSubsystem = World->GetSubsystem<UPetCompanionSubsystem>();
        if (PetSubsystem)
        {
            for (const TPair<FName, FPetSaveState>& PetPair : SaveGameInstance->SaveData.SavedPetStates)
            {
                FPetCompanionData PetData;
                if (PetSubsystem->GetPetData(PetPair.Key, PetData))
                {
                    PetData.GrowthStage = PetPair.Value.GrowthStage;
                    PetData.CurrentXP = FMath::Max(PetPair.Value.CurrentXP, 0.0f);
                    PetData.LoyaltyScore = FMath::Clamp(PetPair.Value.LoyaltyScore, 0.0f, 100.0f);
                    PetData.BehaviorMode = PetPair.Value.BehaviorMode;
                    PetSubsystem->RegisterPet(PetData);
                }
            }

            if (!SaveGameInstance->SaveData.SavedActivePetID.IsNone())
            {
                PetSubsystem->SetActivePet(SaveGameInstance->SaveData.SavedActivePetID);
            }
        }
    }

    if (PC && PC->GetPawn())
    {
        APawn* Pawn = PC->GetPawn();
        Pawn->SetActorLocationAndRotation(
            SaveGameInstance->SaveData.PlayerLocation,
            SaveGameInstance->SaveData.PlayerRotation);
    }

    return true;
}

bool ULanunSaveGameSubsystem::DoesSaveSlotExist() const
{
    return UGameplayStatics::DoesSaveGameExist(DefaultSlotName, DefaultUserIndex);
}
