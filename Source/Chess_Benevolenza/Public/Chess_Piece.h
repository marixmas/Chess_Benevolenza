// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chess_Piece.generated.h"

UENUM()
enum class EPieceColor : uint8
{
	WHITE,
	BLACK,
	EMPTY,
};

UENUM()
enum class EPieceType : uint8
{
	ROOK		UMETA(DisplayName = "Rook"),
	KNIGHT      UMETA(DisplayName = "Knight"),
	BISHOP      UMETA(DisplayName = "Bishop"),
	QUEEN       UMETA(DisplayName = "Queen"),
	KING        UMETA(DisplayName = "King"),
	PAWN        UMETA(DisplayName = "Pawn"),
};


UCLASS()
class CHESS_BENEVOLENZA_API AChess_Piece : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AChess_Piece();

	// set the (x, y) position										//messa per essere usata in GenerateField() in GameField
	void SetGridPosition(const double InX, const double InY);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// (x, y) position of the tile									//messa per essere usata in GenerateField() in GameField
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D TileGridPosition;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// destroy a Chess Piece actor
	UFUNCTION()
	void SelfDestroy();
};
