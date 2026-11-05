#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LanunAttributeSetBase.h"
#include "LanunWalletWidgetBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRupiahChanged, int64, NewRupiah);

/**
 * Base class untuk UI Wallet/Rupiah.
 * Bind ke ULanunAttributeSetBase untuk menampilkan saldo Rupiah int64.
 * Polling via Tick atau panggil Refresh() dari Blueprint saat diperlukan.
 */
UCLASS()
class TIDES_OF_NUSANTARA_API ULanunWalletWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	ULanunWalletWidgetBase(const FObjectInitializer& ObjectInitializer);

	/** Set referensi ke ULanunAttributeSetBase untuk membaca saldo Rupiah. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Wallet")
	void SetAttributeSet(class ULanunAttributeSetBase* InAttributeSet);

	/** Ambil saldo Rupiah saat ini. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Wallet")
	int64 GetRupiah() const;

	/** Format saldo Rupiah sebagai string (misal: "Rp 150.000"). */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Wallet")
	FText GetRupiahFormatted() const;

	/** Segarkan tampilan dan broadcast event OnRupiahChanged. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Wallet")
	void Refresh();

	/** Event dipanggil saat saldo Rupiah berubah. */
	UPROPERTY(BlueprintAssignable, Category = "Lanun|UI|Events")
	FOnRupiahChanged OnRupiahChanged;

protected:
	/** Referensi ke AttributeSet (diatur melalui SetAttributeSet). */
	UPROPERTY()
	class ULanunAttributeSetBase* CachedAttributeSet = nullptr;

	/** Nilai Rupiah terakhir untuk deteksi perubahan. */
	int64 LastRupiah = 0;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
