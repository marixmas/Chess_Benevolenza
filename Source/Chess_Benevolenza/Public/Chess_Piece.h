// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chess_PlayerInterface.h"														
#include "Chess_Piece.generated.h"

UENUM()
enum class EPieceColor : uint8
{
	WHITE,
	BLACK,
	NOCOLOR,

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

	NOTYPE,
};


UCLASS()
class CHESS_BENEVOLENZA_API AChess_Piece : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AChess_Piece();

	// set the (x, y) position
	void SetGridPosition(const int32 InX, const int32 InY);

	// get the (x, y) position
	FVector2D GetGridPosition();					

	// set the type
	void SetPieceType(EPieceType NewPieceType);

	// get the type
	/*virtual*/ EPieceType GetPieceType() const;

	// set the color
	void SetPieceColor(EPieceColor NewPieceColor);

	// get the color
	EPieceColor GetPieceColor();

	void MovePieceFromToPosition(const FVector2D OldPosition, const FVector2D NewPosition);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// (x, y) position of the tile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D PieceGridPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPieceColor PieceColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPieceType PieceType;



public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// calcola possibili mosse per ogni tipo di pezzo
	virtual TArray<FVector2D> CalculatePossibleMoves();
	virtual bool IsMoveValid(const FVector2D& Move);
	//virtual bool IsAttackValid(const FVector2D& Attack);


	// nuove posizioni possibili delle pedine
	TArray<FVector2D> PossibleMoves;

	// chiamata quando un pezzo viene mangiato
	void PieceIsEaten(FVector2D& EatenPiecePosition);

	// destroy a Chess Piece actor
	UFUNCTION()
	void SelfDestroy();



};
