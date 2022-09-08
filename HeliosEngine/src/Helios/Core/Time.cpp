/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Time.h"

using Helios::Time;

__int64 Time::m_firstFrame = 0;
__int64 Time::m_lastFrame = 0;
float Time::m_deltaTime = 0.0f;

double Time::PCFreq = 0.0;