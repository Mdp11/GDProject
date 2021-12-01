// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "GDPointOfInterest.h"


// Sets default values
AGDPointOfInterest::AGDPointOfInterest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGDPointOfInterest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGDPointOfInterest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AGDPointOfInterest::AddNode(AGDPointOfInterest* NewNode)
{
	if(AdjList.Find(NewNode) != NULL)
	{
		AdjList.Add(NewNode);
		return true;
	}
	return false;
}

