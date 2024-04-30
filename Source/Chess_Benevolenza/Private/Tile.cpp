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

void ATile::SetTileMaterial(UMaterialInterface* Material)
{
	if (StaticMeshComponent && Material)
	{
		StaticMeshComponent->SetMaterial(0, Material);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Componente StaticMeshComponent o materiale non validi (per fare SetTileMaterial)."));
	}
}

UMaterialInterface* ATile::GetTileMaterial()
{
	if (StaticMeshComponent)
	{
		return StaticMeshComponent->GetMaterial(0);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Componente StaticMeshComponent non valido per ottenere il materiale della tile."));
		return nullptr;
	}
}



void ATile::SetTileStatus(ETileStatus TileStatus)
{
	Status = TileStatus;
}

bool ATile::IsOccupied() const
{
	if (Status == ETileStatus::OCCUPIED)
	{
		return true;
	}

	else
	{
		return false;
	}
	

}

void ATile::SetTileOwner( int32 TileOwner)
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

void ATile::EmptyTile()
{
	SetTileStatus(ETileStatus::EMPTY);
	SetTileOwner(-1);
}

ATile* ATile::CloneTile()
{
	// creo una nuova istanza di ATile
	ATile* ClonedTile = GetWorld()->SpawnActor<ATile>(ATile::StaticClass());

	if (ClonedTile)
	{
		// copio le proprietà della tile
		ClonedTile->SetTileMaterial(StaticMeshComponent->GetMaterial(0)); // assumo un materiale a caso
		ClonedTile->SetTileStatus(Status);
		ClonedTile->SetTileOwner(PlayerOwner);
		ClonedTile->SetGridPosition(TileGridPosition.X, TileGridPosition.Y);
	}

	return ClonedTile;
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
