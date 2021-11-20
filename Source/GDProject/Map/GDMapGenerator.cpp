// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDMapGenerator.h"

#include "BehaviorTree/BehaviorTreeTypes.h"
#include "GDProject/Tiles/GDTileLowland.h"
#include "Math/UnrealMathUtility.h"


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
	for (int i = 0; i < Levels; i++)
	{
		UE_LOG(LogTemp, Display, TEXT("Generate lvl %i"), i)
		for (int j = 0; j < MaxNodeLevel; j++)
		{
			
			if (Map[i][j] == 1)
			{
				UE_LOG(LogTemp, Display, TEXT("PoI loc %f - %f"), POI_Location.X, POI_Location.Y)
				AGDPointOfInterest* NewPOI;
	            NewPOI = GetWorld()->SpawnActor<AGDPointOfInterest>(BasePOI, POI_Location, FRotator(0, 0, 0));
				PoINode* NewNode = new PoINode(NewPOI, POI_Location, i);
				PoIList.Add(NewNode);
				POI_Location.Y += NodeHorizontalDistance;
			}
		}
		POI_Location.Y =-NodeHorizontalDistance;
		POI_Location.X += NodeVerticallDistance;
	}
}


void AGDMapGenerator::GenerateMapScheme()
{
	TArray<int> SampleRow;
	SampleRow.Init(0, MaxNodeLevel);
	MapScheme.Init(SampleRow, Levels+1);
	Map.Init(SampleRow, Levels+1);
	Map[0][0] = 1;
	for (int i = 1; i < Levels; i++)
	{
		
		int levelPoI = FMath::RandRange(2, MaxNodeLevel-1);
		for (int j = 0; j < levelPoI; j++)
		{
			Map[i][j] = 1;
		}
	}

	for (int i = 0; i < MaxNodeLevel; i++)
	{
		if (MapScheme[1][i] == 1)
		{
			MapScheme[0].Add(1);
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

