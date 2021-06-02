// Fill out your copyright notice in the Description page of Project Settings.


#include "Tiles/GDTile.h"

// Sets default values
AGDTile::AGDTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGDTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGDTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

