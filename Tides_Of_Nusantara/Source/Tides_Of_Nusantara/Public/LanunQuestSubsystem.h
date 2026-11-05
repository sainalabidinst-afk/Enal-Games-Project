#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LanunQuestData.h"
#include "LanunQuestSubsystem.generated.h"

UCLASS()
class TIDES_OF_NUSANTARA_API ULanunQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Register a quest data definition (called from Data Table import) */
	UFUNCTION(BlueprintCallable, Category = "Lanun|Quest")
	void RegisterQuestDefinition(const FLanunQuestData& QuestData);

	/** Get quest definition by ID */
	UFUNCTION(BlueprintCallable, Category = "Lanun|Quest")
	bool GetQuestDefinition(FName QuestID, FLanunQuestData& OutQuestData) const;

	/** Start a quest (move from Available to InProgress) */
	UFUNCTION(BlueprintCallable, Category = "Lanun|Quest")
	bool StartQuest(FName QuestID, int32 CurrentDay);

	/** Update objective progress */
	UFUNCTION(BlueprintCallable, Category = "Lanun|Quest")
	bool UpdateObjective(FName QuestID, EQuestObjectiveType ObjectiveType, FName TargetID, int32 Amount = 1);

	/** Check if quest can be started (prerequisites met, level sufficient) */
	UFUNCTION(BlueprintPure, Category = "Lanun|Quest")
	bool CanStartQuest(FName QuestID) const;

	/** Get current quest state */
	UFUNCTION(BlueprintPure, Category = "Lanun|Quest")
	EQuestState GetQuestState(FName QuestID) const;

	/** Get all available quests (prerequisites met, not started) */
	UFUNCTION(BlueprintCallable, Category = "Lanun|Quest")
	TArray<FLanunQuestData> GetAvailableQuests() const;

	/** Get all in-progress quests */
	UFUNCTION(BlueprintCallable, Category = "Lanun|Quest")
	TArray<FLanunQuestData> GetActiveQuests() const;

	/** Get all completed quests */
	UFUNCTION(BlueprintCallable, Category = "Lanun|Quest")
	TArray<FLanunQuestData> GetCompletedQuests() const;

	/** Complete a quest and apply rewards */
	UFUNCTION(BlueprintCallable, Category = "Lanun|Quest")
	bool CompleteQuest(FName QuestID);

	/** Save quest state */
	UFUNCTION(BlueprintCallable, Category = "Lanun|Quest|Save")
	void SaveQuestState(TArray<FLanunQuestSaveData>& OutSaveData) const;

	/** Load quest state */
	UFUNCTION(BlueprintCallable, Category = "Lanun|Quest|Save")
	void LoadQuestState(const TArray<FLanunQuestSaveData>& SaveData);

	/** Check if player is at quest start location (for trigger volume overlap) */
	UFUNCTION(BlueprintCallable, Category = "Lanun|Quest")
	bool IsAtQuestStartLocation(FName QuestID, FName CurrentLocationID) const;

	UPROPERTY(BlueprintAssignable, Category = "Lanun|Quest|Events")
	FOnQuestStateChanged OnQuestStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Lanun|Quest|Events")
	FOnObjectiveUpdated OnObjectiveUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Lanun|Quest|Events")
	FOnQuestCompleted OnQuestCompleted;

private:
	UPROPERTY()
	TMap<FName, FLanunQuestData> QuestDefinitions;

	UPROPERTY()
	TMap<FName, FLanunQuestSaveData> QuestSaveData;

	void ApplyQuestRewards(const FLanunQuestData& QuestData);
	void CheckQuestCompletion(FName QuestID);
};
