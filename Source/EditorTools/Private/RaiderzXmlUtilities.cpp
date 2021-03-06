// Copyright 2018 Moikkai Games. All Rights Reserved.


#include "RaiderzXmlUtilities.h"

#include "Misc/Paths.h"
#include "HAL/FileManagerGeneric.h"

const FString URaiderzXmlUtilities::DarkRaiderzAssetPath(TEXT("F:/GameDev/DarkRaidAssets"));
const FString URaiderzXmlUtilities::DataFolderPath(TEXT("E:/RaiderzAssets/datadump/Data"));
const FString URaiderzXmlUtilities::SoundXmlFilePath(TEXT("E:/RaiderzAssets/datadump/Data/Sound/sound.xml"));
const FString URaiderzXmlUtilities::NPCXmlFilePath(TEXT("E:/RaiderzAssets/datadump/Data/system/npc.xml"));
const FString URaiderzXmlUtilities::TalentHitInfoXmlFilePath(TEXT("E:/RaiderzAssets/datadump/Data/system/talent_hit_info.xml"));

// FXmlParser can't parse original talent.xml, so it has been replaced with talent_unrealengine.xml which removed a few details that FXmlParser was unable to parse
// const FString URaiderzXmlUtilities::NPCXmlFilePath(TEXT("E:/RaiderzAssets/datadump/Data/system/talent.xml"));
const FString URaiderzXmlUtilities::TalentXmlFilePath(TEXT("E:/RaiderzAssets/talent_unrealengine.xml"));

const FString URaiderzXmlUtilities::EluAnimationXmlExt(TEXT(".elu.animation.xml"));
const FString URaiderzXmlUtilities::EluAnimationEventXmlExt(TEXT(".elu.animationevent.xml"));
const FString URaiderzXmlUtilities::EluAnimationInfoXmlExt(TEXT(".elu.animationInfo.xml"));
const FString URaiderzXmlUtilities::EluAnimationSoundEventXmlExt(TEXT(".elu.animationsoundevent.xml"));
const FString URaiderzXmlUtilities::EluXmlExt(TEXT(".elu.xml"));
const FString URaiderzXmlUtilities::SceneXmlExt(TEXT(".scene.xml"));

URaiderzXmlUtilities::URaiderzXmlUtilities(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

bool URaiderzXmlUtilities::GetRaiderzFilePath(const FString& InFileName, FString& OutFilePath)
{
	FString FileExtension = GetRaiderzFileExtension(InFileName);

	if (FileExtension != TEXT(""))
	{
		TArray<FString> AllFiles;
		IFileManager& FileManager = IFileManager::Get();

		FString SearchString = FString("*") + FileExtension;
		FileManager.FindFilesRecursive(AllFiles, *URaiderzXmlUtilities::DataFolderPath, *SearchString, true, false);

		for (const FString& FilePath : AllFiles)
		{
			const FString CleanFileName = FPaths::GetCleanFilename(FilePath);
			if (CleanFileName == InFileName)
			{
				OutFilePath = FilePath;
				return true;
			}
		}
	}

	return false;
}

FString URaiderzXmlUtilities::GetRaiderzFileExtension(const FString& FilePath, bool bIncludeDot)
{
	const FString Filename = FPaths::GetCleanFilename(FilePath);
	int32 DotPos = Filename.Find(TEXT("."), ESearchCase::CaseSensitive, ESearchDir::FromStart);
	if (DotPos != INDEX_NONE)
	{
		return Filename.Mid(DotPos + (bIncludeDot ? 0 : 1));
	}
	return TEXT("");
}

FString URaiderzXmlUtilities::GetRaiderzBaseFileName(const FString& FilePath)
{
	const FString Filename = FPaths::GetCleanFilename(FilePath);
	int32 ExtPos = Filename.Find(TEXT("."), ESearchCase::CaseSensitive, ESearchDir::FromStart);
	if (ExtPos != INDEX_NONE)
	{
		return Filename.Left(ExtPos);
	}
	return TEXT("");
}

TArray<FXmlNode*> URaiderzXmlUtilities::GetNodesWithTag(FXmlNode* BaseNode, const FString& Tag)
{
	if (!BaseNode)
	{
		return TArray<FXmlNode*>();
	}

	TArray<FXmlNode*> ResultNodes;
	if (BaseNode->GetTag() == Tag)
	{
		ResultNodes.Add(BaseNode);
	}

	TArray<FXmlNode*> ChildrenNodes = BaseNode->GetChildrenNodes();
	for (FXmlNode* ChildNode : ChildrenNodes)
	{
		TArray<FXmlNode*> ResultingChildNodes = GetNodesWithTag(ChildNode, Tag);
		ResultNodes.Append(ResultingChildNodes);
	}
	return ResultNodes;
}

bool URaiderzXmlUtilities::ReadStringFromBinaryData(FString& StringBuffer, const TArray<uint8>& BinaryData, UINT& Offset)
{
	int StringLength;
	if (!WriteBinaryDataToBuffer(&StringLength, sizeof(StringLength), BinaryData, Offset))
	{
		return false;
	}

	// Check if there are enough bytes to write to Buffer.
	// Binary.Num() was cast to UINT to prevent type mismatch error for signed/unsigned
	if ((UINT)BinaryData.Num() < StringLength + Offset)
	{
		return false;
	}

	const uint8* BinaryDataPtr = BinaryData.GetData();
	char* String = new char[StringLength + 1];
	memcpy(String, BinaryDataPtr + Offset, StringLength);
	StringBuffer = FString(ANSI_TO_TCHAR(String));
	Offset += StringLength;
	delete[] String;

	return true;
}

bool URaiderzXmlUtilities::WriteBinaryDataToBuffer(void* Buffer, const UINT NumOfBytesToRead, const TArray<uint8>& BinaryData, UINT& Offset)
{
	// If there are no bytes to read
	if (NumOfBytesToRead == 0)
	{
		// @todo Should it return false since no binary data was written to buffer?
		return true;
	}

	// Check if there are enough bytes to write to Buffer.
	// Binary.Num() was cast to UINT to prevent type mismatch error for signed/unsigned
	if ((UINT)BinaryData.Num() < NumOfBytesToRead + Offset)
	{
		return false;
	}

	const uint8* BinaryDataPtr = BinaryData.GetData();
	memcpy(Buffer, BinaryDataPtr + Offset, NumOfBytesToRead);
	Offset += NumOfBytesToRead;
	return true;
}
