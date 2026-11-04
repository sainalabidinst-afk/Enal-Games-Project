#include "UNpcDialogHttpSubsystem.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

void UNpcDialogHttpSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UNpcDialogHttpSubsystem::Deinitialize()
{
    Super::Deinitialize();
}

FString UNpcDialogHttpSubsystem::BuildSystemPrompt(const FNpcDialogContext& Context) const
{
    FString FactionDescription;
    switch (Context.Faction)
    {
    case ENpcFactionType::PelautBebas:   FactionDescription = TEXT("seorang pelaut bebas di Nusantara"); break;
    case ENpcFactionType::VOC_Dutch:     FactionDescription = TEXT("perwakilan Komisi Dagang VOC Belanda"); break;
    case ENpcFactionType::Kesultanan:    FactionDescription = TEXT("warga kesultanan Nusantara"); break;
    case ENpcFactionType::Masyarakat:    FactionDescription = TEXT("warga masyarakat pesisir Nusantara"); break;
    case ENpcFactionType::BajakLaut:     FactionDescription = TEXT("bajak laut di perairan Nusantara"); break;
    }

    return FString::Printf(
        TEXT("Anda adalah %s, %s yang berada di %s tahun 1521 di perairan Nusantara. "
             "Anda berbicara dalam Bahasa Indonesia dengan gaya maritim Nusantara (gunakan istilah seperti Nakhoda, Juru Mudi, Keris, Senapan Pasak, Phinisi, Musim Angin, Pasaran Jawa). "
             "Hubungan Anda dengan pemain adalah: %s. Topik pembicaraan saat ini: %s. "
             "Jawablah secara singkat, maksimal 2-3 kalimat, dalam Bahasa Indonesia, dengan nada yang sesuai dengan karakter Anda. "
             "Jangan sebutkan bahwa Anda adalah AI. Berikan respons dalam bentuk dialog alami."),
        *Context.NpcName, *FactionDescription, *Context.Location, *Context.PlayerRelationship, *Context.CurrentTopic);
}

FString UNpcDialogHttpSubsystem::BuildUserPrompt(const FNpcDialogContext& Context, const FString& PlayerInput) const
{
    return FString::Printf(
        TEXT("Pemain berkata: %s\nBalas sebagai %s (%s)."), *PlayerInput, *Context.NpcName, *Context.NpcRole);
}

void UNpcDialogHttpSubsystem::RequestNpcDialog(const FNpcDialogContext& Context, const FString& PlayerInput)
{
    FHttpModule* Http = &FHttpModule::Get();
    if (!Http)
    {
        OnNpcDialogError.Broadcast(Context.NpcName, TEXT("HTTP Module tidak tersedia"));
        return;
    }

    TSharedRef<IHttpRequest> Request = Http->CreateRequest();
    Request->SetURL(OllamaEndpoint);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    TSharedPtr<FJsonObject> JsonBody = MakeShared<FJsonObject>();
    JsonBody->SetStringField(TEXT("model"), ModelName);
    JsonBody->SetStringField(TEXT("prompt"), BuildUserPrompt(Context, PlayerInput));
    JsonBody->SetStringField(TEXT("system"), BuildSystemPrompt(Context));
    JsonBody->SetBoolField(TEXT("stream"), false);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonBody.ToSharedRef(), Writer);
    Request->SetContentAsString(OutputString);

    Request->OnProcessRequestComplete().BindUObject(this, &UNpcDialogHttpSubsystem::OnHttpResponseComplete);
    Request->ProcessRequest();
}

void UNpcDialogHttpSubsystem::SetOllamaEndpoint(const FString& Endpoint)
{
    OllamaEndpoint = Endpoint;
}

void UNpcDialogHttpSubsystem::SetModelName(const FString& NewModelName)
{
    this->ModelName = NewModelName;
}

void UNpcDialogHttpSubsystem::OnHttpResponseComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        OnNpcDialogError.Broadcast(TEXT("Unknown"), TEXT("Request gagal atau tidak ada respons"));
        return;
    }

    int32 ResponseCode = Response->GetResponseCode();
    if (ResponseCode != 200)
    {
        OnNpcDialogError.Broadcast(TEXT("Unknown"), FString::Printf(TEXT("HTTP Error %d"), ResponseCode));
        return;
    }

    FString ResponseContent = Response->GetContentAsString();

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        FString DialogResponse;
        if (JsonObject->TryGetStringField(TEXT("response"), DialogResponse))
        {
            // Extract NPC name from context if available, fallback to "NPC"
            FString NpcName = TEXT("NPC");
            OnNpcDialogResponse.Broadcast(NpcName, DialogResponse);
        }
        else
        {
            OnNpcDialogError.Broadcast(TEXT("Unknown"), TEXT("Field 'response' tidak ditemukan dalam JSON"));
        }
    }
    else
    {
        OnNpcDialogError.Broadcast(TEXT("Unknown"), TEXT("Gagal parse JSON response"));
    }
}
