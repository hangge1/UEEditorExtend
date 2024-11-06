// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AdvanceDeletionWidget.h"
#include "Widgets/Views/SListView.h"
#include "DebugHeader.h"
#include "SuperManager.h"

#define ListAll TEXT("List All Available Assets")
#define ListUnused TEXT("List Unused Assets")

void SAdvanceDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;
    StoredAssetData = InArgs._AssetsDataToStore;
    DisplayedAssetData = StoredAssetData;

    CheckBoxesArray.Empty();
    AssetsDataToDeleteArray.Empty();

    ComboxSourceItems.Add(MakeShared<FString>(ListAll));
    ComboxSourceItems.Add(MakeShared<FString>(ListUnused));


	FSlateFontInfo TitleTextFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TitleTextFont.Size = 30;

	ChildSlot
	[
		SNew(SVerticalBox)

		//First vertical slot for title text
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
				.Text(FText::FromString(TEXT("Advance Deletion")))
				.Font(TitleTextFont)
				.Justification(ETextJustify::Center)
				.ColorAndOpacity(FColor::White)
		]

		//Secondslot for drop down to specify the listing condition
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

            //ComboBox Slot
            +SHorizontalBox::Slot()
            [
                ConStructComboBox()
            ]
		]

		//Thirdslot for the asset list
		+SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SNew(SScrollBox)

            +SScrollBox::Slot()
            [
                ConstructAssetListView()
            ]
		]

		//Fourthslot for 3 buttons
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            
            //Button Delete All
            +SHorizontalBox::Slot()
            .FillWidth(10.f)
            .Padding(5.f)
            [
                COnstructDeleteAllButton()
            ]

            //Button Select All
            + SHorizontalBox::Slot()
            .FillWidth(10.f)
            .Padding(5.f)
            [
                COnstructSelectAllButton()
            ]

            //Button DeSelect All
            + SHorizontalBox::Slot()
            .FillWidth(10.f)
            .Padding(5.f)
            [
                COnstructDeSelectAllButton()
            ]
        ]
	];
}

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAdvanceDeletionTab::ConstructAssetListView()
{
    AssetListView = SNew(SListView<TSharedPtr<FAssetData>>)
        .ItemHeight(24.f)
        .ListItemsSource(&DisplayedAssetData)
        .OnGenerateRow(this, &SAdvanceDeletionTab::OnGenerateRowForList);

    return AssetListView.ToSharedRef();
}

void SAdvanceDeletionTab::RefreshAssetListView()
{
    AssetsDataToDeleteArray.Empty();
    CheckBoxesArray.Empty();

    if(AssetListView.IsValid())
    {
        AssetListView->RebuildList();
    }

}

#pragma region RowWidgetForAssetListView

TSharedRef<ITableRow> SAdvanceDeletionTab::OnGenerateRowForList(
    TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
    if(!AssetDataToDisplay.IsValid()) 
        return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);

    const FString DisplayAssetClassName = AssetDataToDisplay->AssetClassPath.GetAssetName().ToString();
    FSlateFontInfo AssetClassNameFont = GetEmboseedTextFont();
    AssetClassNameFont.Size = 10;

    const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();
    FSlateFontInfo AssetNameFont = GetEmboseedTextFont();
    AssetNameFont.Size = 15;

    TSharedRef<STableRow<TSharedPtr<FAssetData>>> RowWidget = 
    SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable).Padding(FMargin(5.f))
    [
        SNew(SHorizontalBox)

        // First Slot for check box
        +SHorizontalBox::Slot()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Center)
        .FillWidth(0.05f)
        [
            ConstructCheckBox(AssetDataToDisplay)
        ]

        // Second Slot for displaying asset class name
        + SHorizontalBox::Slot()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Fill)
        .FillWidth(0.55f)
        [
            ConstructTextForRowWidget(DisplayAssetClassName, AssetClassNameFont)
        ]

        // Third Slot for displaying asset name
        + SHorizontalBox::Slot()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Fill)
        [
            ConstructTextForRowWidget(DisplayAssetName, AssetNameFont)
        ]

        // Third Slot for a button
        + SHorizontalBox::Slot()
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Fill)
        [
            ConstructButtonForRowWidget(AssetDataToDisplay)
        ]
    ];

    return RowWidget;
}

TSharedRef<SCheckBox> SAdvanceDeletionTab::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
    TSharedRef<SCheckBox> ConstructCheckBox = SNew(SCheckBox)
    .Type(ESlateCheckBoxType::CheckBox)
    .OnCheckStateChanged(this, &SAdvanceDeletionTab::OnCheckBoxStateChanged, AssetDataToDisplay)
    .Visibility(EVisibility::Visible);

    CheckBoxesArray.Add(ConstructCheckBox);

    return ConstructCheckBox;
}

void SAdvanceDeletionTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
    switch (NewState)
    {
    case ECheckBoxState::Checked:
        AssetsDataToDeleteArray.AddUnique(AssetData);
        break;
    case ECheckBoxState::Unchecked:
        if(AssetsDataToDeleteArray.Contains(AssetData))
        {
            AssetsDataToDeleteArray.Remove(AssetData);
        }
        break;
    case ECheckBoxState::Undetermined:
        DebugHeader::Print(AssetData->AssetName.ToString() + TEXT("Is Undetermined"), FColor::Blue);
        checkNoEntry();
    default:
        break;
    }

    
}

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontToUse)
{
    TSharedRef<STextBlock> ConstructTextBlock = SNew(STextBlock)
    .Text(FText::FromString(TextContent))
    .Font(FontToUse)
    .ColorAndOpacity(FColor::White);

    return ConstructTextBlock;
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructButtonForRowWidget(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
    TSharedRef<SButton> ConstructButton = SNew(SButton)
    .Text(FText::FromString(TEXT("Delete")))
    .OnClicked(this, &SAdvanceDeletionTab::OnDeleteButtonClicked, AssetDataToDisplay);

    return ConstructButton;
}

FReply SAdvanceDeletionTab::OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData)
{
    FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));

    const bool bConfirmDelete = SuperManagerModule.DeleteSingleAssetForAssetList(*ClickedAssetData);
       
    if(bConfirmDelete)
    {
        // Update Source Item List
        if(StoredAssetData.Contains(ClickedAssetData))
        {
            StoredAssetData.Remove(ClickedAssetData);

            if(DisplayedAssetData.Contains(ClickedAssetData))
            {
                DisplayedAssetData.Remove(ClickedAssetData);
            }

            //Refresh the list
            RefreshAssetListView();
        }
    }

    return FReply::Handled();
}

TSharedRef<SButton> SAdvanceDeletionTab::COnstructDeleteAllButton()
{
    TSharedRef<SButton> DeleteAllButton = SNew(SButton)
    .ContentPadding(FMargin(5.f))
    .OnClicked(this, &SAdvanceDeletionTab::OnDeleteAllButtonClicked)
    [
        ConstructTextForTabButtons(TEXT("Delete All"))
    ];

    return DeleteAllButton;
}

FReply SAdvanceDeletionTab::OnDeleteAllButtonClicked()
{
    if(AssetsDataToDeleteArray.Num() == 0)
    {
        DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("No Asset Currently Selected!"));
        return FReply::Handled();
    }

    TArray<FAssetData> AssetDataToDelete;

    for (const TSharedPtr<FAssetData>& Data : AssetsDataToDeleteArray)
    {
        AssetDataToDelete.Add(*Data);
    }
    //Pass data to out module for deletetion
    FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));
  
    const bool bAssetDeleted = SuperManagerModule.DeleteMultipleAssetsForAssetList(AssetDataToDelete);
    if(bAssetDeleted)
    {
        for (const TSharedPtr<FAssetData>& DeletedData : AssetsDataToDeleteArray)
        {
            if(StoredAssetData.Contains(DeletedData))
            {
                StoredAssetData.Remove(DeletedData);
            }

            if( DisplayedAssetData.Contains(DeletedData) )
            {
                DisplayedAssetData.Remove(DeletedData);
            }
        }

        RefreshAssetListView();
    }


    return FReply::Handled();
}

TSharedRef<SButton> SAdvanceDeletionTab::COnstructSelectAllButton()
{
    TSharedRef<SButton> SelectAllButton = SNew(SButton)
        .ContentPadding(FMargin(5.f))
        .OnClicked(this, &SAdvanceDeletionTab::OnSelectAllButtonClicked)
        [
            ConstructTextForTabButtons(TEXT("Select All"))
        ];

    return SelectAllButton;
}

FReply SAdvanceDeletionTab::OnSelectAllButtonClicked()
{
    if(CheckBoxesArray.Num() == 0)
    {
        return FReply::Handled();
    }

    for (const TSharedRef<SCheckBox>& CheckBox : CheckBoxesArray)
    {
        if(CheckBox->IsChecked()) continue;

        CheckBox->ToggleCheckedState();
    }

    return FReply::Handled();
}

TSharedRef<SButton> SAdvanceDeletionTab::COnstructDeSelectAllButton()
{
    TSharedRef<SButton> UnSelectAllButton = 
        SNew(SButton)
        .ContentPadding(FMargin(5.f))
        .OnClicked(this, &SAdvanceDeletionTab::OnDeSelectAllButtonClicked)
        [
            ConstructTextForTabButtons(TEXT("DeSelect All"))
        ];

    return UnSelectAllButton;
}

FReply SAdvanceDeletionTab::OnDeSelectAllButtonClicked()
{
    if( CheckBoxesArray.Num() == 0 )
    {
        return FReply::Handled();
    }

    for( const TSharedRef<SCheckBox>& CheckBox : CheckBoxesArray )
    {
        if( !CheckBox->IsChecked() ) continue;

        CheckBox->ToggleCheckedState();
    }

    return FReply::Handled();
}

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructTextForTabButtons(const FString& TextContent)
{
    FSlateFontInfo ButtonTextFont = GetEmboseedTextFont();
    ButtonTextFont.Size = 15;

    TSharedRef<STextBlock> TextBlock = SNew(STextBlock)
        .Text(FText::FromString(TextContent))
        .Font(ButtonTextFont)
        .Justification(ETextJustify::Center);

    return TextBlock;
}

#pragma endregion

#pragma region ComboBoxForListCondition

TSharedRef<SComboBox<TSharedPtr<FString>>> SAdvanceDeletionTab::ConStructComboBox()
{
    TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructedComboBox
    = SNew(SComboBox<TSharedPtr<FString>>)
    .OptionsSource(&ComboxSourceItems)
    .OnGenerateWidget(this, &SAdvanceDeletionTab::OnGenerateComboContext)
    .OnSelectionChanged(this, &SAdvanceDeletionTab::OnComboxSeletionChanged)
    [
      SAssignNew(ComboDisplayTextBlock, STextBlock).Text(FText::FromString(TEXT("List All Seletc Option")))
    ];

    return ConstructedComboBox;
}

TSharedRef<SWidget> SAdvanceDeletionTab::OnGenerateComboContext(TSharedPtr<FString> SourceItem)
{
    TSharedRef<STextBlock> ConstructComboxRow =
    SNew(STextBlock).Text(FText::FromString(*SourceItem.Get()));
    return ConstructComboxRow;
}

void SAdvanceDeletionTab::OnComboxSeletionChanged(TSharedPtr<FString> SeletedOption, ESelectInfo::Type InSelectItem)
{
    ComboDisplayTextBlock->SetText(FText::FromString(*SeletedOption));

    FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));

    if(*SeletedOption == ListAll)
    {
        //List ALL Assets
        DisplayedAssetData = StoredAssetData;
        RefreshAssetListView();
    }
    else if(*SeletedOption == ListUnused)
    {
        //List UnUsed Assets
        SuperManagerModule.ListUnusedAssetsForAssetList(StoredAssetData, DisplayedAssetData);
        RefreshAssetListView();
    }

}

#pragma endregion  