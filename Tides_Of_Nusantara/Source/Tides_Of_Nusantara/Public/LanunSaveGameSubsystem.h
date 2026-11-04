#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFramework/SaveGame.h"
#include "CalendarWorldSubsystem.h"
#include "CrewOfficerSubsystem.h"
#include "PetCompanionSubsystem.h"
#include "LanunSaveGameSubsystem.generated.h"

class APlayerController;

USTRUCT(BlueprintType)
struct FOfficerSaveState
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crew")
    FName AssignedShipID = NAME_None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crew")
    EOfficerRole CurrentRole = EOfficerRole::None;
};

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
};

UCLASS()
class TIDES_OF_NUSANTARA_API ULanunSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save Data")
    FLanunWorldSaveData SaveData;
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

    UFUNCTION(BlueprintCallable, Category = "Save System")
    bool SaveGameState(APlayerController* PC);

    UFUNCTION(BlueprintCallable, Category = "Save System")
    bool LoadGameState(APlayerController* PC);

    UFUNCTION(BlueprintPure, Category = "Save System")
    bool DoesSaveSlotExist() const;
};
