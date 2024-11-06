// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Widgets/SCompoundWidget.h"

class SAdvanceDeletionTab : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAdvanceDeletionTab){}
        SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetsDataToStore);
        SLATE_ARGUMENT(FString, CurrentSeletedFolder);
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

private:
    FString CurrentSeletedFolderPath;
    TArray< TSharedPtr<FAssetData> > StoredAssetData;
    TArray< TSharedPtr<FAssetData> > DisplayedAssetData;
    TArray< TSharedPtr<FAssetData> > AssetsDataToDeleteArray;
    TArray< TSharedRef<SCheckBox> > CheckBoxesArray;

    void RefreshAssetListView();
    TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructAssetListView();
    TSharedPtr<SListView<TSharedPtr<FAssetData>>> AssetListView;

#pragma region RowWidgetForAssetListView

    //Row Widget
    TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable);

    void OnRowWidgetMouseButtonClicked(TSharedPtr<FAssetData> ClickData);

    //CheckBox in Row
    TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
    void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);

    //TextBlock in Row
    TSharedRef<STextBlock> ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontToUse);

    //Button in Row
    TSharedRef<SButton> ConstructButtonForRowWidget(const TSharedPtr<FAssetData>& AssetDataToDisplay);
    FReply OnDeleteButtonClicked(TSharedPtr<FAssetData> AssetDataToDisplay);

#pragma endregion

#pragma region TabButtons

    TSharedRef<SButton> COnstructDeleteAllButton();
    FReply OnDeleteAllButtonClicked();

    TSharedRef<SButton> COnstructSelectAllButton();
    FReply OnSelectAllButtonClicked();

    TSharedRef<SButton> COnstructDeSelectAllButton();
    FReply OnDeSelectAllButtonClicked();

    TSharedRef<STextBlock> ConstructTextForTabButtons(const FString& TextContent);

#pragma endregion

#pragma region ComboBoxForListCondition
    
    TSharedRef<SComboBox<TSharedPtr<FString>>> ConStructComboBox();

    TArray<TSharedPtr<FString>> ComboxSourceItems;

    TSharedRef<SWidget> OnGenerateComboContext(TSharedPtr<FString> SourceItem);

    void OnComboxSeletionChanged(TSharedPtr<FString> SeletedOption, ESelectInfo::Type InSelectItem);

    TSharedPtr<STextBlock> ComboDisplayTextBlock;

    TSharedRef<STextBlock> ConstructComboHelpTexts(const FString& TextContent, ETextJustify::Type Justify);
#pragma endregion   

    FSlateFontInfo GetEmboseedTextFont() const { return FCoreStyle::Get().GetFontStyle(FName("EmbossedText")); }
};