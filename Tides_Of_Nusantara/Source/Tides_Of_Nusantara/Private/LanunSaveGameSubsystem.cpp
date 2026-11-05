#include "LanunSaveGameSubsystem.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "CrewOfficerSubsystem.h"
#include "PetCompanionSubsystem.h"
#include "ShipFleetSubsystem.h"
#include "TerritoryBuyoutSubsystem.h"
#include "Math/UnrealMathUtility.h"
#include "LanunHeroProgressionSubsystem.h"
#include "RivalNemesisSubsystem.h"
#include "LanunQuestSubsystem.h"
#include "LanunAttributeSetBase.h"

FString ULanunSaveGameSubsystem::GetSlotName(int32 SlotIndex) const
{
	return FString::Printf(TEXT("LanunSaveSlot_%d"), SlotIndex);
}

FString ULanunSaveGameSubsystem::GetBackupSlotName(int32 SlotIndex) const
{
	return FString::Printf(TEXT("LanunSaveSlot_%d_Backup"), SlotIndex);
}

FString ULanunSaveGameSubsystem::GetStagingSlotName(int32 SlotIndex) const
{
	return FString::Printf(TEXT("LanunSaveSlot_%d_Staging"), SlotIndex);
}

void ULanunSaveGameSubsystem::MigrateDataV1ToV2(ULanunSaveGame* SaveObj)
{
	if (!SaveObj || SaveObj->SaveSchemaVersion >= 2)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Migrating Save Game Schema from V%d to V2..."), SaveObj->SaveSchemaVersion);

	FLanunWorldSaveData& SaveData = SaveObj->SaveData;

	for (const FOfficerSaveState& OldData : SaveData.LegacyOfficers_DEPRECATED)
	{
		FOfficerLegacyState NewLegacyState;
		NewLegacyState.OfficerID = OldData.AssignedShipID;
		NewLegacyState.GenerationIndex = 1;
		NewLegacyState.LegacyStatus = ECompanionLegacyStatus::ActiveOnBoard;
		NewLegacyState.InheritedLoyaltyBonus = 0;
		NewLegacyState.LegacyBonusAppliedGeneration = 1;

		SaveData.OfficerLegacyStates.Add(NewLegacyState);
	}

	SaveData.LegacyOfficers_DEPRECATED.Empty();
	SaveObj->SaveSchemaVersion = 2;

	UE_LOG(LogTemp, Log, TEXT("Save Game Migration Completed Successfully."));
}

bool ULanunSaveGameSubsystem::SaveGameAtomic(int32 SlotIndex)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		return false;
	}

	ULanunSaveGame* SaveObj = Cast<ULanunSaveGame>(UGameplayStatics::CreateSaveGameObject(ULanunSaveGame::StaticClass()));
	if (!SaveObj)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		UCalendarWorldSubsystem* CalendarSubsystem = World->GetSubsystem<UCalendarWorldSubsystem>();
		if (CalendarSubsystem)
		{
			FLanunWorldSaveData& SaveData = SaveObj->SaveData;
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
			SaveObj->SaveData.PurchasedVillages = TerritorySubsystem->GetPurchasedVillagesMap();
		}

		UShipFleetSubsystem* FleetSubsystem = World->GetSubsystem<UShipFleetSubsystem>();
		if (FleetSubsystem)
		{
			FShipFleetData Flagship;
			if (FleetSubsystem->GetFlagship(Flagship))
			{
				SaveObj->SaveData.SavedFlagshipID = Flagship.ShipID;
			}

			TMap<FName, float>& HullHealthMap = SaveObj->SaveData.SavedShipHullHealthMap;
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
			TMap<FName, FOfficerSaveState>& OfficerStates = SaveObj->SaveData.SavedOfficerStates;
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
			SaveObj->SaveData.SavedActivePetID = NAME_None;
			if (PetSubsystem->GetActivePet(ActivePet))
			{
				SaveObj->SaveData.SavedActivePetID = ActivePet.PetID;
			}

			TMap<FName, FPetSaveState>& PetStates = SaveObj->SaveData.SavedPetStates;
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

		ULanunHeroProgressionSubsystem* ProgSubsystem = GI->GetSubsystem<ULanunHeroProgressionSubsystem>();
		if (ProgSubsystem)
		{
			const FHeroProgressionState& ProgState = ProgSubsystem->GetProgressionState();
			const FHeroAttributeRaw& AttrRaw = ProgSubsystem->GetAttributeRaw();

			SaveObj->SaveData.HeroSaveData.Level = ProgState.Level;
			SaveObj->SaveData.HeroSaveData.CurrentXP = ProgState.CurrentXP;
			SaveObj->SaveData.HeroSaveData.AttributePoints = ProgState.UnallocatedAttributePoints;
			SaveObj->SaveData.HeroSaveData.SkillPoints = ProgState.UnallocatedSkillPoints;

			SaveObj->SaveData.HeroSaveData.RawAttributes.Empty();
			TMap<FGameplayTag, int32> AttrMap;
			AttrMap.Add(FGameplayTag::RequestGameplayTag(TEXT("Attribute.Strength")), AttrRaw.Strength);
			AttrMap.Add(FGameplayTag::RequestGameplayTag(TEXT("Attribute.Agility")), AttrRaw.Agility);
			AttrMap.Add(FGameplayTag::RequestGameplayTag(TEXT("Attribute.Leadership")), AttrRaw.Leadership);
			AttrMap.Add(FGameplayTag::RequestGameplayTag(TEXT("Attribute.Mysticism")), AttrRaw.Mysticism);
			SaveObj->SaveData.HeroSaveData.RawAttributes = AttrMap;
		}

		URivalNemesisSubsystem* NemesisSubsystem = GI->GetSubsystem<URivalNemesisSubsystem>();
		if (NemesisSubsystem)
		{
			SaveObj->SaveData.SavedRivalGrudgeStates = NemesisSubsystem->GetAllRivalGrudgeStates();
		}

		ULanunQuestSubsystem* QuestSubsystem = GI->GetSubsystem<ULanunQuestSubsystem>();
		if (QuestSubsystem)
		{
			TArray<FLanunQuestSaveData> QuestSaveArray;
			QuestSubsystem->SaveQuestState(QuestSaveArray);
			for (const FLanunQuestSaveData& QuestData : QuestSaveArray)
			{
				SaveObj->SaveData.SavedQuestStates.Add(QuestData.QuestID, QuestData);
			}
		}
	}

	ULanunHeroProgressionSubsystem* ProgSubsystem = GI->GetSubsystem<ULanunHeroProgressionSubsystem>();
	if (ProgSubsystem)
	{
		SaveObj->SaveData.SavedRupiah = ProgSubsystem->GetRupiahFromASC();
	}

	FString PrimarySlot = GetSlotName(SlotIndex);
	FString StagingSlot = GetStagingSlotName(SlotIndex);
	FString BackupSlot = GetBackupSlotName(SlotIndex);

	if (!UGameplayStatics::SaveGameToSlot(SaveObj, StagingSlot, 0))
	{
		UE_LOG(LogTemp, Error, TEXT("SaveGameAtomic: Gagal menyimpan ke staging slot %s"), *StagingSlot);
		return false;
	}

	if (UGameplayStatics::DoesSaveGameExist(PrimarySlot, 0))
	{
		if (USaveGame* PrimaryBackup = UGameplayStatics::LoadGameFromSlot(PrimarySlot, 0))
		{
			UGameplayStatics::SaveGameToSlot(PrimaryBackup, BackupSlot, 0);
		}
	}

	bool bSuccess = UGameplayStatics::SaveGameToSlot(SaveObj, PrimarySlot, 0);

	UGameplayStatics::DeleteGameInSlot(StagingSlot, 0);

	return bSuccess;
}

bool ULanunSaveGameSubsystem::LoadGameAtomic(int32 SlotIndex)
{
	FString PrimarySlot = GetSlotName(SlotIndex);
	FString BackupSlot = GetBackupSlotName(SlotIndex);

	USaveGame* LoadedBase = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(PrimarySlot, 0))
	{
		LoadedBase = UGameplayStatics::LoadGameFromSlot(PrimarySlot, 0);
	}

	if (!LoadedBase && UGameplayStatics::DoesSaveGameExist(BackupSlot, 0))
	{
		LoadedBase = UGameplayStatics::LoadGameFromSlot(BackupSlot, 0);
	}

	ULanunSaveGame* SaveObj = Cast<ULanunSaveGame>(LoadedBase);
	if (!SaveObj)
	{
		return false;
	}

	if (SaveObj->SaveSchemaVersion < 2)
	{
		MigrateDataV1ToV2(SaveObj);
		UGameplayStatics::SaveGameToSlot(SaveObj, PrimarySlot, 0);
	}

	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		ULanunHeroProgressionSubsystem* ProgSubsystem = GI->GetSubsystem<ULanunHeroProgressionSubsystem>();
		if (ProgSubsystem)
		{
			const FLanunHeroSaveData& HeroSave = SaveObj->SaveData.HeroSaveData;
			ProgSubsystem->LoadHeroData(HeroSave);
			ProgSubsystem->SetRupiahToASC(SaveObj->SaveData.SavedRupiah);
		}

		URivalNemesisSubsystem* NemesisSubsystem = GI->GetSubsystem<URivalNemesisSubsystem>();
		if (NemesisSubsystem)
		{
			NemesisSubsystem->LoadRivalGrudgeStates(SaveObj->SaveData.SavedRivalGrudgeStates);
		}

		ULanunQuestSubsystem* QuestSubsystem = GI->GetSubsystem<ULanunQuestSubsystem>();
		if (QuestSubsystem)
		{
			TArray<FLanunQuestSaveData> QuestLoadArray;
			for (const TPair<FName, FLanunQuestSaveData>& Pair : SaveObj->SaveData.SavedQuestStates)
			{
				QuestLoadArray.Add(Pair.Value);
			}
			QuestSubsystem->LoadQuestState(QuestLoadArray);
		}
	}

	return true;
}

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

		UGameInstance* GI = GetGameInstance();
		if (GI)
		{
			ULanunHeroProgressionSubsystem* ProgSubsystem = GI->GetSubsystem<ULanunHeroProgressionSubsystem>();
			if (ProgSubsystem)
			{
				const FHeroProgressionState& ProgState = ProgSubsystem->GetProgressionState();
				const FHeroAttributeRaw& AttrRaw = ProgSubsystem->GetAttributeRaw();

				SaveGameInstance->SaveData.HeroSaveData.Level = ProgState.Level;
				SaveGameInstance->SaveData.HeroSaveData.CurrentXP = ProgState.CurrentXP;
				SaveGameInstance->SaveData.HeroSaveData.AttributePoints = ProgState.UnallocatedAttributePoints;
				SaveGameInstance->SaveData.HeroSaveData.SkillPoints = ProgState.UnallocatedSkillPoints;

				SaveGameInstance->SaveData.HeroSaveData.RawAttributes.Empty();
				TMap<FGameplayTag, int32> AttrMap;
				AttrMap.Add(FGameplayTag::RequestGameplayTag(TEXT("Attribute.Strength")), AttrRaw.Strength);
				AttrMap.Add(FGameplayTag::RequestGameplayTag(TEXT("Attribute.Agility")), AttrRaw.Agility);
				AttrMap.Add(FGameplayTag::RequestGameplayTag(TEXT("Attribute.Leadership")), AttrRaw.Leadership);
				AttrMap.Add(FGameplayTag::RequestGameplayTag(TEXT("Attribute.Mysticism")), AttrRaw.Mysticism);
				SaveGameInstance->SaveData.HeroSaveData.RawAttributes = AttrMap;
			}

			URivalNemesisSubsystem* NemesisSubsystem = GI->GetSubsystem<URivalNemesisSubsystem>();
			if (NemesisSubsystem)
			{
				SaveGameInstance->SaveData.SavedRivalGrudgeStates = NemesisSubsystem->GetAllRivalGrudgeStates();
			}

			ULanunQuestSubsystem* QuestSubsystem = GI->GetSubsystem<ULanunQuestSubsystem>();
			if (QuestSubsystem)
			{
				TArray<FLanunQuestSaveData> QuestSaveArray;
				QuestSubsystem->SaveQuestState(QuestSaveArray);
				for (const FLanunQuestSaveData& QuestData : QuestSaveArray)
				{
					SaveGameInstance->SaveData.SavedQuestStates.Add(QuestData.QuestID, QuestData);
				}
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

	MigrateSaveDataSchema(SaveGameInstance);

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

		UGameInstance* GI = GetGameInstance();
		if (GI)
		{
			ULanunHeroProgressionSubsystem* ProgSubsystem = GI->GetSubsystem<ULanunHeroProgressionSubsystem>();
			if (ProgSubsystem)
			{
				const FLanunHeroSaveData& HeroSave = SaveGameInstance->SaveData.HeroSaveData;
				ProgSubsystem->LoadHeroData(HeroSave);
			}

			URivalNemesisSubsystem* NemesisSubsystem = GI->GetSubsystem<URivalNemesisSubsystem>();
			if (NemesisSubsystem)
			{
				NemesisSubsystem->LoadRivalGrudgeStates(SaveGameInstance->SaveData.SavedRivalGrudgeStates);
			}

			ULanunQuestSubsystem* QuestSubsystem = GI->GetSubsystem<ULanunQuestSubsystem>();
			if (QuestSubsystem)
			{
				TArray<FLanunQuestSaveData> QuestLoadArray;
				for (const TPair<FName, FLanunQuestSaveData>& Pair : SaveGameInstance->SaveData.SavedQuestStates)
				{
					QuestLoadArray.Add(Pair.Value);
				}
				QuestSubsystem->LoadQuestState(QuestLoadArray);
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

void ULanunSaveGameSubsystem::MigrateSaveDataSchema(ULanunSaveGame* SaveInstance)
{
	if (!SaveInstance)
	{
		return;
	}

	if (SaveInstance->SaveSchemaVersion >= 2)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Migrating Save Game Schema from V%d to V2..."), SaveInstance->SaveSchemaVersion);

	FLanunWorldSaveData& SaveData = SaveInstance->SaveData;

	for (const FOfficerSaveState& OldData : SaveData.LegacyOfficers_DEPRECATED)
	{
		FOfficerLegacyState NewLegacyState;
		NewLegacyState.OfficerID = OldData.AssignedShipID;
		NewLegacyState.GenerationIndex = 1;
		NewLegacyState.LegacyStatus = ECompanionLegacyStatus::ActiveOnBoard;
		NewLegacyState.InheritedLoyaltyBonus = 0;
		NewLegacyState.LegacyBonusAppliedGeneration = 1;

		SaveData.OfficerLegacyStates.Add(NewLegacyState);
	}

	SaveData.LegacyOfficers_DEPRECATED.Empty();
	SaveInstance->SaveSchemaVersion = 2;

	UE_LOG(LogTemp, Log, TEXT("Save Game Migration Completed Successfully."));
}
