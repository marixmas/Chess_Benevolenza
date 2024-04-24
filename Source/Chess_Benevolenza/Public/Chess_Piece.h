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

	// get the (x, y) position
	FVector2D GetGridPosition();					// avevo messo come argomento AChess_Piece* Piece ma non credo sia necessario basta fare puntatoreAoggettoPiece->GetGridPosition()

	//void SetTileStatus(ETileStatus TileStatus);					//no?!

	void SetPieceType(EPieceType NewPieceType);
	//EPieceType GetPieceType();

	void SetPieceColor(EPieceColor NewPieceColor);
	EPieceColor GetPieceColor();

	// per usare le classi derivate
	virtual EPieceType GetPieceType() const;

	// Dichiarazione della funzione virtuale pura
    virtual void SpecialAction() const PURE_VIRTUAL(AChess_Piece::SpecialAction, );




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// (x, y) position of the tile									//messa per essere usata in GenerateField() in GameField
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D TileGridPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPieceColor PieceColor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPieceType PieceType;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// calcola possibili mosse per ogni tipo di pezzo
	virtual TArray<FVector2D> CalculatePossibleMoves();

	// nuove posizioni possibili delle pedine
	TArray<FVector2D> PossibleMoves;

	// destroy a Chess Piece actor
	UFUNCTION()
	void SelfDestroy();



};
