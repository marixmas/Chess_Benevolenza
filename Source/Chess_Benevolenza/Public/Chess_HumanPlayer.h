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
	bool IsMyTurn = false;


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

	// Accende le tile dei suggerimenti
	void HighlightGameFieldTiles(const TArray<FVector2D>& TilePositions);						// prima c'era anche un secondo argomento AChess_GameMode* GameMode, AGameField* GField

	// Spegne le tile illuminate dei suggerimenti
	void TurnOffHighlightedTiles(const TArray<FVector2D>& HighlightedPositions);

	AChess_Piece* SelectedWhitePiece;

	bool bPieceSelected;

	AChess_Piece* SelectedBlackPiece;

	FVector2D BlackPieceLocation;

	TArray<FVector2D> PossibleMoves;

	//FVector TransformTileToWorld(const FVector2D& TilePosition);

protected:
	// ottiene le coordinate della casella dalla hit result
	//FVector2D GetTileCoordinatesFromHit(const FHitResult& Hit);

	// muove il pezzo selezionato alla nuova posizione
	void MoveSelectedPiece(const FVector2D& NewPosition);

public:
	


	// va alla classe del pezzo selezionato
	//void 

};
