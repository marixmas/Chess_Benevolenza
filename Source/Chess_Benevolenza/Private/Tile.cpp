// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"

// Sets default values
ATile::ATile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//lo metto false poiché aggiornamento della tile è inutile
	PrimaryActorTick.bCanEverTick = false;

	// template function that creates a components
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// every actor has a RootComponent that defines the transform in the World
	SetRootComponent(Scene);
	StaticMeshComponent->SetupAttachment(Scene);

	Status = ETileStatus::EMPTY;
	PlayerOwner = -1;
	TileGridPosition = FVector2D(0, 0);


}

void ATile::SetTileStatus(const ETileStatus TileStatus)
{
	Status = TileStatus;
}

void ATile::SetTileOwner(const int32 TileOwner)
{
	PlayerOwner = TileOwner;
}

ETileStatus ATile::GetTileStatus() const
{
	return Status;
}

int32 ATile::GetOwner() const
{
	return PlayerOwner;
}

void ATile::SetGridPosition(const double InX, const double InY)
{
	TileGridPosition.Set(InX, InY);
}

FVector2D ATile::GetGridPosition()
{
	return TileGridPosition;
}


// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

}

/* non serve fare tick
Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/
