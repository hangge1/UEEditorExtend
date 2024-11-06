// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickMaterialCreationWidget.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "EditorUtilityLibrary.h"


#pragma region QuickMaterialCreationCore

void UQuickMaterialCreationWidget::CreateQuickMaterialFromSeletedTextures()
{
    if(bCustomMaterialName)
    {
        if(MaterialName.IsEmpty() || MaterialName.Equals(TEXT("M_")))
        {
            DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please input a valid material name!"));
            return;
        }
    }

    TArray<FAssetData> SeletedAssets = UEditorUtilityLibrary::GetSelectedAssetData();
    TArray<UTexture2D*> SelectedTexturesArray;
    FString SelectedTextureFolderPath;
    if( !ProcessSeletedData(SeletedAssets, SelectedTexturesArray, SelectedTextureFolderPath) )
        return;

    if(!CheckIsNameUsed(SelectedTextureFolderPath, MaterialName))
    {
        
    }
    DebugHeader::Print(SelectedTextureFolderPath, FColor::Cyan);
}

#pragma endregion

#pragma region QuickMaterialCreationCore

bool UQuickMaterialCreationWidget::ProcessSeletedData(const TArray<FAssetData>& SeletedAssets,
                        TArray<UTexture2D*>& OutSeletedTexturesArray, FString& OutSeletedTexturesPackagePath)
{
    if(SeletedAssets.Num() == 0)
    {
        DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No Texture Seleted!"), true);
        return false;
    }

    OutSeletedTexturesArray.Empty();
    OutSeletedTexturesPackagePath.Empty();

    bool bMaterialSet = false;
    for (const FAssetData& SeletedData : SeletedAssets)
    {
        UObject* SeletedAsset = SeletedData.GetAsset();
        if(!SeletedAsset) continue;

        UTexture2D* SeletedTexture = Cast<UTexture2D>(SeletedAsset);
        if( !SeletedTexture )
        {
            DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please select only Texture!\n" + SeletedAsset->GetName() + TEXT(" is not texture")), true);
            continue;
        }

        if( OutSeletedTexturesPackagePath.IsEmpty() )
        {
            OutSeletedTexturesPackagePath = SeletedData.PackagePath.ToString();
        }

        if( !bCustomMaterialName && !bMaterialSet )
        {
            MaterialName = SeletedAsset->GetName();
            MaterialName.RemoveFromStart(TEXT("T_"));
            MaterialName.InsertAt(0, TEXT("M_"));
            bMaterialSet = true;
        }

        OutSeletedTexturesArray.Add(SeletedTexture);
    }

    return !OutSeletedTexturesArray.IsEmpty();
}

bool UQuickMaterialCreationWidget::CheckIsNameUsed(const FString& FolderPathToCheck, const FString& MaterialNameToCheck)
{
    TArray<FString> ExistingAssetsPaths = UEditorAssetLibrary::ListAssets(FolderPathToCheck, false);

    for (const FString& ExistingAssetPath : ExistingAssetsPaths)
    {
        const FString ExistingAssetName = FPaths::GetBaseFilename(ExistingAssetPath);
        if(ExistingAssetName.Equals(MaterialNameToCheck))
        {
            DebugHeader::ShowMsgDialog(EAppMsgType::Ok, 
            TEXT("This Material Name is Already!\n" + MaterialNameToCheck), true);
            return false;
        }
    }

    return true;
}

#pragma endregion