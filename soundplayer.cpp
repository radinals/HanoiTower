#include "soundplayer.h"

#include <qsoundeffect.h>

SoundPlayer::SoundPlayer()
{
	m_sound = new QSoundEffect;
}

SoundPlayer::~SoundPlayer()
{
	delete m_sound;
}
