#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "LanunQuestTypes.generated.h"

/** Baris data quest untuk DataTable import dari CSV. Digunakan untuk membaca QuestData.csv di Unreal Editor. */
USTRUCT(BlueprintType)
struct FLanunQuestDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FName QuestID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText QuestTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FText QuestDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FName QuestGiverNPCID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FName StartLocationID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (ClampMin = "1"))
	int32 MinLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FString PrerequisiteQuestIDs; // Comma-separated FName list

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int64 RewardRupiah = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 RewardXP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 RewardAttributePoints = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	int32 RewardSkillPoints = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	FGameplayTag CompletionTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	bool bIsMainStory = false;
};
