/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#include "Graphics.h"

bool Wallnut::Graphics::Init()
{
	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory))) return false;

	RECT rect;
	GetClientRect(m_hWnd, &rect);

	if (FAILED(factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			m_hWnd, D2D1::SizeU(rect.right, rect.bottom)
		),
		&renderTarget
	))) return false;

	if (FAILED(DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(writeFactory),
		reinterpret_cast<IUnknown**>(&writeFactory)
	))) return false;

	return true;
}

Wallnut::Graphics::~Graphics()
{
	SafeRelease(&factory);
	SafeRelease(&renderTarget);
	SafeRelease(&writeFactory);
}

Wallnut::Graphics* Wallnut::Graphics::instance;