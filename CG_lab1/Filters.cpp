#include "Filters.h"
#include <algorithm>
#include <array>


template<class T>
T Clamp(T value, T max, T min) // Функция подгоняет значение под максимум или под минимум, если значение заходит за рамки
{
	if (value > max)
		return max;
	else if (value < min)
		return min;
	else
		return value;
}

QImage Filter::process(const QImage& img) const 
{
	QImage result(img);

	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++)
		{
			QColor color = calcNewPixelColor(img, x, y);
			result.setPixelColor(x, y, color);
		}

	return result;
}

QColor InvertFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	color.setRgb(255 - color.red(), 255 - color.green(), 255 - color.blue());
	return color;
}

//------------------------------------------------------------------------------

QColor GrayScaleFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	float Intensity = 0.3 * color.red() + 0.587 * color.green() + 0.114 * color.blue();
	color.setRgb(Intensity, Intensity, Intensity);
	return color;
}

//------------------------------------------------------------------------------

QColor SepiaFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	float k = 15;
	float Intensity = 0.3 * color.red() + 0.587 * color.green() + 0.114 * color.blue();
	color.setRgb(Clamp(Intensity + 2*k, 255.f, 0.f), Clamp(Intensity + k/2, 255.f, 0.f), Clamp(Intensity-k, 255.f, 0.f));
	return color;
}

//------------------------------------------------------------------------------

QColor LightFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	float k = 60;
	//float Intensity = 0.3 * color.red() + 0.587 * color.green() + 0.114 * color.blue();
	color.setRgb(Clamp(color.red()+k, 255.f, 0.f), Clamp(color.green()+k, 255.f, 0.f), Clamp(color.blue()+k, 255.f, 0.f));
	return color;
}

//------------------------------------------------------------------------------

QColor PerfectReflectorFilter::calcNewPixelColorA(const QImage& img, int x, int y, float maxR, float maxG, float maxB) const
{
	QColor color = img.pixelColor(x, y);
	color.setRgb(color.red() * maxR / 255, color.green() * maxG / 255, color.blue() * maxB / 255);
	return color;
}

//------------------------------------------------------------------------------

QColor ErosionFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float returnR = 255;
	float returnG = 255;
	float returnB = 255;
	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	for (int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++)
		{
			int idx = (i + radius) * size + j + radius;

			QColor color = img.pixelColor(Clamp(x + j, img.width() - 1, 0), Clamp(y + i, img.height() - 1, 0));
			if (mKernel[idx])
			{
				if (color.red() < returnR)
					returnR = color.red();
				if (color.green() < returnG)
					returnG = color.green();
				if (color.blue() < returnB)
					returnB = color.blue();
			}
		}
	return QColor(Clamp(returnR, 255.f, 0.f), Clamp(returnG, 255.f, 0.f), Clamp(returnB, 255.f, 0.f));
}

//------------------------------------------------------------------------------

QColor MedianFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float returnR = 255;
	float returnG = 255;
	float returnB = 255;
	int size = mKernel.getSize();
	int radius = mKernel.getRadius();

	int* dataR;
	int* dataG;
	int* dataB;
	dataR = new int[size];
	dataG = new int[size];
	dataB = new int[size];

	for (int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++)
		{
			int idx = (i + radius) * size + j + radius;

			QColor color = img.pixelColor(Clamp(x + j, img.width() - 1, 0), Clamp(y + i, img.height() - 1, 0));
			if (mKernel[idx])
			{
				dataR[radius * 2 + i + j] = color.red();
				dataG[radius * 2 + i + j] = color.green();
				dataB[radius * 2 + i + j] = color.blue();
			}
			sort(dataR, dataR + size); sort(dataG, dataG + size); sort(dataB, dataB + size);
		}
	return QColor(dataR[size * size / 2],dataG[size * size / 2],dataB[size * size / 2]);
}

//------------------------------------------------------------------

QColor DilationFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float returnR = 0;
	float returnG = 0;
	float returnB = 0;
	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	for (int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++)
		{
			int idx = (i + radius) * size + j + radius;

			QColor color = img.pixelColor(Clamp(x + j, img.width() - 1, 0), Clamp(y + i, img.height() - 1, 0));
			if (mKernel[idx])
			{
				if (color.red() > returnR)
					returnR = color.red();
				if (color.green() > returnG)
					returnG = color.green();
				if (color.blue() > returnB)
					returnB = color.blue();
			}
		}
	return QColor(Clamp(returnR, 255.f, 0.f), Clamp(returnG, 255.f, 0.f), Clamp(returnB, 255.f, 0.f));
}

//--------------------------------------------------------------------

QImage PerfectReflectorFilter::process(const QImage& img) const
{
	float maxR = 0;
	float maxG = 0;
	float maxB = 0;
	QColor color;
	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++)
		{
			color = img.pixelColor(x, y);
			if (color.red() > maxR)
				maxR = color.red();
			if (color.green() > maxG)
				maxR = color.green();
			if (color.blue() > maxB)
				maxR = color.blue();
		}
	QImage result(img);

	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++)
		{
			QColor color = calcNewPixelColorA(img, x, y, maxR, maxG, maxB);
			result.setPixelColor(x, y, color);
		}
	return result;
}

//------------------------------------------------------------------------------

QColor AutolevelsFilter::calcNewPixelColorA(const QImage& img, int x, int y, float maxR, float maxG, float maxB, float minR, float minG, float minB) const
{
	QColor color = img.pixelColor(x, y);
	color.setRgb(Clamp((color.red()-minR)*(255-0)/(maxR-minR),255.f,0.f), Clamp((color.green() - minG) * (255 - 0) / (maxG - minG),254.f,0.f), Clamp((color.blue() - minB) * (255 - 0) / (maxB - minB),254.f,0.f));
	return color;
}

QImage AutolevelsFilter::process(const QImage& img) const
{
	float maxR = 0;
	float maxG = 0;
	float maxB = 0;
	float minR = 255;
	float minG = 255;
	float minB = 255;
	QColor color;
	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++)
		{
			color = img.pixelColor(x, y);
			if (color.red() > maxR)
				maxR = color.red();
			if (color.green() > maxG)
				maxR = color.green();
			if (color.blue() > maxB)
				maxR = color.blue();
			if (color.red() < minR)
				minR = color.red();
			if (color.green() < minG)
				maxG = color.green();
			if (color.blue() < minB)
				maxB = color.blue();
		}
	QImage result(img);

	for (int x = 0; x < img.width(); x++)
		for (int y = 0; y < img.height(); y++)
		{
			QColor color = calcNewPixelColorA(img, x, y, maxR, maxG, maxB, minR, minG, minB);
			result.setPixelColor(x, y, color);
		}
	return result;
}

//------------------------------------------------------------------------------


//----------------------------------

QColor MatrixFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float returnR = 0;
	float returnG = 0;
	float returnB = 0;
	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	for(int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++)
		{
			int idx = (i + radius) * size + j + radius;

			QColor color = img.pixelColor(Clamp(x + j, img.width() - 1, 0), Clamp(y + i, img.height() - 1, 0));
			returnR += color.red() * mKernel[idx];
			returnG += color.green() * mKernel[idx];
			returnB += color.blue() * mKernel[idx];
		}
	return QColor(Clamp(returnR, 255.f, 0.f), Clamp(returnG, 255.f, 0.f), Clamp(returnB, 255.f, 0.f));
}

QImage TopHadFilter::process(const QImage& img) const
{
	{
		DilationFilter Dilation;
		QImage img1 = Dilation.process(img);
		ErosionFilter Eros;
		img1 = Eros.process(img1);

		QImage result(img);
		for (int x = 0; x < img.width(); x++)
			for (int y = 0; y < img.height(); y++)
			{
				QColor color = img.pixelColor(x, y);;
				QColor color1 = img1.pixelColor(x, y);
				float returnR = color.red() - color1.red();
				float returnG = color.green() - color1.green();
				float returnB = color.blue() - color1.blue();
				QColor colorResult;
				colorResult.setRgb(Clamp(returnR, 255.f, 0.f), Clamp(returnG, 255.f, 0.f), Clamp(returnB, 255.f, 0.f));
				result.setPixelColor(x, y, colorResult);
			}
		return result;
	}
}

QColor SobelFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float returnR = 0;
	float returnG = 0;
	float returnB = 0;
	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	for (int i = -radius; i <= radius; i++)
		for (int j = -radius; j <= radius; j++)
		{
			int idx = (i + radius) * size + j + radius;

			QColor color = img.pixelColor(Clamp(x + j, img.width() - 1, 0), Clamp(y + i, img.height() - 1, 0));
			if (mKernel[idx])
			{
				returnR += color.red()* mKernel[idx];
				returnG += color.green()* mKernel[idx];
				returnB += color.blue()* mKernel[idx];
			}
		}
	return QColor(Clamp(abs(returnR), 255.f, 0.f), Clamp(abs(returnG), 255.f, 0.f), Clamp(abs(returnB), 255.f, 0.f));
}
