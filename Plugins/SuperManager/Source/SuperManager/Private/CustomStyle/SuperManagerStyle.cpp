// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomStyle/SuperManagerStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

FName FSuperManagerStyle::CustomStyleSetName = FName("SuperManagerStyle");
TSharedPtr<FSlateStyleSet> FSuperManagerStyle::CreatedSlateStyleSet = nullptr;

void FSuperManagerStyle::InitializeIcons()
{
    if(!CreatedSlateStyleSet.IsValid())
    {
        CreatedSlateStyleSet = CreateSlateStyleSet();
    }
    FSlateStyleRegistry::RegisterSlateStyle(*CreatedSlateStyleSet);
}

void FSuperManagerStyle::ShutDown()
{
    
}

TSharedRef<FSlateStyleSet> FSuperManagerStyle::CreateSlateStyleSet()
{
    TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShareable<FSlateStyleSet>(new FSlateStyleSet(CustomStyleSetName));

    const FString IconDir = IPluginManager::Get().FindPlugin(TEXT("SuperManager"))->GetBaseDir() /"Resources";
    CustomStyleSet->SetContentRoot(IconDir);

    const FVector2D Icon16x16(16.f, 16.f);
    CustomStyleSet->Set("ContentBrowser.DeleteUnusedAssets", new FSlateImageBrush(IconDir/"DeleteUnusedAsset.png", Icon16x16));


    return CustomStyleSet;
}
