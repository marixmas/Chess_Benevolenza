// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_RandomPlayer.h"

// Sets default values
AChess_RandomPlayer::AChess_RandomPlayer()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GameInstance = Cast<UChess_GameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

}

// Called when the game starts or when spawned
void AChess_RandomPlayer::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AChess_RandomPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AChess_RandomPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AChess_RandomPlayer::OnTurn()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
	GameInstance->SetTurnMessage(TEXT("AI (Random) Turn"));

	// prendo puntatori a GameMode e a GField e controllo che siano validi
	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	AGameField* GField = GameMode->GetGField();
	if (!GameMode || !GameMode->GField)
	{
		UE_LOG(LogTemp, Error, TEXT("GField o GameMode non valido per fare TurnOffHighlightedTiles!"));
	}

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{

			// prendo puntatori a GameMode e a GField e controllo che siano validi
			AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
			AGameField* GField = GameMode->GetGField();
			if (!GameMode || !GameMode->GField)
			{
				UE_LOG(LogTemp, Error, TEXT("GField o GameMode non valido per fare TurnOffHighlightedTiles!"));
			}

			bool bValidPieceAndMove = false;
			AChess_Piece* RandomPiece = nullptr;
			FVector2D RandomNewPosition;

			do
			{	
				// prendo il sola lettura l'array dei pezzi
				const TArray <AChess_Piece*>& PiecesArray = GField->GetPiecesArray();
				// genero un indice casuale all'interno dei limiti dell'array
				int32 RandIdx = FMath::RandRange(0, PiecesArray.Num() - 1);
				// prendo il pezzo corrispondente all'indice casuale
				RandomPiece = Cast<AChess_Piece>(PiecesArray[RandIdx]);
				
				// se il pezzo é bianco estrai di nuovo
				if (RandomPiece->GetPieceColor() == EPieceColor::WHITE)
				{
					continue;
				}

				// calcola le mosse possibili
				TArray<FVector2D> PossibleMovesArray = RandomPiece->CalculatePossibleMoves();

				// se il pezzo è nero ma non ci sono mosse possibili, estrai di nuovo
				if (PossibleMovesArray.Num() == 0)
				{
					continue;
				}

				// prendi una mossa casuale
				int32 RandomMoveIdx = FMath::RandRange(0, PossibleMovesArray.Num() - 1);
				RandomNewPosition = PossibleMovesArray[RandomMoveIdx];

				bValidPieceAndMove = true;


			}
			while (!bValidPieceAndMove);

			// quando andrá tutto bene avró una pedina estratta a caso e una relativa mossa estratta a caso
			// mi salvo la sua posizione attuale della pedina
			FVector2D BlackCurrentPosition = GField->ReversePiecesMap[RandomPiece];

			// muovo la pedina
			RandomPiece->MovePieceFromToPosition (BlackCurrentPosition, RandomNewPosition);

			// Controllo se si verifica scacco									////////////////////////////////////////

			// Controllo se si verifica scacco matto													///////////////////////////////////////

			// Controllo se si verifica patta												///////////////////////////////////////

			GameMode->CheckIfKingIsEaten();

			GameMode->TurnNextPlayer();													///// DA RIMETTEREEEEE


		}, 3, false);

}

void AChess_RandomPlayer::OnWin()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Wins!"));
	GameInstance->SetTurnMessage(TEXT("AI Wins!"));
	GameInstance->IncrementScoreAiPlayer();
}

void AChess_RandomPlayer::OnLose()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Loses!"));
	GameInstance->SetTurnMessage(TEXT("AI Loses!"));
}

