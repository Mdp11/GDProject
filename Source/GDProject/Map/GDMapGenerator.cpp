// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDMapGenerator.h"

#include "GDPoIBattle.h"
#include "GDPoIBonfire.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "GDProject/Tiles/GDTileLowland.h"
#include "Math/UnrealMathUtility.h"

#define BASE_POI 1
#define BATTLE_POI 2
#define BONFIRE_POI 3



// Sets default values
AGDMapGenerator::AGDMapGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGDMapGenerator::BeginPlay()
{
	Super::BeginPlay();

	GenerateMapScheme();

	FVector POI_Location = FVector(0, 0, 0);

	UE_LOG(LogTemp, Display, TEXT("PoI loc %f - %f"), POI_Location.X, POI_Location.Y)
	AGDPointOfInterest* StartPoint = GetWorld()->SpawnActor<AGDPointOfInterest>(BasePOI, POI_Location, FRotator(0, 0, 0));
	PoINode* StartNode = new PoINode(StartPoint, POI_Location, 0);
	PoIList.Add(StartNode);

	POI_Location.X += NodeVerticallDistance;
	
	for (int i = 1; i < Levels; i++)
	{
		auto level = Map[i].FilterByPredicate([](const int& value){
			return  value != 0;
		});
		if (level.Num() % 2 == 0)
		{
			if (level.Num() == 2)
			{
				POI_Location.Y =- (NodeHorizontalDistance/2);
			} else
			{
				POI_Location.Y =- (((level.Num()-1)*NodeHorizontalDistance)/2);
                UE_LOG(LogTemp, Display, TEXT("PARI  %f"), POI_Location.Y)
			}
		} else
		{
			POI_Location.Y =- (((level.Num() - 1) *NodeHorizontalDistance)/2);
			UE_LOG(LogTemp, Display, TEXT("DISPARI  %f"), POI_Location.Y)
		}
		UE_LOG(LogTemp, Display, TEXT("Generate lvl %i"), i)
		for (int j = 0; j < MaxNodeLevel; j++)
		{
			if (Map[i][j] == BATTLE_POI)
			{
				UE_LOG(LogTemp, Display, TEXT("BATTLE loc %f - %f"), POI_Location.X, POI_Location.Y)
	            AGDPointOfInterest* NewPOI = GetWorld()->SpawnActor<AGDPointOfInterest>(BattlePOI, POI_Location, FRotator(0, 0, 0));
				PoINode* NewNode = new PoINode(NewPOI, POI_Location, i);
				PoIList.Add(NewNode);
				POI_Location.Y += NodeHorizontalDistance;
			}
			else if (Map[i][j] == BONFIRE_POI)
			{
				UE_LOG(LogTemp, Display, TEXT("BONFIRE loc %f - %f"), POI_Location.X, POI_Location.Y)
				AGDPointOfInterest* NewPOI = GetWorld()->SpawnActor<AGDPointOfInterest>(BonfirePOI, POI_Location, FRotator(0, 0, 0));
				PoINode* NewNode = new PoINode(NewPOI, POI_Location, i);
				PoIList.Add(NewNode);
				POI_Location.Y += NodeHorizontalDistance;
			} else if (Map[i][j] == BASE_POI)
			{
				UE_LOG(LogTemp, Display, TEXT("PoI loc %f - %f"), POI_Location.X, POI_Location.Y)
				AGDPointOfInterest* NewPOI = GetWorld()->SpawnActor<AGDPointOfInterest>(BasePOI, POI_Location, FRotator(0, 0, 0));
				PoINode* NewNode = new PoINode(NewPOI, POI_Location, i);
				PoIList.Add(NewNode);
				POI_Location.Y += NodeHorizontalDistance;
			}
		}
		POI_Location.X += NodeVerticallDistance;
	}
}


void AGDMapGenerator::GenerateMapScheme()
{
	TArray<int> SampleRow;
	SampleRow.Init(0, MaxNodeLevel + 1);
	Map.Init(SampleRow, Levels+1);
	Map[0][0] = BASE_POI;
	int RestoreCount = 3;
	
	for (int i = 1; i < Levels; i++)
	{

		int levelPoI = FMath::RandRange(2, MaxNodeLevel);
		for (int j = 0; j < levelPoI; j++)
		{
			Map[i][j] = BATTLE_POI;
		}
		if(--RestoreCount == 0)
        {
			Map[i][levelPoI-1] = BONFIRE_POI;
			RestoreCount = 3;
        }
	}
}

int AGDMapGenerator::SetPOIType()
{
	return 0;
}

// Called every frame
void AGDMapGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

