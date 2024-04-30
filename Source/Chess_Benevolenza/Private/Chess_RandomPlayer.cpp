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
	if (!GameMode->IsGameOver)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
		GameInstance->SetTurnMessage(TEXT("AI (Random) Turn"));
	}
	// prendo puntatori a GameMode e a GField e controllo che siano validi
	//AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	AGameField* GField = GameMode->GetGField();
	if (!GameMode || !GameMode->GField)
	{
		UE_LOG(LogTemp, Error, TEXT("GField o GameMode non valido per fare TurnOffHighlightedTiles!"));
	}

	FTimerHandle TimerHandle;

	if (IsInCheck == false)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
			{
			
				// prendo puntatori a GameMode e a GField e controllo che siano validi
				AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
				AGameField* GField = GameMode->GetGField();
				
				if (GameMode && GField)
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


					}
					while (!bValidPieceAndMove);

					// quando andrá tutto bene avró una pedina estratta a caso e una relativa mossa estratta a caso
					// mi salvo la posizione attuale della pedina
					FVector2D BlackCurrentPosition = RandomPiece->GetGridPosition();										// = GField->ReversePiecesMap[RandomPiece];

					// se dove c'è un'altra pedina (so che già che sarà bianca perché è una mossa possibile) la mangio
					if ((GField->PiecesMap[RandomNewPosition]) != nullptr)
					{
						(GField->PiecesMap[RandomNewPosition])->PieceIsEaten(RandomNewPosition);
					}

					// muovo la pedina
					RandomPiece->MovePieceFromToPosition (BlackCurrentPosition, RandomNewPosition);


					// Dopo il movimento, controlla se il pezzo è un pedone e se ha raggiunto l'ultima fila
					if (RandomPiece->GetPieceType() == EPieceType::PAWN && RandomNewPosition.X == 0)
					{
						GField->PromotionToQueen(RandomPiece);
					}

					// controllo: dopo la mia mossa il re avversario (del player 1) é in scacco?
					if (GameMode->IsKingInCheck(GField, 0)) 
					{ 
						FString Message = FString::Printf(TEXT("White King is in Check!"));
						GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Orange, Message);
						//GameInstance->SetTurnMessage(TEXT("Black is in Check!"));
						//GameInstance->GetTurnMessage();
						GameMode->Players[0]->IsInCheck = true;

						// Evidenzio in rosso la tile del re sotto scacco
						// prendo il sola lettura l'array dei pezzi bianchi
						const TArray <AChess_Piece*>& WhitePiecesArray = GField->GetWhitePiecesArray();

						AChess_Piece* King = nullptr;
						for (AChess_Piece* Piece : WhitePiecesArray)
						{
							if (Piece->GetPieceType() == EPieceType::KING)
							{
								King = Piece;
								break;
							}
						}

						IChess_PlayerInterface* UncastedHumanPlayer = GameMode->Players[0];
						(Cast<AChess_HumanPlayer>(UncastedHumanPlayer))->HighlightInRedGameFieldTiles(TArray <FVector2D> {King->GetGridPosition()}); 
					}

					// controllo: dopo la mia mossa il re avversario (del player 0 é in scacco matto?
					if (GameMode->IsCheckmate(/*GField, */0))
					{
						GameMode->Players[1]->OnWin();
						GameMode->Players[0]->OnLose();
						GameMode->IsGameOver = true;
						GameInstance->SetTurnMessage(TEXT("White King is in Checkmate! GAME OVER"));
						GameInstance->GetTurnMessage();

						// Evidenzio in rosso la tile del re sotto scacco
						// prendo il sola lettura l'array dei pezzi bianchi
						const TArray <AChess_Piece*>& WhitePiecesArray = GField->GetWhitePiecesArray();

						AChess_Piece* King = nullptr; 
						for (AChess_Piece* Piece : WhitePiecesArray)
						{
							if (Piece->GetPieceType() == EPieceType::KING)
							{
								King = Piece;
								break;
							}
						}

						IChess_PlayerInterface* UncastedHumanPlayer = GameMode->Players[0]; 
						(Cast<AChess_HumanPlayer>(UncastedHumanPlayer))->HighlightInRedGameFieldTiles(TArray <FVector2D> {King->GetGridPosition()});
					}
					else
					{
						GameMode->TurnNextPlayer();
					}

				}
				
				else
				{
					UE_LOG(LogTemp, Error, TEXT("GField o GameMode non valido per fare TurnOffHighlightedTiles!"));
				}

			}, 1, false);
	}
	
	else  // IsInCheck == true     allora prendo la mossa data dalla funzione IsCheckMate
	{
		FTimerHandle SecondTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(SecondTimerHandle, [&]()
			{
				// prendo puntatori a GameMode e a GField e controllo che siano validi
				AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
				AGameField* GField = GameMode->GetGField();
				
				if (GameMode && GField)
				{

					FVector2D Move = GameMode->GetMoveToExitTheCheck(); 
					AChess_Piece* Piece = GameMode->GetPieceToMoveToExitTheCheck();
					// muovo la pedina
					Piece->MovePieceFromToPosition(Piece->GetGridPosition(), Move);


					// Dopo il movimento, controlla se il pezzo è un pedone e se ha raggiunto l'ultima fila
					if (Piece->GetPieceType() == EPieceType::PAWN && Move.X == 0)
					{
						GField->PromotionToQueen(Piece);
					}

					// controllo: dopo la mia mossa il re avversario (del player 1) é in scacco?
					if (GameMode->IsKingInCheck(GField, 0))
					{
						FString Message = FString::Printf(TEXT("White King is in Check!"));
						GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Orange, Message);
						//GameInstance->SetTurnMessage(TEXT("Black is in Check!"));
						//GameInstance->GetTurnMessage();
						GameMode->Players[0]->IsInCheck = true;

						// Evidenzio in rosso la tile del re sotto scacco
						// prendo il sola lettura l'array dei pezzi bianchi
						const TArray <AChess_Piece*>& WhitePiecesArray = GField->GetWhitePiecesArray();

						AChess_Piece* King = nullptr;
						for (AChess_Piece* Pezzo : WhitePiecesArray)
						{
							if (Pezzo->GetPieceType() == EPieceType::KING)
							{
								King = Pezzo;
								break;
							}
						}

						IChess_PlayerInterface* UncastedHumanPlayer = GameMode->Players[0];
						(Cast<AChess_HumanPlayer>(UncastedHumanPlayer))->HighlightInRedGameFieldTiles(TArray <FVector2D> {King->GetGridPosition()});

					}

					// controllo: dopo la mia mossa il re avversario (del player 0 é in scacco matto?
					if (GameMode->IsCheckmate(/*GField, */0))
					{
						GameMode->Players[1]->OnWin();
						GameMode->Players[0]->OnLose();
						GameMode->IsGameOver = true;
						GameInstance->SetTurnMessage(TEXT("White King is in Checkmate! GAME OVER"));
						GameInstance->GetTurnMessage();

						// Evidenzio in rosso la tile del re sotto scacco
						// prendo il sola lettura l'array dei pezzi bianchi
						const TArray <AChess_Piece*>& WhitePiecesArray = GField->GetWhitePiecesArray();

						AChess_Piece* King = nullptr;
						for (AChess_Piece* Pezzo : WhitePiecesArray)
						{
							if (Pezzo->GetPieceType() == EPieceType::KING)
							{
								King = Pezzo;
								break;
							}
						}

						IChess_PlayerInterface* UncastedHumanPlayer = GameMode->Players[0];
						(Cast<AChess_HumanPlayer>(UncastedHumanPlayer))->HighlightInRedGameFieldTiles(TArray <FVector2D> {King->GetGridPosition()});
					}
					else
					{
						GameMode->TurnNextPlayer();
					}	
					
			
					GameMode->Players[1]->IsInCheck = false;

				}

				else
				{
					UE_LOG(LogTemp, Error, TEXT("GField o GameMode non valido per fare OnTurn del RandomPlayer!"));
				}


			}, 1, false);
	}	

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

