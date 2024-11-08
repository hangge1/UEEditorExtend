// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickAssetAction.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "Misc/MessageDialog.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"

void UQuickAssetAction::DuplicateAssets(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please enter a VALID number"));
		return;
	}

	TArray<FAssetData> SeletedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 Counter = 0;

	for (const FAssetData& CurrentAssetData : SeletedAssetsData)
	{
		for (int32 i = 0; i < NumOfDuplicates; i++) 
		{
			const FString SourceAssetPath = CurrentAssetData.GetSoftObjectPath().ToString();
			const FString NewDuplicatedAssetName = CurrentAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i + 1);
			const FString NewPathName = FPaths::Combine(CurrentAssetData.PackagePath.ToString(), NewDuplicatedAssetName);

			if (UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewPathName))
			{
				UEditorAssetLibrary::SaveAsset(NewPathName, false);
				++Counter;
			}
		}
	}

	if (Counter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully duplicated " + FString::FromInt(Counter) + " files"));
	}
	
}

void UQuickAssetAction::AddPrefixes()
{
	TArray<UObject*> SelectedObjects = UEditorUtilityLibrary::GetSelectedAssets();

	uint32 Counter = 0;

	for (UObject* currentAsset : SelectedObjects)
	{
		if (!currentAsset) continue;

		FString* PrefixFound = PrefixMap.Find(currentAsset->GetClass());
		if (!PrefixFound || PrefixFound->IsEmpty())
		{
			DebugHeader::Print(TEXT("Failed to find prefix for class") + currentAsset->GetClass()->GetName(), FColor::Red);
			continue;
		}

		FString OldName = currentAsset->GetName();
		if (OldName.StartsWith(*PrefixFound))
		{
			DebugHeader::Print(OldName + TEXT(" already has prefix added"), FColor::Red);
			continue;
		}

		if (currentAsset->IsA<UMaterialInstanceConstant>())
		{
			OldName.RemoveFromStart(TEXT("M_"));
			OldName.RemoveFromEnd(TEXT("_Inst"));
		}

		const FString NewNameWithPrefix = *PrefixFound + OldName;
		UEditorUtilityLibrary::RenameAsset(currentAsset, NewNameWithPrefix);

		Counter++;
	}

	DebugHeader::ShowNotifyInfo(TEXT("Successfully renamed " + FString::FromInt(Counter) + " Assets!"));
}

void UQuickAssetAction::RemoveUnusedAssets()
{
	TArray<FAssetData> SeletedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssetsData;

	FixUpRedirectors();
	for (const FAssetData& CurrentAssetData : SeletedAssetsData)
	{
		TArray<FString> AssetReferences = UEditorAssetLibrary::FindPackageReferencersForAsset(CurrentAssetData.GetSoftObjectPath().ToString(), true);
		if (AssetReferences.IsEmpty())
		{
			UnusedAssetsData.Add(CurrentAssetData);
		}
	}

	if (UnusedAssetsData.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No Unused Assets found among selected assets"));
		return;
	}

	int32 NumOfAssetsDeleted = ObjectTools::DeleteAssets(UnusedAssetsData);
	if (NumOfAssetsDeleted == 0) return;

	DebugHeader::ShowNotifyInfo(TEXT("Successfully Deleted " + FString::FromInt(NumOfAssetsDeleted) + " Unused Assets"));
}

void UQuickAssetAction::FixUpRedirectors()
{
	FAssetRegistryModule& AssetRegistryModule = 
	FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.bRecursiveClasses = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.ClassPaths.Add(UObjectRedirector::StaticClass()->GetClassPathName());

	TArray<FAssetData> OutRedirectors;
	AssetRegistryModule.Get().GetAssets(Filter, OutRedirectors);

	TArray<UObjectRedirector*> RedirectorsToFixArray;
	for (const FAssetData& RedirectorData : OutRedirectors)
	{
		if (UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsToFixArray.Add(RedirectorToFix);
		}
	}

	FAssetToolsModule& AssetToolsModule =
		FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
}
