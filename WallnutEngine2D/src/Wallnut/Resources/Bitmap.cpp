#include "Bitmap.h"
#include "Wallnut/Application.h"
#include "Wallnut/Graphics/Graphics.h"

Wallnut::Bitmap::Bitmap(const wchar_t* path)
{
	
	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	IWICImagingFactory* wicFactory = NULL;
	if (FAILED(CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID*)&wicFactory
	))) {

		return;
	}

	IWICBitmapDecoder* wicDecoder = NULL;

	if (FAILED(wicFactory->CreateDecoderFromFilename(
		path,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad, \
		& wicDecoder
	))) {
		return;
	}

	IWICBitmapFrameDecode* wicFrame = NULL;

	if (FAILED(wicDecoder->GetFrame(0, &wicFrame)))
	{
		return;
	}

	IWICFormatConverter* wicConverter = NULL;

	if (FAILED(wicFactory->CreateFormatConverter(&wicConverter)))
	{
		return;
	}

	if (FAILED(wicConverter->Initialize(
		wicFrame,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		NULL,
		0.0,
		WICBitmapPaletteTypeCustom
	))) {
		return;
	}

	if (FAILED(Graphics::instance->renderTarget->CreateBitmapFromWicBitmap(
		wicConverter,
		NULL,
		&bitmap
	)))
	{
		return;
	}

	SafeRelease(&wicFactory);
	SafeRelease(&wicDecoder);
	SafeRelease(&wicConverter);
	SafeRelease(&wicFrame);
}

Wallnut::Bitmap::~Bitmap()
{
	SafeRelease(&bitmap);
}
