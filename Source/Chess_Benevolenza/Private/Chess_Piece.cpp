// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_Piece.h"
#include "GameField.h"
#include "Chess_GameMode.h"

// Sets default values
AChess_Piece::AChess_Piece()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//metto false
	PrimaryActorTick.bCanEverTick = false;

}

void AChess_Piece::SetGridPosition(const double InX, const double InY)
{
	TileGridPosition.Set(InX, InY);
}
FVector2D AChess_Piece::GetGridPosition()							// avevo messo come argomento AChess_Piece* Piece ma non credo sia necessario basta fare puntatoreAoggettoPiece->GetGridPosition()
{
	return TileGridPosition;
}
/*
void AChess_Piece::SetTileStatus(ETileStatus TileStatus)
{
	ATile->GetTileStatus();
	ATile->Status = TileStatus;
}
*/

void AChess_Piece::SetPieceType(EPieceType NewPieceType)
{
	PieceType = NewPieceType;
}

void AChess_Piece::SetPieceColor(EPieceColor NewPieceColor)
{
	PieceColor = NewPieceColor;
}

EPieceColor AChess_Piece::GetPieceColor()
{
	return PieceColor;
}

EPieceType AChess_Piece::GetPieceType() const
{
	return PieceType;
}

/*
EPieceType AChess_Piece::GetPieceType() const
{
	return PieceType;
}
*/




// Called when the game starts or when spawned
void AChess_Piece::BeginPlay()
{
	Super::BeginPlay();
	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	//GameMode->GField->OnResetEvent.AddDynamic(this, &AChess_Piece::SelfDestroy);				//ho dovuto bc non mi andava la AddDynamic
}

// Called every frame
void AChess_Piece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<FVector2D> AChess_Piece::CalculatePossibleMoves()
{
	// TArray<FVector2D> PossibleMoves; così mi da errore di sovrascrizione della PossibleMoves variabile di classe dichiarata in Chess_Piece
	return PossibleMoves;
}

void  AChess_Piece::SelfDestroy()
{
	Destroy();
}

