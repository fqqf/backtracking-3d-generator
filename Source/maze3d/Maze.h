// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <stack>
#include "Maze.generated.h"


UCLASS()
class MAZE3D_API AMaze : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMaze();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category=Room, meta = (AllowPrivateAccess = "true"))
	USceneComponent* Scene;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Room, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> Visited;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Room, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> Blue;

	void SpawnItem(FVector Position, UClass* Class);
	void BuildMaze();
	void Backtrack();

	int MazeWidth = 10;
	int MazeHeight = 10;
	int* MazeCells;
	int PathWidth = 1;

	bool AllowToBuildMaze = false;

	enum {
		CELL_PATH_N = 0x01,
		CELL_PATH_E = 0x02,
		CELL_PATH_S = 0x04,
		CELL_PATH_W = 0x08,
		CELL_VISITED = 0x10,
	};

	int VisitedCells;
	std::stack<FVector2D> Stack;
};
