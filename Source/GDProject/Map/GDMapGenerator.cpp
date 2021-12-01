// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDMapGenerator.h"

#include "GDPoIBattle.h"
#include "GDPoIBonfire.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "GDProject/Tiles/GDTileLowland.h"
#include "Math/UnrealMathUtility.h"

#define BASE_POI 1
#define BONFIRE_POI 2
#define BATTLE_0_POI 3
#define BATTLE_1_POI 4
#define BATTLE_2_POI 5



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
			AGDPointOfInterest* NewPOI;
			PoINode* NewNode;
			UE_LOG(LogTemp, Display, TEXT("Generate POI %i"), Map[i][j])
			if (Map[i][j] == 0) continue;
			switch (Map[i][j])
			{
				case BATTLE_0_POI:
					UE_LOG(LogTemp, Display, TEXT("BATTLE 0 loc %f - %f"), POI_Location.X, POI_Location.Y)
                    NewPOI = GetWorld()->SpawnActor<AGDPointOfInterest>(Battle_0_POI, POI_Location, FRotator(0, 0, 0));
                	PoIList.Add(NewNode);
                	POI_Location.Y += NodeHorizontalDistance;
					break;
				case BATTLE_1_POI:
					UE_LOG(LogTemp, Display, TEXT("BATTLE 1 loc %f - %f"), POI_Location.X, POI_Location.Y)
					NewPOI = GetWorld()->SpawnActor<AGDPointOfInterest>(Battle_1_POI, POI_Location, FRotator(0, 0, 0));
					POI_Location.Y += NodeHorizontalDistance;
					break;
				case BATTLE_2_POI:
					UE_LOG(LogTemp, Display, TEXT("BATTLE 2 loc %f - %f"), POI_Location.X, POI_Location.Y)
					NewPOI = GetWorld()->SpawnActor<AGDPointOfInterest>(Battle_2_POI, POI_Location, FRotator(0, 0, 0));
					POI_Location.Y += NodeHorizontalDistance;
				break;
				case BONFIRE_POI:
					UE_LOG(LogTemp, Display, TEXT("BONFIRE loc %f - %f"), POI_Location.X, POI_Location.Y)
					NewPOI = GetWorld()->SpawnActor<AGDPointOfInterest>(BonfirePOI, POI_Location, FRotator(0, 0, 0));
					POI_Location.Y += NodeHorizontalDistance;
				break;
				default:
					UE_LOG(LogTemp, Display, TEXT("BASE loc %f - %f"), POI_Location.X, POI_Location.Y)
					NewPOI = GetWorld()->SpawnActor<AGDPointOfInterest>(BasePOI, POI_Location, FRotator(0, 0, 0));
					POI_Location.Y += NodeHorizontalDistance;
				break;
			}
			NewNode = new PoINode(NewPOI, POI_Location, i);
			PoIList.Add(NewNode);
		}
		POI_Location.X += NodeVerticallDistance;
	}
	POI_Location.X -= NodeVerticallDistance;
	POI_Location.Y = 0; 
	AGDPointOfInterest* BossPoint = GetWorld()->SpawnActor<AGDPointOfInterest>(Battle_2_POI, POI_Location, FRotator(0, 0, 0));
	PoINode* BossNode = new PoINode(BossPoint, POI_Location, 0);
	PoIList.Add(BossNode);
}


void AGDMapGenerator::GenerateMapScheme()
{
	TArray<int> SampleRow;
	SampleRow.Init(0, MaxNodeLevel + 1);
	Map.Init(SampleRow, Levels+1);
	Map[0][0] = BASE_POI;
	int RestoreCount = 3;
	
	for (int i = 1; i < Levels+1; i++)
	{
		int levelPoI = FMath::RandRange(2, MaxNodeLevel);
		int Hard_PoI = 1;
		int Medium_PoI = 1;

		for (int j = 0; j < levelPoI; j++)
		{
			int BattlePOIDiff = FMath::RandRange(BATTLE_0_POI, BATTLE_2_POI);
			if (BattlePOIDiff == BATTLE_2_POI)
			{
				if(Hard_PoI != 0)
				{
					Hard_PoI--;
				}
				else if(Medium_PoI != 0)
				{
					BattlePOIDiff = BATTLE_1_POI;
					Medium_PoI--;
				}
				else
				{
					BattlePOIDiff = BATTLE_0_POI;
				}
			}
			else if (BattlePOIDiff == BATTLE_1_POI && Medium_PoI)
			{
				Medium_PoI--;
			}
			else
			{
				BattlePOIDiff = BATTLE_0_POI;
			}
			Map[i][j] = BattlePOIDiff;
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

