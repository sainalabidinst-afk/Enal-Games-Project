#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFramework/SaveGame.h"
#include "CalendarWorldSubsystem.h"
#include "CrewOfficerSubsystem.h"
#include "PetCompanionSubsystem.h"
#include "LanunAttributeSetBase.h"
#include "LanunHeroSaveData.h"
#include "LanunQuestData.h"
#include "RivalNemesisSubsystem.h"
#include "GameplayTagContainer.h"
#include "LanunSaveGameSubsystem.generated.h"

class APlayerController;
class ULanunHeroProgressionSubsystem;

UENUM(BlueprintType)
enum class ECompanionLegacyStatus : uint8
{
	ActiveOnBoard,
	Retired,
	Deceased,
	Dismissed
};

/** Struct V1 (legacy) — format asli yang sudah tersimpan di file .sav lama. */
USTRUCT(BlueprintType)
struct FOfficerSaveState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crew")
	FName AssignedShipID = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crew")
	EOfficerRole CurrentRole = EOfficerRole::None;
};

/** Struct V2 — format baru dengan metadata warisan dan perk yang terpreservasi. */
USTRUCT(BlueprintType)
struct FOfficerLegacyState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crew|Legacy")
	FName OfficerID = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crew|Legacy")
	int32 GenerationIndex = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crew|Legacy")
	ECompanionLegacyStatus LegacyStatus = ECompanionLegacyStatus::ActiveOnBoard;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crew|Legacy")
	FGameplayTagContainer PreservedPerks;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crew|Legacy")
	int32 InheritedLoyaltyBonus = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crew|Legacy")
	int32 LegacyBonusAppliedGeneration = 1;
};

/** Data progresi Hero untuk persistensi save/load — didefinisikan di LanunHeroSaveData.h. */

USTRUCT(BlueprintType)
struct FPetSaveState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pet")
	EPetGrowthStage GrowthStage = EPetGrowthStage::Cub;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pet")
	float CurrentXP = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pet")
	float LoyaltyScore = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pet")
	EPetBehaviorMode BehaviorMode = EPetBehaviorMode::Guard;
};

USTRUCT(BlueprintType)
struct FLanunWorldSaveData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Calendar")
	int32 SavedYear = 1521;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Calendar")
	int32 SavedMonth = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Calendar")
	int32 SavedDayOfMonth = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Calendar")
	int32 SavedHour = 6;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Calendar")
	int32 SavedMinute = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Calendar")
	int32 TotalDaysPassed = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	FVector PlayerLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	FRotator PlayerRotation = FRotator::ZeroRotator;

	/** Dompet Rupiah terpusat (int64) — tersimpan permanen. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Currency|Rupiah")
	int64 SavedRupiah = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Territory")
	TMap<FName, bool> PurchasedVillages;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fleet")
	FName SavedFlagshipID = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fleet")
	TMap<FName, float> SavedShipHullHealthMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crew")
	TMap<FName, FOfficerSaveState> SavedOfficerStates;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pet")
	FName SavedActivePetID = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pet")
	TMap<FName, FPetSaveState> SavedPetStates;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hero|Save")
	FLanunHeroSaveData HeroSaveData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Nemesis|Save")
	TMap<FName, int32> SavedRivalGrudgeStates;

	/** Quest states for save/load persistence */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest|Save")
	TMap<FName, FLanunQuestSaveData> SavedQuestStates;

	/** Array V1 (deprecated) — digunakan hanya selama migrasi dari save file lama. */
	UPROPERTY()
	TArray<FOfficerSaveState> LegacyOfficers_DEPRECATED;

	/** Array V2 — hasil migrasi dari LegacyOfficers_DEPRECATED. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crew|Migration")
	TArray<FOfficerLegacyState> OfficerLegacyStates;
};

UCLASS()
class TIDES_OF_NUSANTARA_API ULanunSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save Data")
	FLanunWorldSaveData SaveData;

	/** Versi schema save game. 1 = lama, 2 = setelah migrasi. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save Data")
	int32 SaveSchemaVersion = 1;
};

UCLASS()
class TIDES_OF_NUSANTARA_API ULanunSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save System|Config")
	FString DefaultSlotName = TEXT("LanunSaveSlot_0");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save System|Config")
	int32 DefaultUserIndex = 0;

	/** Simpan game dengan protokol staging + backup atomic. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|Save")
	bool SaveGameAtomic(int32 SlotIndex = 0);

	/** Muat game dengan fallback ke backup jika primary corrupt. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|Save")
	bool LoadGameAtomic(int32 SlotIndex = 0);

	/** Simpan game state lengkap (kompatibel dengan kode lama). */
	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool SaveGameState(class APlayerController* PC);

	/** Muat game state lengkap (kompatibel dengan kode lama). */
	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool LoadGameState(class APlayerController* PC);

	UFUNCTION(BlueprintPure, Category = "Save System")
	bool DoesSaveSlotExist() const;

	/** Migrasi data save dari schema V1 ke V2. Dipanggil otomatis saat LoadGameState. */
	UFUNCTION(BlueprintCallable, Category = "Save System|Migration")
	void MigrateSaveDataSchema(ULanunSaveGame* SaveInstance);

private:
	FString GetSlotName(int32 SlotIndex) const;
	FString GetBackupSlotName(int32 SlotIndex) const;
	FString GetStagingSlotName(int32 SlotIndex) const;
	void MigrateDataV1ToV2(ULanunSaveGame* SaveObj);
};
