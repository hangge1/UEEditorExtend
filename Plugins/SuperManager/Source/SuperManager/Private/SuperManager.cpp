// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManager.h"
#include "ContentBrowserModule.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "SlateWidgets/AdvanceDeletionWidget.h"
#include "CustomStyle/SuperManagerStyle.h"

#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerModule::StartupModule()
{
    FSuperManagerStyle::InitializeIcons();

	InitCBMenuExtention();

	RegisterAdvanceDeletionTab();
}

void FSuperManagerModule::ShutdownModule()
{
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName("AdvanceDeletion"));

    FSuperManagerStyle::ShutDown();
}

#pragma region CustomEditorTab

void FSuperManagerModule::RegisterAdvanceDeletionTab()
{
	FTabSpawnerEntry& TabSpawnerEntry = FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		FName("AdvanceDeletion"),
		FOnSpawnTab::CreateRaw(this, &FSuperManagerModule::OnSpawnAdvanceDeletionTab)
	);


    TabSpawnerEntry.SetIcon(FSlateIcon(FSuperManagerStyle::GetStyleSetName(), "ContentBrowser.AdvanceDeletion"));
	TabSpawnerEntry.SetDisplayName(FText::FromString(TEXT("Advance Deletion")));
}

TSharedRef<SDockTab> FSuperManagerModule::OnSpawnAdvanceDeletionTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return
	SNew(SDockTab).TabRole(ETabRole::NomadTab)
	[
		SNew(SAdvanceDeletionTab)
        .AssetsDataToStore(GetAllAssetDataUnderSeletedFolder())
        .CurrentSeletedFolder(FolderPathSelected[0])
	];
}

TArray<TSharedPtr<FAssetData>> FSuperManagerModule::GetAllAssetDataUnderSeletedFolder()
{
    TArray<TSharedPtr<FAssetData>> AvaibleAssetsData;

    TArray<FString> AssetPathNameArray = UEditorAssetLibrary::ListAssets(FolderPathSelected[0]);

    for( const FString& AssetPathName : AssetPathNameArray )
    {
        if( AssetPathName.Contains(TEXT("Developers")) ||
           AssetPathName.Contains(TEXT("Collections")) ||
           AssetPathName.Contains(TEXT("__ExternalActors__")) ||
           AssetPathName.Contains(TEXT("__ExternalObjects__")) )
        {
            continue;
        }

        if( !UEditorAssetLibrary::DoesAssetExist(AssetPathName) )
            continue;

        const FAssetData Data = UEditorAssetLibrary::FindAssetData(AssetPathName);
        AvaibleAssetsData.Add(MakeShared<FAssetData>(Data));
    }

    return AvaibleAssetsData;
}

#pragma endregion

#pragma region ContentBrowerMenuExtention

void FSuperManagerModule::InitCBMenuExtention()
{
	FContentBrowserModule& ContentBrowserModule =
	FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	TArray<FContentBrowserMenuExtender_SelectedPaths>& ContextMenuExtenders =
	ContentBrowserModule.GetAllPathViewContextMenuExtenders();


	/* 法1 需要三行，正常写法
	FContentBrowserMenuExtender_SelectedPaths CustomCBMenuDelegate;
	CustomCBMenuDelegate.BindRaw(this, &FSuperManagerModule::CustomCBMenuExtender);
	ContextMenuExtenders.Add(CustomCBMenuDelegate);
	*/

	// 法2
	ContextMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FSuperManagerModule::CustomCBMenuExtender));
}

TSharedRef<FExtender> FSuperManagerModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender(new FExtender());

	if (SelectedPaths.Num() > 0)
	{
		FolderPathSelected = SelectedPaths;

		MenuExtender->AddMenuExtension(FName("Delete"),  //Extension Hook, Key
			EExtensionHook::After, //Inseting before or after
			TSharedPtr<FUICommandList>(), //Custom hot keys
			FMenuExtensionDelegate::CreateRaw(this, &FSuperManagerModule::AddCBMenuEntry));
	}

	return MenuExtender;
}

void FSuperManagerModule::AddCBMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Unused Assets")), //Title
		FText::FromString(TEXT("Safely Delete All unused assets under folder")), //Tooltip
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), "ContentBrowser.DeleteUnusedAssets"), //Icon
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteUnusedAssetButtonClicked) //actual function to execute
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Empty Folders")),
		FText::FromString(TEXT("Safely Delete All Empty Folders Without Any Assets")), 
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), "ContentBrowser.DeleteEmptyFolders"), //Icon
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnDeleteEmptyFoldersButtonClicked) 
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Advance Deletetion")),
		FText::FromString(TEXT("List assets by specific condition in a tab deleting")),
		FSlateIcon(FSuperManagerStyle::GetStyleSetName(), "ContentBrowser.AdvanceDeletion"), //Icon
		FExecuteAction::CreateRaw(this, &FSuperManagerModule::OnAdvanceDeletionButtonClicked)
	);

}

void FSuperManagerModule::OnDeleteUnusedAssetButtonClicked()
{
	if (FolderPathSelected.Num() > 1)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("You can only do this to one folder!"));
		return;
	}

	TArray<FString> AssetsPathNames = UEditorAssetLibrary::ListAssets(FolderPathSelected[0]);
	if (AssetsPathNames.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No asset found under seleted folder!"));
		return;
	}
	
	EAppReturnType::Type ConfirmResult = 
	DebugHeader::ShowMsgDialog(EAppMsgType::YesNo, TEXT("A total of ") + 
		FString::FromInt(AssetsPathNames.Num()) + TEXT("Assets need to be checked.\nWould you like to process?"), false);;

	if (ConfirmResult != EAppReturnType::Yes) 
		return;

	FixUpRedirectors();

	TArray<FAssetData> UnusedAssetsDataArray;
	for (const FString& AssetPathName : AssetsPathNames)
	{
		if (AssetPathName.Contains(TEXT("Developers")) ||
			AssetPathName.Contains(TEXT("Collections")) ||
			AssetPathName.Contains(TEXT("__ExternalActors__")) ||
			AssetPathName.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName)) 
			continue;

		TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);
		if (AssetReferencers.Num() == 0)
		{
			UnusedAssetsDataArray.Add(UEditorAssetLibrary::FindAssetData(AssetPathName));
		}
	}

	if (UnusedAssetsDataArray.Num() > 0)
	{
		ObjectTools::DeleteAssets(UnusedAssetsDataArray);
	}
	else
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No unused asset found under selected folder!"), false);
	}
}

void FSuperManagerModule::OnDeleteEmptyFoldersButtonClicked()
{
	FixUpRedirectors();

	TArray<FString> FolderPathsArray = UEditorAssetLibrary::ListAssets(FolderPathSelected[0], true, true);
	uint32 Counter = 0;

	FString EmptyFolderPathsNames;
	TArray<FString> EmptyFoldersPathsArray;

	for (const FString& FolderPath : FolderPathsArray)
	{
		if (FolderPath.Contains(TEXT("Developers")) ||
			FolderPath.Contains(TEXT("Collections")) ||
			FolderPath.Contains(TEXT("__ExternalActors__")) ||
			FolderPath.Contains(TEXT("__ExternalObjects__")))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesDirectoryExist(FolderPath))
			continue;

		if (UEditorAssetLibrary::DoesDirectoryHaveAssets(FolderPath))
			continue;

		EmptyFolderPathsNames.Append(FolderPath);
		EmptyFolderPathsNames.Append(TEXT("\n"));
		EmptyFoldersPathsArray.Add(FolderPath);
	}

	if (EmptyFoldersPathsArray.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No Empty Folder found under seleted folder"), false);
		return;
	}

	EAppReturnType::Type ConfirmResult = DebugHeader::ShowMsgDialog(EAppMsgType::OkCancel,
		TEXT("Empty Folders Found in:\n") + EmptyFolderPathsNames + TEXT("\nWould you like to delete all?"), false);

	if (ConfirmResult != EAppReturnType::Ok) 
		return;

	for (const FString& EmptyFolderPath : EmptyFoldersPathsArray)
	{
		if (UEditorAssetLibrary::DeleteDirectory(EmptyFolderPath))
		{
			Counter++;
		}
		else
		{
			DebugHeader::Print(TEXT("Folder: ") + EmptyFolderPath + " Delete Failed!", FColor::Red);
		}
	}

	if (Counter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Sucessfully Delete ") + FString::FromInt(Counter) + " Empty Folders!");
	}
	
}

void FSuperManagerModule::OnAdvanceDeletionButtonClicked()
{
    FixUpRedirectors();
	FGlobalTabmanager::Get()->TryInvokeTab(FName("AdvanceDeletion"));
}

void FSuperManagerModule::FixUpRedirectors()
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

#pragma endregion


#pragma region ProcessDataForAdvanceDeletionTab

bool FSuperManagerModule::DeleteSingleAssetForAssetList(const FAssetData& AssetDataToDelete)
{
    TArray<FAssetData> AssetDataForDeletion;
    AssetDataForDeletion.Add(AssetDataToDelete);

    return ObjectTools::DeleteAssets(AssetDataForDeletion) > 0;
}

bool FSuperManagerModule::DeleteMultipleAssetsForAssetList(const TArray<FAssetData>& AssetsToDelete)
{
    if(ObjectTools::DeleteAssets(AssetsToDelete) > 0)
    {
        return true;
    }

    return false;
}

void FSuperManagerModule::ListUnusedAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsDataToFilter, 
        TArray<TSharedPtr<FAssetData>>& OutUnusedAssetsData)
{
    OutUnusedAssetsData.Empty();

    for (const TSharedPtr<FAssetData>& AssetData : AssetsDataToFilter)
    {
        TArray<FString> AssetRefs = 
        UEditorAssetLibrary::FindPackageReferencersForAsset(AssetData->GetSoftObjectPath().ToString());
        if(AssetRefs.Num() == 0)
        {
            OutUnusedAssetsData.Add(AssetData);
        }
    }
}

void FSuperManagerModule::ListSameNameAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsDataToFilter, 
    TArray<TSharedPtr<FAssetData>>& OutUnusedAssetsData)
{
    OutUnusedAssetsData.Empty();

    TMultiMap<FString, TSharedPtr<FAssetData>> AssetsHelperMap;
    for( const TSharedPtr<FAssetData>& AssetData : AssetsDataToFilter )
    {
        AssetsHelperMap.Emplace(AssetData->AssetName.ToString(), AssetData);
    }

    for( const TSharedPtr<FAssetData>& AssetData : AssetsDataToFilter )
    {
        TArray<TSharedPtr<FAssetData>> OutAssetDatas;
        AssetsHelperMap.MultiFind(AssetData->AssetName.ToString(), OutAssetDatas);

        if(OutAssetDatas.Num() <= 1) continue;

        for (const TSharedPtr<FAssetData>& Data : OutAssetDatas)
        {
            if(Data.IsValid())
            {
                OutUnusedAssetsData.AddUnique(Data);
            }
        }
    }
}

void FSuperManagerModule::SyncCBToClickedAssetForAssetList(const FString& AssetPathToSync)
{
    TArray<FString> AsyncAssetArray;
    AsyncAssetArray.Add(AssetPathToSync);

    UEditorAssetLibrary::SyncBrowserToObjects(AsyncAssetArray);
}

#pragma endregion

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSuperManagerModule, SuperManager)