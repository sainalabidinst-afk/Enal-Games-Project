#include "LanunGameModeBase.h"
#include "LanunPlayerController.h"
#include "LanunCharacterHeroBase.h"

ALanunGameModeBase::ALanunGameModeBase()
{
	PlayerControllerClass = ALanunPlayerController::StaticClass();
	DefaultPawnClass = ALanunCharacterHeroBase::StaticClass();
}
