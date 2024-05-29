#ifndef SOUNDS_H
#define SOUNDS_H

#include <QSoundEffect>
#include <QString>
#include <qsoundeffect.h>

class SoundPlayer
{
      public:
	SoundPlayer() { m_sound = new QSoundEffect; }

	~SoundPlayer() { delete m_sound; }

	inline void setSource(const QString& qrc_path)
	{
		m_sound->setSource("qrc" + qrc_path);
	}

	QSoundEffect* const getSound() const { return m_sound; };

      private:
	QSoundEffect* m_sound;
};

#endif // !SOUNDS_H
