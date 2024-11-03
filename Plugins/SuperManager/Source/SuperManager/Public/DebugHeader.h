#pragma once

#include "Misc/MessageDialog.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"


void Print(const FString& Message, const FColor& Color)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, Color, Message);
	}
}

void PrintLog(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}

EAppReturnType::Type ShowMsgDialog(EAppMsgType::Type MsgType, const FString& Message, bool bShowMsgAsWarning = true)
{
	if (bShowMsgAsWarning)
	{
		FText MsgTitle = FText::FromString(TEXT("Warning"));
		return FMessageDialog::Open(EAppMsgType::Ok,FText::FromString(Message),&MsgTitle);
	}
	else
	{
		return FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Message));
	}
}

void ShowNotifyInfo(const FString& Message)
{
	FNotificationInfo NotifyInfo(FText::FromString(Message));
	NotifyInfo.bUseLargeFont = true;
	NotifyInfo.FadeOutDuration = 7.f;
	
	FSlateNotificationManager::Get().AddNotification(NotifyInfo);
}