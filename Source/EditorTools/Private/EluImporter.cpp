// Copyright 2018 Moikkai Games. All Rights Reserved.


#include "EluImporter.h"
#include "EOD.h"
#include "RaiderzXmlUtilities.h"

#include "PackageTools.h"
#include "MeshUtilities.h"
#include "Editor.h"
#include "RawMesh.h"
#include "AssetRegistryModule.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "UObject/Package.h"
#include "PackageTools.h"
#include "Misc/PackageName.h"
#include "Misc/FileHelper.h"
#include "Animation/Skeleton.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"


UEluImporter::UEluImporter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UEluImporter::ImportEluStaticMesh()
{
	FString EluFile;
	bool bSuccess = PickEluFile(EluFile);
	if (!bSuccess)
	{
		return;
	}

	bool bImportSuccess = ImportEluStaticMesh_Internal(EluFile);
}

void UEluImporter::ImportEluSkeletalMesh()
{
	FString EluFile;
	bool bSuccess = PickEluFile(EluFile);
	if (!bSuccess)
	{
		return;
	}

	bool bImportSuccess = ImportEluSkeletalMesh_Internal(EluFile);
}

bool UEluImporter::PickEluFile(FString& OutFilePath)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	bool bSuccess = false;
	TArray<FString> SelectedFiles;
	if (DesktopPlatform)
	{
		const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
		bSuccess = DesktopPlatform->OpenFileDialog(
			ParentWindowWindowHandle,
			TEXT("Select FBX file to import"),
			URaiderzXmlUtilities::DataFolderPath,
			TEXT(""),
			TEXT("RaiderZ files|*.elu"),
			EFileDialogFlags::None,
			SelectedFiles
		);
	}

	if (!bSuccess || SelectedFiles.Num() == 0)
	{
		PrintWarning(TEXT("User failed to select any elu file!"));
		return false;
	}

	OutFilePath = SelectedFiles[0];
	return true;
}

FEluFileData UEluImporter::LoadEluData(const FString& EluFilePath)
{
	FEluFileData EluData;

	TArray<uint8> BinaryData;
	bool bSuccess = FFileHelper::LoadFileToArray(BinaryData, *EluFilePath);
	if (!bSuccess)
	{
		EluData.bLoadSuccess = false;
		return EluData;
	}

	UINT Offset = 0;
	FEluHeader EluHeader;
	URaiderzXmlUtilities::WriteBinaryDataToBuffer(&EluHeader, sizeof(EluHeader), BinaryData, Offset);

	if (EluHeader.Signature != EXPORTER_SIG)
	{
		FString LogMessage = TEXT("Failed to verify elu signatures for file: ") + FPaths::GetCleanFilename(EluFilePath); +TEXT(". File signature: ") +
			FString::FromInt(EluHeader.Signature) + TEXT(", expected signature: ") + FString::FromInt(EXPORTER_SIG);
		PrintError(LogMessage);

		EluData.bLoadSuccess = false;
		return EluData;
	}

	if (EluHeader.Version != EXPORTER_CURRENT_MESH_VER)
	{
		FString LogMessage = TEXT("File '") + FPaths::GetCleanFilename(EluFilePath) + TEXT("' is not the latest file version.");
		PrintWarning(LogMessage);
	}

	FEluMeshNodeLoader_v12 EluMeshNodeLoaderObj_v12;
	FEluMeshNodeLoader_v13 EluMeshNodeLoaderObj_v13;
	FEluMeshNodeLoader_v14 EluMeshNodeLoaderObj_v14;
	FEluMeshNodeLoader_v15 EluMeshNodeLoaderObj_v15;
	FEluMeshNodeLoader_v16 EluMeshNodeLoaderObj_v16;
	FEluMeshNodeLoader_v17 EluMeshNodeLoaderObj_v17;
	FEluMeshNodeLoader_v18 EluMeshNodeLoaderObj_v18;
	FEluMeshNodeLoader_v20 EluMeshNodeLoaderObj_v20;

	FEluMeshNodeLoader* EluMeshNodeLoader = nullptr;
	if (EluHeader.Version == EXPORTER_MESH_VER20)
	{
		EluMeshNodeLoader = &EluMeshNodeLoaderObj_v20;
		EluMeshNodeLoader->CurrentEluVersion = EXPORTER_MESH_VER20;
	}
	else if (EluHeader.Version == EXPORTER_MESH_VER18)
	{
		EluMeshNodeLoader = &EluMeshNodeLoaderObj_v18;
		EluMeshNodeLoader->CurrentEluVersion = EXPORTER_MESH_VER18;
	}
	else if (EluHeader.Version == EXPORTER_MESH_VER17)
	{
		EluMeshNodeLoader = &EluMeshNodeLoaderObj_v17;
		EluMeshNodeLoader->CurrentEluVersion = EXPORTER_MESH_VER17;
	}
	else if (EluHeader.Version == EXPORTER_MESH_VER16)
	{
		EluMeshNodeLoader = &EluMeshNodeLoaderObj_v16;
		EluMeshNodeLoader->CurrentEluVersion = EXPORTER_MESH_VER16;
	}
	else if (EluHeader.Version == EXPORTER_MESH_VER15)
	{
		EluMeshNodeLoader = &EluMeshNodeLoaderObj_v15;
		EluMeshNodeLoader->CurrentEluVersion = EXPORTER_MESH_VER15;
	}
	else if (EluHeader.Version == EXPORTER_MESH_VER14)
	{
		EluMeshNodeLoader = &EluMeshNodeLoaderObj_v14;
		EluMeshNodeLoader->CurrentEluVersion = EXPORTER_MESH_VER14;
	}
	else if (EluHeader.Version == EXPORTER_MESH_VER13)
	{
		EluMeshNodeLoader = &EluMeshNodeLoaderObj_v13;
		EluMeshNodeLoader->CurrentEluVersion = EXPORTER_MESH_VER13;
	}
	else if (EluHeader.Version <= EXPORTER_MESH_VER12)
	{
		EluMeshNodeLoader = &EluMeshNodeLoaderObj_v12;
		EluMeshNodeLoader->CurrentEluVersion = EXPORTER_MESH_VER12;
	}
	else
	{
		FString LogMessage = TEXT("elu version of file '") + FPaths::GetCleanFilename(EluFilePath) + TEXT("' is not supported");
		PrintError(LogMessage);

		EluData.bLoadSuccess = false;
		return EluData;
	}

	TArray<TSharedPtr<FEluMeshNode>> EluMeshNodes;
	for (int i = 0; i < EluHeader.MeshNum; i++)
	{
		TSharedPtr<FEluMeshNode> EluMeshNode(new FEluMeshNode());
		bool bLoadSuccessful = EluMeshNodeLoader->Load(EluMeshNode, BinaryData, Offset);

		if (bLoadSuccessful)
		{
			EluMeshNodes.Add(EluMeshNode);
		}
	}

	EluData.bLoadSuccess = true;
	EluData.EluHeader = EluHeader;
	EluData.EluMeshNodes = EluMeshNodes;

	return EluData;
}

bool UEluImporter::ImportEluStaticMesh_Internal(const FString& EluFilePath)
{
	FEluFileData EluData = LoadEluData(EluFilePath);
	if (!EluData.bLoadSuccess)
	{
		return false;
	}

	const TArray<TSharedPtr<FEluMeshNode>>& EluMeshNodes = EluData.EluMeshNodes;

	/*
	int32 NodeNum = EluMeshNodes.Num();
	for (int i = 0; i < NodeNum; i++)
	{
		TSharedPtr<FEluMeshNode> MeshNode = EluMeshNodes[i];
		if (MeshNode.IsValid())
		{
			FString LogMessage = TEXT("Node name: ") + MeshNode->NodeName + TEXT(", LOD index:  ") + FString::FromInt(MeshNode->LODProjectIndex);
			PrintWarning(LogMessage);
		}
	}
	*/

	FString PackageName = FString("/Game/RaiderZ/Zunk/WAKA");
	bool bPackageExists = FPackageName::DoesPackageExist(PackageName);

	// If package doesn't exist, it's safe to create new package
	PackageName = PackageTools::SanitizePackageName(PackageName);
	UPackage* Package = CreatePackage(nullptr, *PackageName);
	Package->FullyLoad();

	if (!bPackageExists && EluMeshNodes.Num() != 0)
	{
		UStaticMesh* StaticMesh = NewObject<UStaticMesh>(Package, UStaticMesh::StaticClass(), *FString("WAKA"), EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
		StaticMesh->AddSourceModel();
		StaticMesh->LightingGuid = FGuid::NewGuid();
		StaticMesh->LightMapCoordinateIndex = 1;
		StaticMesh->LightMapResolution = 64;

		FRawMesh RawMesh;
		int32 PointsOffset = 0;

		int32 NodeNum = EluMeshNodes.Num();
		for (int i = 0; i < NodeNum; i++)
		{
			TSharedPtr<FEluMeshNode> MeshNode = EluMeshNodes[i];
			check(MeshNode.IsValid());

			if (MeshNode->LODProjectIndex != 0 || MeshNode->NodeName.Contains("hide") || MeshNode->PointsTable.Num() == 0)
			{
				continue;
			}

			// FString LogMessage = TEXT("Processing node: ") + MeshNode->NodeName;
			// PrintWarning(LogMessage);
			// LogMessage = TEXT("Local matrix: \n") + MeshNode->LocalMatrix.ToString();
			// PrintWarning(LogMessage);

			PointsOffset = RawMesh.VertexPositions.Num();
			// RawMesh.VertexPositions += MeshNode->PointsTable;

			for (const FVector& Point : MeshNode->PointsTable)
			{
				const FVector& TransformedPoint = MeshNode->LocalMatrix.TransformPosition(Point);
				RawMesh.VertexPositions.Add(TransformedPoint);
			}

			int32 PolyNum = MeshNode->PolygonTable.Num();
			for (int j = PolyNum - 1; j >= 0; j--)
			{
				const FMeshPolygonData& PolyData = MeshNode->PolygonTable[j];

				RawMesh.FaceMaterialIndices.Add(PolyData.MaterialID);
				RawMesh.FaceSmoothingMasks.Add(1);

				int32 SubNum = PolyData.FaceSubDatas.Num();
				for (int k = SubNum - 1; k >= 0; k--)
				{
					const FFaceSubData& FaceData = PolyData.FaceSubDatas[k];

					RawMesh.WedgeIndices.Add(PointsOffset + FaceData.p);

					if (MeshNode->TangentTanTable.Num() > FaceData.n_tan)
					{
						const FVector& Tangent_X = MeshNode->TangentTanTable[FaceData.n_tan];
						RawMesh.WedgeTangentX.Add(Tangent_X);
					}
					else
					{
						RawMesh.WedgeTangentX.Add(FVector::ZeroVector);
					}


					if (MeshNode->TangentBinTable.Num() > FaceData.n_bin)
					{
						const FVector& Tangent_Y = MeshNode->TangentBinTable[FaceData.n_bin];
						RawMesh.WedgeTangentY.Add(Tangent_Y);
					}
					else
					{
						RawMesh.WedgeTangentY.Add(FVector::ZeroVector);
					}

					if (MeshNode->NormalsTable.Num() > FaceData.n)
					{
						const FVector& Normal = MeshNode->NormalsTable[FaceData.n];
						RawMesh.WedgeTangentZ.Add(-Normal);
					}
					else
					{
						RawMesh.WedgeTangentZ.Add(FVector::ZeroVector);
					}

					RawMesh.WedgeColors.Add(FColor(0, 0, 0));

					if (MeshNode->TexCoordTable.Num() > FaceData.uv)
					{
						FVector TexCoord = MeshNode->TexCoordTable[FaceData.uv];
						RawMesh.WedgeTexCoords[0].Add(FVector2D(TexCoord.X, TexCoord.Y));
						RawMesh.WedgeTexCoords[1].Add(FVector2D(0, 0));
					}
					else
					{
						RawMesh.WedgeTexCoords[0].Add(FVector2D(0, 0));
						RawMesh.WedgeTexCoords[1].Add(FVector2D(0, 0));
					}


					// if (MeshNode->PointColorTable.Num() > FaceData.)

				}
			}
		}

		StaticMesh->SourceModels[0].SaveRawMesh(RawMesh);

		TArray<FText> ErrorText;
		StaticMesh->Build(false, &ErrorText);
		StaticMesh->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(StaticMesh);
	}

	/*
	IMeshUtilities& MeshUtilities = FModuleManager::Get().LoadModuleChecked<IMeshUtilities>("MeshUtilities");
	TArray<FVector2D> OutUniqueUVs;
	bool bResult = MeshUtilities.GenerateUniqueUVsForStaticMesh(RawMesh, 1024, OutUniqueUVs);
	if (bResult)
	{
		PrintWarning("Unique UV generation succeeded!");
	}
	else
	{
		PrintError("Unique UV generation failed!");
	}

	RawMesh.WedgeTexCoords[0] = OutUniqueUVs;
	*/

	//~ @todo check UnFbx::FFbxImporter::BuildStaticMeshFromGeometry

	// Unselect all actors.
	// GEditor->SelectNone(false, false);
	// GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, Class, InParent, Name, Type);

	// FbxImporter->ImportFromFile(FbxImportFileName, Type, true)
	// ImportAllSkeletalMesh(RootNodeToImport, FbxImporter, Flags, NodeIndex, InterestingNodeCount, SceneInfoPtr);
	// ImportAllStaticMesh(RootNodeToImport, FbxImporter, Flags, NodeIndex, InterestingNodeCount, SceneInfoPtr);


	return true;
}

bool UEluImporter::ImportEluSkeletalMesh_Internal(const FString& EluFilePath)
{
	FEluFileData EluData = LoadEluData(EluFilePath);
	if (!EluData.bLoadSuccess)
	{
		return false;
	}

	const TArray<TSharedPtr<FEluMeshNode>>& EluMeshNodes = EluData.EluMeshNodes;
	FString SkelPackName = FString("/Game/RaiderZ/Zunk/Skel");
	SkelPackName = PackageTools::SanitizePackageName(SkelPackName);
	UPackage* SkelPack = CreatePackage(nullptr, *SkelPackName);
	SkelPack->FullyLoad();

	USkeleton* Skel = NewObject<USkeleton>(SkelPack, *FString("Skel"), EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
	const FReferenceSkeleton& RefSkel = Skel->GetReferenceSkeleton();
	// horrible hack to modify the skeleton in place
	FReferenceSkeletonModifier SkelMod((FReferenceSkeleton&)RefSkel, Skel);

	SkelMod.Add(FMeshBoneInfo(TEXT("root_bone"), TEXT("root_bone"), INDEX_NONE), FTransform());

	for (int i = 0; i < EluMeshNodes.Num(); i++)
	{
		TSharedPtr<FEluMeshNode> MeshNode = EluMeshNodes[i];
		FTransform Transform(MeshNode->LocalMatrix);
		FString SanitizedNodeName = UPackageTools::SanitizePackageName(MeshNode->NodeName);
		SkelMod.Add(FMeshBoneInfo(FName(*SanitizedNodeName), SanitizedNodeName, MeshNode->ParentNodeID + 1), Transform);
	}


	Skel->MarkPackageDirty();
	FAssetRegistryModule::AssetCreated(Skel);



	FString PackageName = FString("/Game/RaiderZ/Zunk/SkelMesh");
	bool bPackageExists = FPackageName::DoesPackageExist(PackageName);

	// If package doesn't exist, it's safe to create new package
	PackageName = PackageTools::SanitizePackageName(PackageName);
	UPackage* Package = CreatePackage(nullptr, *PackageName);
	Package->FullyLoad();

	USkeletalMesh* SkeletalMesh = NewObject<USkeletalMesh>(Package, USkeletalMesh::StaticClass(), *FString("WAKA"), EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
	check(SkeletalMesh);

	FSkeletalMeshModel* ImportedModel = SkeletalMesh->GetImportedModel();
	SkeletalMesh->PreEditChange(nullptr);

	ImportedModel->LODModels.Add(new FSkeletalMeshLODModel());
	SkeletalMesh->AddLODInfo();

	FSkeletalMeshLODModel& LODModel = ImportedModel->LODModels[0];

	SkeletalMesh->GetLODInfo(0)->LODHysteresis = 0.02;

	FSkeletalMeshOptimizationSettings Settings;
	SkeletalMesh->GetLODInfo(0)->ReductionSettings = Settings;


	LODModel.NumTexCoords = 1;

	TArray<FSoftSkinVertex> SoftVertices;
	TArray<FVector> Points;
	TArray<SkeletalMeshImportData::FMeshWedge> Wedges;
	TArray<SkeletalMeshImportData::FMeshFace> Faces;
	TArray<SkeletalMeshImportData::FVertInfluence> Influences;

	TArray<int32> PointsMap;
	TArray<FVector> TangentX;
	TArray<FVector> TangentY;
	TArray<FVector> TangentZ;
	TArray<uint16> MatIndices;
	TArray<uint32> SmoothingGroups;




	// FSoftSkinVertex SkinVertex;




	// if (!(Skel->GetReferenceSkeleton().FindBoneIndex(FName("RootBone")) > -1))
	{
		// SkelMod.Add(FMeshBoneInfo(TEXT("RootBone"), TEXT("RootBone"), INDEX_NONE), FTransform());
	}

	return true;
}
