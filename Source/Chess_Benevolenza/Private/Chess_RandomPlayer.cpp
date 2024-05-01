// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_RandomPlayer.h"
#include "Chess_HumanPlayer.h"

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
	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode && !GameMode->IsGameOver)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
		GameInstance->SetTurnMessage(TEXT("AI (Random) Turn"));
	}

	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode non valido per fare TurnOffHighlightedTiles!"));
	}
	

	FTimerHandle TimerHandle;
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			
			// prendo puntatori a GameMode e a GField e controllo che siano validi
			AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
			AGameField* GField = GameMode->GetGField();

			if (!GameMode->IsGameOver && GameMode && GField)
			{
				bool bValidPieceAndMove = false;
				AChess_Piece* RandomPiece = nullptr;
				FVector2D RandomNewPosition;

				do
				{

					// prendo il sola lettura l'array dei pezzi neri
					const TArray <AChess_Piece*>& BlackPiecesArray = GField->GetBlackPiecesArray();
					// genero un indice casuale all'interno dei limiti dell'array
					int32 RandIdx = FMath::RandRange(0, BlackPiecesArray.Num() - 1);
					// prendo il pezzo corrispondente all'indice casuale
					RandomPiece = Cast<AChess_Piece>(BlackPiecesArray[RandIdx]);

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


				} while (!bValidPieceAndMove);

				// quando andrá tutto bene avró una pedina estratta a caso e una relativa mossa estratta a caso
				// mi salvo la posizione attuale della pedina
				FVector2D BlackCurrentPosition = RandomPiece->GetGridPosition();										// = GField->ReversePiecesMap[RandomPiece];

				// se dove c'è un'altra pedina (so che già che sarà bianca perché è una mossa possibile) la mangio
				if ((GField->PiecesMap[RandomNewPosition]) != nullptr)
				{
					(GField->PiecesMap[RandomNewPosition])->PieceIsEaten(RandomNewPosition);
				}

				// muovo la pedina
				RandomPiece->MovePieceFromToPosition(BlackCurrentPosition, RandomNewPosition);

				// Dopo il movimento, controlla se il pezzo è un pedone e se ha raggiunto l'ultima fila
				if ((RandomPiece->GetPieceType() == EPieceType::PAWN) && RandomNewPosition.X == 0)
				{
					GField->PromotionToQueen(RandomPiece);
				}


				// prendo il booleano che mi dice se questo giocatore é in scacco a seguito della mossa dell'avversario
				// IsInCheck           //  non lo modifico ora

				// controllo se i giocatori siano in scacco

				bool WhiteInCheck = GameMode->IsKingInCheck(GameMode->GetGField(), 0);
				bool BlackInCheck = GameMode->IsKingInCheck(GameMode->GetGField(), 1);
				bool WhiteWasInCheck = GameMode->Players[0]->IsInCheck;
				// se prima della mossa il nero era in scacco
				if (IsInCheck)
				{
					// se é ancora in scacco diventa scacco matto per il nero e finisce il gioco
					if (BlackInCheck)
					{
						GameMode->Players[0]->OnWin();
						GameMode->Players[1]->OnLose();
						GameMode->IsGameOver = true;
						GameInstance->SetTurnMessage(TEXT("Black King is in Checkmate! GAME OVER"));
						GameInstance->GetTurnMessage();
					}

					// se è uscito dallo scacco con la mossa appena fatta
					else if (!BlackInCheck)
					{
						GameMode->Players[1]->IsInCheck = false;
						GameMode->TurnNextPlayer();
					}
				}

				// se prima della mossa il nero NON era in scacco
				else if (!IsInCheck)
				{
					// se dopo la mossa appena fatta è in scacco
					if (BlackInCheck)
					{
						FString Message = FString::Printf(TEXT("Black King is in Check!"));
						GEngine->AddOnScreenDebugMessage(2, 2.f, FColor::Orange, Message);
						GameMode->Players[1]->IsInCheck = true;
						GameMode->TurnNextPlayer();
					}
					// continua ad essere non in scacco
					else if (!BlackInCheck)
					{
						GameMode->Players[1]->IsInCheck = false;
						GameMode->TurnNextPlayer();
					}
				}

				// controllo se dopo la mia mossa il re nero è in scacco
				if (!WhiteWasInCheck && WhiteInCheck)
				{
					FString Message = FString::Printf(TEXT("White King is in Check!"));
					GEngine->AddOnScreenDebugMessage(2, 2.f, FColor::Orange, Message);
					GameMode->Players[0]->IsInCheck = true;
					GameMode->TurnNextPlayer();
				}

				if (WhiteWasInCheck && WhiteInCheck)
				{
					GameMode->Players[1]->OnWin();
					GameMode->Players[0]->OnLose();
					GameMode->IsGameOver = true;
					GameInstance->SetTurnMessage(TEXT("White King is in Checkmate! GAME OVER"));
					GameInstance->GetTurnMessage();
				}
				
				// cerco il re bianco
				AChess_Piece* WhiteKing = nullptr;

				for (AChess_Piece* Piece : GameMode->GField->WhitePiecesArray)
				{
					if (Piece->GetPieceType() != EPieceType::KING)
					{
						WhiteKing = Piece;
						break;
					}
				}
				
				// cerco il re nero
				AChess_Piece* BlackKing = nullptr;
				for (AChess_Piece* Piece : GameMode->GField->BlackPiecesArray)
				{
					if (Piece->GetPieceType() != EPieceType::KING)
					{
						BlackKing = Piece;
						break;
					}
				}

				// se non ne trovo uno vuol dire che é stato mangiato e quindi é game over
				if (!WhiteKing)
				{
					GameMode->Players[1]->OnWin();
					GameMode->Players[0]->OnLose();
					GameMode->IsGameOver = true;
					GameInstance->SetTurnMessage(TEXT("White King was Eaten! GAME OVER"));
					GameInstance->GetTurnMessage();
				}
				
				if (!BlackKing)
				{
					GameMode->Players[0]->OnWin();
					GameMode->Players[1]->OnLose();
					GameMode->IsGameOver = true;
					GameInstance->SetTurnMessage(TEXT("Black King was Eaten! GAME OVER"));
					GameInstance->GetTurnMessage();
				}

				else
				{
				

				}



			}
			

		}, 1, false);
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

