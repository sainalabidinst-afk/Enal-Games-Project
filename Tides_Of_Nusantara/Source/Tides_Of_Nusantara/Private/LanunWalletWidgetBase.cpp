#include "LanunWalletWidgetBase.h"
#include "LanunAttributeSetBase.h"
#include "Internationalization/Regex.h"

ULanunWalletWidgetBase::ULanunWalletWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ULanunWalletWidgetBase::SetAttributeSet(ULanunAttributeSetBase* InAttributeSet)
{
	CachedAttributeSet = InAttributeSet;
	Refresh();
}

int64 ULanunWalletWidgetBase::GetRupiah() const
{
	if (CachedAttributeSet)
	{
		return CachedAttributeSet->GetRupiah();
	}
	return 0;
}

FText ULanunWalletWidgetBase::GetRupiahFormatted() const
{
	const int64 Rupiah = GetRupiah();
	return FText::Format(LOCTEXT("RupiahFormat", "Rp {0}"), FText::AsNumber(Rupiah));
}

void ULanunWalletWidgetBase::Refresh()
{
	const int64 CurrentRupiah = GetRupiah();
	if (CurrentRupiah != LastRupiah)
	{
		LastRupiah = CurrentRupiah;
		OnRupiahChanged.Broadcast(CurrentRupiah);
	}
}

void ULanunWalletWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	Refresh();
}
