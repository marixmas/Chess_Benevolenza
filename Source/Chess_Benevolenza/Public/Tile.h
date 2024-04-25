// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chess_Piece.h"						// messo per  fare settilestatus() nella chess_piece
#include "Components/StaticMeshComponent.h"		// per cambiare materiale alla tile nella highlight
#include "Tile.generated.h"


UENUM()
enum class ETileStatus : uint8
{
	EMPTY     UMETA(DisplayName = "Empty"),
	OCCUPIED      UMETA(DisplayName = "Occupied"),
};


UCLASS()
class CHESS_BENEVOLENZA_API ATile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATile();
	
	// imposta il materiale della tile
	void SetTileMaterial(UMaterialInterface* Material);

	// imposta lo stato della tile
	void SetTileStatus(const ETileStatus TileStatus);

	// Returns true if the tile is occupied by a piece
	bool IsOccupied() const;

	// imposta l'owner della tile
	void SetTileOwner (const int32 TileOwner);

	// get the tile status
	ETileStatus GetTileStatus() const;

	// get the tile owner
	int32 GetOwner() const;

	// set the (x, y) position
	void SetGridPosition(const double InX, const double InY);

	// get the (x, y) position
	FVector2D GetGridPosition();

	// rende vuota una tile
	void EmptyTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ETileStatus Status;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 PlayerOwner;

	// (x, y) position of the tile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D TileGridPosition;


	// Tipo di Pezzo occupante
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//TSubclassOf<AChess_Piece> OccupyingPieceType;

	// Colore di Pezzo occupante
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//EChessPieceColor OccupyingPieceColor;


	//public:	
		// qui commento la tick perché non serve che aggiorni la tile poiché è ferma durante il gioco

		// Called every frame
		//virtual void Tick(float DeltaTime) override;

};