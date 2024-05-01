// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Chess_GameInstance.h"
#include "Chess_PlayerInterface.h"
#include "Chess_Piece.h"						// mi serve per InfoOfClickedPiece 
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"				// mi serve nel corrispettivo file cpp e forse anche in questo stesso file... vediamo
#include "Chess_HumanPlayer.generated.h"

UCLASS()
class CHESS_BENEVOLENZA_API AChess_HumanPlayer : public APawn, public IChess_PlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AChess_HumanPlayer();

	// camera component attached to player pawn
	UCameraComponent* Camera;

	// game instance reference
	UChess_GameInstance* GameInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// keeps track of turn
	bool IsMyTurn;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	virtual void OnTurn() override;
	virtual void OnWin() override;
	virtual void OnLose() override;

	// called on left mouse click (binding)
	UFUNCTION()
	void OnClick();

	// stampa a screen un debug message con le info del pezzo selezionato
	void InfoOfClickedPiece (AChess_Piece* CurrentPiece);

	// Accende le tile dei suggerimenti in giallo
	void HighlightGameFieldTiles(const TArray<FVector2D>& TilePositions);						// prima c'era anche un secondo argomento AChess_GameMode* GameMode, AGameField* GField

	// Accende la tile dello scacco in rosso
	void HighlightInRedGameFieldTiles(int32 PlayerKing); 

	// Spegne le tile illuminate dei suggerimenti
	void TurnOffHighlightedTiles();

	// Spegne le tile illuminate in rosso dello scacco
	void TurnOffRedHighlightedTiles();

	AChess_Piece* SelectedWhitePiece;

	FVector2D WhitePieceLocation;

	bool bPieceSelected;

	AChess_Piece* SelectedBlackPiece;

	FVector2D BlackPieceLocation;

	TArray<FVector2D> PossibleMoves;

	//FVector TransformTileToWorld(const FVector2D& TilePosition);

	//bool HumanPlayerIsInCheck;

protected:
	// muove il pezzo selezionato alla nuova posizione
	void MoveSelectedPiece(const FVector2D& OldPosition, const FVector2D& NewPosition);



};
