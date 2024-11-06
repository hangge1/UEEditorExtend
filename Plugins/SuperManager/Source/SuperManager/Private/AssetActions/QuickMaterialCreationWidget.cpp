// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickMaterialCreationWidget.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "EditorUtilityLibrary.h"
#include "AssetToolsModule.h"
#include "Factories/MaterialFactoryNew.h"
#include "Materials/MaterialExpressionTextureSample.h"

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
        return;
    }

    UMaterial* CreatedMaterial = CreateMaterialAsset(MaterialName, SelectedTextureFolderPath);
    if(!CreatedMaterial)
    {
        DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Failed to create material!"));
        return;
    }

    uint32 PinsConnectedCounter = 0;
    for (UTexture2D* SelectTexture : SelectedTexturesArray)
    {
        if(!SelectTexture) continue;

        Default_CreateMaterialNodes(CreatedMaterial, SelectTexture, PinsConnectedCounter);
    }
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

// If return true, then No Same Name Asset; 
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

UMaterial* UQuickMaterialCreationWidget::CreateMaterialAsset(const FString& NameOfTheMaterial, const FString& PathToPut)
{
    FAssetToolsModule& AssetToolModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

    UMaterialFactoryNew* MaterialFactory = NewObject<UMaterialFactoryNew>();

    UObject* CreatedObject = AssetToolModule.Get().CreateAsset(NameOfTheMaterial, PathToPut, 
        UMaterial::StaticClass(), MaterialFactory);

    return Cast<UMaterial>(CreatedObject);
}

void UQuickMaterialCreationWidget::Default_CreateMaterialNodes(UMaterial* CreatedMaterial, UTexture2D* SelectedTexture, uint32& PinsConnectedCounter)
{
    UMaterialExpressionTextureSample* TextureSampleNode = NewObject<UMaterialExpressionTextureSample>(CreatedMaterial);
    if(!TextureSampleNode) return;

    if(!CreatedMaterial->HasBaseColorConnected())
    {
        if(TryConnectBaseColor(TextureSampleNode, CreatedMaterial, SelectedTexture))
        {
            PinsConnectedCounter++;
            return;
        }
    }
}

#pragma endregion

#pragma region CreateMaterialNodes

bool UQuickMaterialCreationWidget::TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSampleNode, 
    UMaterial* CreatedMaterial, UTexture2D* SelectedTexture)
{
    for(const FString& BaseColorName : BaseColorArray)
    {
        if(SelectedTexture->GetName().Contains(BaseColorName))
        {
            //Connect pins to base color socket here
            TextureSampleNode->Texture = SelectedTexture;

            CreatedMaterial->GetExpressionCollection().AddExpression(TextureSampleNode);
            CreatedMaterial->GetExpressionInputForProperty(MP_BaseColor)->Expression = TextureSampleNode;
            CreatedMaterial->PostEditChange();

            TextureSampleNode->MaterialExpressionEditorX -= 600;

            return true;
        }
    }

    return false;
}

#pragma endregion