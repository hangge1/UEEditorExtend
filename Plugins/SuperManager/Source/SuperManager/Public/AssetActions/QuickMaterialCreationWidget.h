// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickMaterialCreationWidget.generated.h"

class UMaterialExpressionTextureSample;
class UTexture2D;
/**
 * 
 */
UCLASS()
class SUPERMANAGER_API UQuickMaterialCreationWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
public:
    
#pragma region QuickMaterialCreationCore

    UFUNCTION(BlueprintCallable)
    void CreateQuickMaterialFromSeletedTextures();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CreateMaterialFromSeletedTextures")
    bool bCustomMaterialName = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CreateMaterialFromSeletedTextures", meta = (EditCondition = "bCustomMaterialName"))
    FString MaterialName = TEXT("M_");

#pragma endregion

#pragma region SupportTextureNames
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
    TArray<FString> BaseColorArray = {
        TEXT("_BaseColor"),
        TEXT("_Albedo"),
        TEXT("_Diffuse"),
        TEXT("_diff")
    };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
    TArray<FString> MetallicArray = {
        TEXT("_Metallic"),
        TEXT("_metal")
    };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
    TArray<FString> RoughnessColorArray = {
        TEXT("_Roughness"),
        TEXT("_RoughnessMap"),
        TEXT("_rough")
    };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
    TArray<FString> NormalArray = {
        TEXT("_Normal"),
        TEXT("_NormalMap"),
        TEXT("_nor")
    };
#pragma endregion

private:

#pragma region QuickMaterialCreation

    bool ProcessSeletedData(const TArray<FAssetData>& SeletedAssets, 
        TArray<UTexture2D*>& OutSeletedTexturesArray, FString& OutSeletedTexturesData);

    bool CheckIsNameUsed(const FString& FolderPathToCheck, const FString& MaterialNameToCheck);

    UMaterial* CreateMaterialAsset(const FString& NameOfTheMaterial, const FString& PathToPut);

    void Default_CreateMaterialNodes(UMaterial* CreatedMaterial, UTexture2D* SelectedTexture, uint32& PinsConnectedCounter);
#pragma endregion

#pragma region CreateMaterialNodes

    bool TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSample, UMaterial* TargetMaterial, UTexture2D* SelectedTexture);

#pragma endregion
};
