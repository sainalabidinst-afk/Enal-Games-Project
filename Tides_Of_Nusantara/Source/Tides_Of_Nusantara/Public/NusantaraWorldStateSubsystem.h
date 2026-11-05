#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LanunQuestData.h"
#include "NusantaraWorldStateSubsystem.generated.h"

UENUM(BlueprintType)
enum class ERegionState : uint8
{
    Unlocked,
    Discovered,
    Controlled,
    Blockaded,
    Unrest
};

USTRUCT(BlueprintType)
struct FRegionData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World|Region")
    FName RegionID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World|Region")
    FText RegionName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World|Region")
    ERegionState State = ERegionState::Unlocked;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World|Region")
    int32 DiscoveryLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World|Region")
    int32 ControlPercentage = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World|Region")
    int32 UnrestLevel = 0;
};

USTRUCT(BlueprintType)
struct FLanunQuestJournalEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World|Quest")
    FName QuestID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World|Quest")
    FText QuestTitle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World|Quest")
    EQuestState State = EQuestState::NotStarted;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World|Quest")
    int32 CurrentObjectiveIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World|Quest")
    FString LastUpdatedLocation;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRegionStateChanged, FName, RegionID, ERegionState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestJournalUpdated, FName, QuestID, EQuestState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFirstQuestStarted, FName, QuestID);

/**
 * Subsystem untuk melacak state dunia Nusantara dan progresi quest.
 * Digunakan untuk Vertical Slice Quest 1 (Sumpah Siri') dan quest berikutnya.
 */
UCLASS()
class TIDES_OF_NUSANTARA_API UNusantaraWorldStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Set state wilayah (contoh: Teluk Spermonde, Bandar Somba Opu, dll). */
	UFUNCTION(BlueprintCallable, Category = "Lanun|World|Region")
	void SetRegionState(FName RegionID, ERegionState NewState, int32 DiscoveryLevel = 0, int32 ControlPercentage = 0);

	/** Ambil state wilayah. */
	UFUNCTION(BlueprintPure, Category = "Lanun|World|Region")
	ERegionState GetRegionState(FName RegionID) const;

	/** Tandai quest sebagai started dalam journal. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|World|Quest")
	void AddQuestToJournal(FName QuestID, FText QuestTitle);

	/** Update state quest dalam journal. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|World|Quest")
	void UpdateQuestInJournal(FName QuestID, EQuestState NewState, int32 ObjectiveIndex = 0);

	/** Ambil daftar semua quest dalam journal. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|World|Quest")
	TArray<FLanunQuestJournalEntry> GetQuestJournal() const;

	/** Cek apakah quest tertentu telah diselesaikan. */
	UFUNCTION(BlueprintPure, Category = "Lanun|World|Quest")
	bool IsQuestCompleted(FName QuestID) const;

	/** Cek apakah quest tertentu sedang berlangsung. */
	UFUNCTION(BlueprintPure, Category = "Lanun|World|Quest")
	bool IsQuestInProgress(FName QuestID) const;

	/** Set lokasi terakhir pemain (untuk trigger quest lokasi). */
	UFUNCTION(BlueprintCallable, Category = "Lanun|World|Location")
	void SetLastLocation(FName LocationID);

	/** Ambil lokasi terakhir pemain. */
	UFUNCTION(BlueprintPure, Category = "Lanun|World|Location")
	FName GetLastLocation() const;

	/** Cek apakah pemain berada di lokasi awal quest. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|World|Location")
	bool IsAtLocation(FName LocationID) const;

	/** Tandai Quest 1 (Sumpah Siri') sebagai started. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|World|Quest|Quest1")
	void StartQuest1_SumpahSiri();

	/** Cek apakah Quest 1 sudah selesai. */
	UFUNCTION(BlueprintPure, Category = "Lanun|World|Quest|Quest1")
	bool IsQuest1Completed() const;

	/** Tandai Quest 1 sebagai completed. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|World|Quest|Quest1")
	void CompleteQuest1_SumpahSiri();

	UPROPERTY(BlueprintAssignable, Category = "Lanun|World|Events")
	FOnRegionStateChanged OnRegionStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Lanun|World|Events")
	FOnQuestJournalUpdated OnQuestJournalUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Lanun|World|Events")
	FOnFirstQuestStarted OnFirstQuestStarted;

private:
	UPROPERTY()
	TMap<FName, FRegionData> RegionStates;

	UPROPERTY()
	TMap<FName, FLanunQuestJournalEntry> QuestJournal;

	UPROPERTY()
	FName LastLocationID = NAME_None;
};
