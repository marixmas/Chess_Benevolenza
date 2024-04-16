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

// Called when the game starts or when spawned
void AChess_Piece::BeginPlay()
{
	Super::BeginPlay();
	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	//GameMode->GField->OnResetEvent.AddDynamic(this, &AChess_Piece::SelfDestroy);				//ho dovuto bc non mi andavba la AddDynamic
}

// Called every frame
void AChess_Piece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void  AChess_Piece::SelfDestroy()
{
	Destroy();
}

