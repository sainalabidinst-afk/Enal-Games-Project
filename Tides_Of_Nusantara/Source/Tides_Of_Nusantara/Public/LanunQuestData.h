#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LanunQuestData.generated.h"

UENUM(BlueprintType)
enum class EQuestObjectiveType : uint8
{
    TalkToNPC,
    ReachLocation,
    SailToLocation,
    CollectItem,
    DefeatEnemy,
    SurviveDuration
};

UENUM(BlueprintType)
enum class EQuestState : uint8
{
    NotStarted,
    Available,
    InProgress,
    Completed,
    Failed
};

USTRUCT(BlueprintType)
struct FQuestObjective
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Objective")
    EQuestObjectiveType ObjectiveType = EQuestObjectiveType::TalkToNPC;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Objective")
    FText ObjectiveText;

    /** Target NPC ID (for TalkToNPC) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Objective")
    FName TargetNPCID = NAME_None;

    /** Target Location Name (for ReachLocation / SailToLocation) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Objective")
    FName TargetLocationID = NAME_None;

    /** Item ID to collect (for CollectItem) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Objective")
    FName TargetItemID = NAME_None;

    /** Required amount */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Objective", meta = (ClampMin = "1"))
    int32 RequiredAmount = 1;

    /** Current progress (runtime only, not saved in data table) */
    UPROPERTY(Transient, BlueprintReadWrite, Category = "Quest|Objective")
    int32 CurrentAmount = 0;

    /** Is this objective completed? */
    UPROPERTY(Transient, BlueprintReadWrite, Category = "Quest|Objective")
    bool bIsCompleted = false;
};

USTRUCT(BlueprintType)
struct FQuestReward
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Reward")
    int64 RupiahReward = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Reward")
    int64 XPReward = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Reward")
    int32 AttributePointReward = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Reward")
    int32 SkillPointReward = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Reward")
    FName UnlockSkillID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Reward")
    int32 FactionReputationChange = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Reward")
    int32 TerritoryPopularityChange = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Reward")
    int32 CrewLoyaltyChange = 0;
};

USTRUCT(BlueprintType)
struct FLanunQuestData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Identity")
    FName QuestID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Identity")
    FText QuestTitle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Identity")
    FText QuestDescription;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Identity")
    FName QuestGiverNPCID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Identity")
    FName StartLocationID = NAME_None;

    /** Prerequisite quests that must be completed first */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Progression")
    TArray<FName> PrerequisiteQuestIDs;

    /** Minimum level required to start */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Progression", meta = (ClampMin = "1"))
    int32 MinLevel = 1;

    /** Objectives in order */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Objectives")
    TArray<FQuestObjective> Objectives;

    /** Rewards upon completion */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Rewards")
    FQuestReward Rewards;

    /** Gameplay tag to grant upon completion */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Tags")
    FGameplayTag CompletionTag;

    /** Is this a main story quest? */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Flags")
    bool bIsMainStory = false;
};

USTRUCT(BlueprintType)
struct FLanunQuestSaveData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest|Save")
    FName QuestID = NAME_None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest|Save")
    EQuestState State = EQuestState::NotStarted;

    /** Current objective index */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest|Save")
    int32 CurrentObjectiveIndex = 0;

    /** Objective progress (parallel array to Objectives in data table) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest|Save")
    TArray<int32> ObjectiveProgress;

    /** Completed objectives */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest|Save")
    TArray<bool> ObjectiveCompleted;

    /** Timestamp when quest was accepted (calendar day) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest|Save")
    int32 AcceptedDay = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestStateChanged, FName, QuestID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectiveUpdated, FName, QuestID, int32, ObjectiveIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestCompleted, FName, QuestID);
