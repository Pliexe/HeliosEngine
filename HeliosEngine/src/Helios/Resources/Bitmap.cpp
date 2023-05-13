#include "Bitmap.h"
#include "Helios/Core/DepricatedApplication.h"
#include "Helios/Graphics/DepricatedGraphics.h"
#include "Helios/Core/Logger.h"
#include "Helios/Translation/Vector.h"

Helios::Bitmap::Bitmap(const wchar_t* path)
{
	if (DepricatedGraphics::instance) {

		CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
		IWICImagingFactory* wicFactory = NULL;
		if (FAILED(CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			(LPVOID*)&wicFactory
		))) {
			LOG_ERROR("Failed to initialize IWIC Imaging Factory");
			return;
		}

		IWICBitmapDecoder* wicDecoder = NULL;

		if (FAILED(wicFactory->CreateDecoderFromFilename(
			path,
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&wicDecoder
		))) {
			LOG_ERROR("Failed to create an WIC Bitmap Decoder");
			return;
		}

		IWICBitmapFrameDecode* wicFrame = NULL;

		if (FAILED(wicDecoder->GetFrame(0, &wicFrame)))
		{
			LOG_ERROR("Failed to get the first frame of the image");
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
			LOG_ERROR("Failed to initialize WIC Converter");
			return;
		}

		ID2D1Bitmap* bmp;

		if (FAILED(DepricatedGraphics::instance->m_renderTarget2D->CreateBitmapFromWicBitmap(
			wicConverter,
			NULL,
			&bmp
		)))
		{
			LOG_ERROR("Failed to convert Bitmap from WicBitmap");
			return;
		}

		bitmap.reset(bmp, [](ID2D1Bitmap* ptr) {
			SafeRelease(&ptr);
		});

		SafeRelease(&wicFactory);
		SafeRelease(&wicDecoder);
		SafeRelease(&wicConverter);
		SafeRelease(&wicFrame);

	} else {
		throw std::logic_error("DepricatedGraphics Instance is nullptr. Can't create a Bitmap before the program has Initialized!");
		abort();
	}
}