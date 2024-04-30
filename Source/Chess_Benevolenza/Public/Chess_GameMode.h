// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameField.h"
#include "Chess_PlayerInterface.h"
#include "GameFramework/GameModeBase.h"								  //prima era /GameModeBase.h
//#include "FEscapeMoveInfo.h"
#include "Chess_GameMode.generated.h"


class AActor;

struct FPosition;

/**
 *
 */
UCLASS()
class CHESS_BENEVOLENZA_API AChess_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// tracks if the game is over
	bool IsGameOver;

	// array of player interfaces
	TArray<IChess_PlayerInterface*> Players;
	int32 CurrentPlayer;

	// tracks the number of moves in order to signal a drawn game
	int32 MoveCounter;

	// TSubclassOf is a template class that provides UClass type safety.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGameField> GameFieldClass;

	// field size
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 FieldSize;

	// reference to a GameField object
	UPROPERTY(VisibleAnywhere)
	AGameField* GField;


	AChess_GameMode();

	// Called when the game starts or when spawnedD
	virtual void BeginPlay() override;

	// called at the start of the game
	void ChooseHumanPlayerAndStartGame();

	// get the current player
	int32 GetCurrentPlayer();

	// get the next player index
	int32 GetNextPlayer(int32 Player);

	// called at the end of the game turn
	void TurnNextPlayer();

	
	bool Draw;
 
	// mossa che passo al giocatore per uscire dallo scacco
	/*TArray <FVector2D> MoveToExitTheCheck;*/													// da rimettereee direi
	FVector2D MoveToExitTheCheck;
	AChess_Piece* PieceToMoveToExitTheCheck;

	AChess_Piece* GetPieceToMoveToExitTheCheck();
	FVector2D GetMoveToExitTheCheck();

	
	// controlla lo scacco di un solo player
	bool IsKingInCheck(AGameField* GenericGameField, int32 OpponentPlayer);

	// controlla lo scacco matto
	bool IsCheckmate(/*AGameField* GenericGameField,*/ int32 OpponentPlayer);

	// simula lo spostamento di una pedina su una scacchiera clonata									// ultima roba messa         ORA E IN GAMEFIELD
	//bool SimulateMoveAndCheckmate(AChess_Piece* PieceToSimulate, FVector2D PositionToMoveTo);

	// controlla la patta
	//bool IsDraw(int32 Player);

	// bool HasLegalMoves(int32 Player);


	AChess_Piece* King;

	FVector2D KingPosition;

	TArray<AChess_Piece*>* OpponentColorPiecesArray;

	TArray<AChess_Piece*> CopyOfColorPiecesArray;

	AGameField* ClonedGameField;							// forse ci vuole per forza   se no la metto nei due player

	// metodo get per usare il puntatore ClonedGameField ovunque nel codice (inizialmente creato per GameField)
	AGameField* GetClonedGameField() const;


	// metodo get per usare il puntatore GField ovunque nel codice (inizialmente creato per GameField)
	AGameField* GetGField() const;

	// flag per controllare che la gamefield non venga spawnata pi� di una volta
	bool bFieldGenerated;

	// flag per controllare che la gamefield non venga spawnata pi� di una volta
	bool bPiecesGenerated;

//	TArray<AChess_Piece*>* ColorPiecesArray;								
};


