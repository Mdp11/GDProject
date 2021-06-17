// © 2021 Alessandro Arena, Mattia De Prisco, Nunzio Lopardo All Rights Reserved


#include "ArrowNotify.h"

#include "GDProject/Units/GDArcher.h"
#include "GDProject/Units/Actors/GDArrow.h"

void UArrowNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AGDArcher* Archer = Cast<AGDArcher>(MeshComp->GetOwner());
	if (Archer)
	{
		if (bDrawArrow && Archer->ArrowClass)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AGDArrow* Arrow = Archer->GetWorld()->SpawnActor<AGDArrow>(Archer->ArrowClass, SpawnParameters);
			if (Arrow)
			{

				Arrow->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				                          Archer->ArrowAttachSocketName);
				Arrow->SetOwner(Archer);
				Archer->Arrow = Arrow;
			}
		}
		else if (bFireArrow)
		{
			Archer->Arrow->Fire();
		}
	}
}
