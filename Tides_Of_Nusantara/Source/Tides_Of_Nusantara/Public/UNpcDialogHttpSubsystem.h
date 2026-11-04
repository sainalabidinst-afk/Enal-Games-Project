#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Containers/UnrealString.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UNpcDialogHttpSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNpcDialogResponse, FString, NpcName, FString, DialogResponse);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNpcDialogError, FString, NpcName, FString, ErrorMessage);

UENUM(BlueprintType)
enum class ENpcFactionType : uint8
{
    PelautBebas,
    VOC_Dutch,
    Kesultanan,
    Masyarakat,
    BajakLaut
};

USTRUCT(BlueprintType)
struct FNpcDialogContext
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Dialog")
    FString NpcName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Dialog")
    FString NpcRole;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Dialog")
    FString Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Dialog")
    ENpcFactionType Faction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Dialog")
    FString PlayerRelationship;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Dialog")
    FString CurrentTopic;
};

UCLASS()
class TIDES_OF_NUSANTARA_API UNpcDialogHttpSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "NPC Dialog|Ollama")
    void RequestNpcDialog(const FNpcDialogContext& Context, const FString& PlayerInput);

    UFUNCTION(BlueprintCallable, Category = "NPC Dialog|Ollama")
    void SetOllamaEndpoint(const FString& Endpoint);

    UFUNCTION(BlueprintCallable, Category = "NPC Dialog|Ollama")
    void SetModelName(const FString& NewModelName);

    UFUNCTION(BlueprintPure, Category = "NPC Dialog|Ollama")
    FString GetOllamaEndpoint() const { return OllamaEndpoint; }

    UFUNCTION(BlueprintPure, Category = "NPC Dialog|Ollama")
    FString GetModelName() const { return ModelName; }

    UPROPERTY(BlueprintAssignable, Category = "NPC Dialog|Events")
    FOnNpcDialogResponse OnNpcDialogResponse;

    UPROPERTY(BlueprintAssignable, Category = "NPC Dialog|Events")
    FOnNpcDialogError OnNpcDialogError;

private:
    FString OllamaEndpoint = TEXT("http://localhost:11434/api/generate");
    FString ModelName = TEXT("llama3");

    FString BuildSystemPrompt(const FNpcDialogContext& Context) const;
    FString BuildUserPrompt(const FNpcDialogContext& Context, const FString& PlayerInput) const;
    void OnHttpResponseComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
