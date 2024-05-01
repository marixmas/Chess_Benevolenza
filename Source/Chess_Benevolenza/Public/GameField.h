// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "Chess_Piece.h"
#include "Chess_Piece_Queen.h"	
#include "Chess_Piece_King.h"	
#include "Chess_Piece_Pawn.h"	
#include "Chess_Piece_Knight.h"	
#include "Chess_Piece_Bishop.h"	
#include "Chess_Piece_Rook.h"	
#include "GameFramework/Actor.h"
#include "GameField.generated.h"

// macro declaration for a dynamic multicast delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReset);


UCLASS()
class CHESS_BENEVOLENZA_API AGameField : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGameField();

	// keeps track of tiles
	UPROPERTY(Transient)
	TArray<ATile*> TileArray;

	//given a position returns a tile
	UPROPERTY(Transient)
	TMap<FVector2D, ATile*> TileMap;

	// keeps track of pieces
	UPROPERTY(Transient)
	TArray<AChess_Piece*> PiecesArray;

	UPROPERTY(Transient)
	TArray<AChess_Piece*> WhitePiecesArray;

	UPROPERTY(Transient)
	TArray<AChess_Piece*> BlackPiecesArray;


	//given a position returns a piece
	UPROPERTY(Transient)
	TMap<FVector2D, AChess_Piece*> PiecesMap;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float NormalizedCellPadding;
	static const int32 NOT_ASSIGNED = -1;

	// BlueprintAssignable Usable with Multicast Delegates only. Exposes the property for assigning in Blueprints.
	// declare a variable of type FOnReset (delegate)
	UPROPERTY(BlueprintAssignable)
	FOnReset OnResetEvent;

	// size of field
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Size;
	
	// TSubclassOf template class that provides UClass type safety
	// non-colore
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ATile> TileClass;

	

	// Materiale per il colore 1
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* TileMaterial1;

	// Materiale per il colore 2
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* TileMaterial2;

	// Materiale per l'evidenziazione delle mosse (giallo)
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* TileHighlightMaterial;

	
	// tile padding dimension
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CellPadding;

	// tile size
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TileSize;


	//identificatori per lo spawn delle pedine
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChess_Piece> ChessPieceRookBpWhite;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChess_Piece> ChessPieceKnightBpWhite;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChess_Piece> ChessPieceBishopBpWhite;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChess_Piece> ChessPieceQueenBpWhite;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChess_Piece> ChessPieceKingBpWhite;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChess_Piece> ChessPiecePawnBpWhite;


	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChess_Piece> ChessPieceRookBpBlack;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChess_Piece> ChessPieceKnightBpBlack;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChess_Piece> ChessPieceBishopBpBlack;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChess_Piece> ChessPieceQueenBpBlack;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChess_Piece> ChessPieceKingBpBlack;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChess_Piece> ChessPiecePawnBpBlack;



	// generate an empty game field
	void GenerateField();

	//genera le pedine sulla scacchiera
	void GeneratePieces();

	// spawna il pezzo
	void SpawnChessPiece(int32 x, int32 y, EPieceColor PieceColor, EPieceType PieceType);

	// prende il blue print del pezzo
	UClass* GetChessPieceBP(EPieceColor PieceColor, EPieceType PieceType);

	// Called when an instance of this class is placed (in editor) or spawned
	virtual void OnConstruction(const FTransform& Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// remove all signs from the field
	UFUNCTION(BlueprintCallable)
	void ResetField();
	

	// promuove il pedone a Regina del suo colore  (quando arrivati dall'altra parte della scacchiera)
	void PromotionToQueen(AChess_Piece* PawnToPromote);


	// return a (x,y) position given a hit (click) on a field tile
	FVector2D GetPosition(const FHitResult& Hit);

	// return the array of tile pointers
	TArray<ATile*>& GetTileArray();

	// return the array of pieces pointers
	TArray<AChess_Piece*>& GetPiecesArray();

	// return the array of white pieces pointers
	TArray<AChess_Piece*>& GetWhitePiecesArray(); 

	// return the array of black pieces pointers
	TArray<AChess_Piece*>& GetBlackPiecesArray();

	// return copy of the array of pieces pointers
	TArray<AChess_Piece*> GetCopyOfPiecesArray();

	// return copy of the array of white pieces pointers
	TArray<AChess_Piece*> GetCopyOfWhitePiecesArray();

	// return copy of the array of black pieces pointers
	TArray<AChess_Piece*> GetCopyOfBlackPiecesArray();


	// return a relative position given (x,y) position
	FVector GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const;

	// return (x,y) position given a relative position
	FVector2D GetXYPositionByRelativeLocation(const FVector& Location) const;

	/*
	public:
		// Called every frame
		virtual void Tick(float DeltaTime) override;
	*/
	//
};

