#pragma once
#include <QColor>
#include <QImage>

using namespace std;
class Filter
{
protected:
	virtual QColor calcNewPixelColor(const QImage& img, int x, int y) const = 0; // ����������� ���� �������
public:
	virtual ~Filter() = default; 
	virtual QImage process(const QImage& img) const; //������������� ���� ������� �� ��������� ����������� (calcNewPixelColor) � ��������� ��� � �����
};

class InvertFilter :public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override; //������ ����������� �������� ������� ����� RGB �� ��������
};

class GrayScaleFilter :public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override; 
};

class SepiaFilter :public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override; 
};

class LightFilter :public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override; 
};

class PerfectReflectorFilter :public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override { QColor color = img.pixelColor(x, y); return color; }
	QColor calcNewPixelColorA(const QImage& img, int x, int y,float maxR, float maxG, float maxB) const;
public:
	QImage process(const QImage& img) const override;
};

class AutolevelsFilter :public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override { QColor color = img.pixelColor(x, y); return color; }
	QColor calcNewPixelColorA(const QImage& img, int x, int y, float maxR, float maxG, float maxB,float minR, float minG, float minB) const;
public:
	QImage process(const QImage& img) const override;
};

class Kernel
{
protected:
	unique_ptr<float[]> data;//����� ����������� ���������. ����� - ������ ����
	size_t radius;// ����������� ������������� ���. ���������� � ����������� �������� unsigned 
	size_t getLen() const { return getSize() * getSize(); }

public:
	Kernel(size_t radius) :radius(radius) // ������
	{
		data = make_unique<float[]>(getLen()); // ��������� ������ ��� ������
	}
	Kernel(const Kernel& other) : Kernel(other.radius) // ������ �����������
	{
		copy(other.data.get(), other.data.get() + getLen(), data.get());
	}
	//��������� 
	size_t getRadius() const { return radius; } 
	size_t getSize() const { return 2 * radius + 1; } // �������
	float operator[](size_t id)const { return data[id]; } 
	float& operator[](size_t id) { return data[id]; }
};

class MatrixFilter :public Filter
{
protected:
	//�������� �� ��������, �.�. ������ ���� ���������� ���������
	Kernel mKernel;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	MatrixFilter(const Kernel& kernel) :mKernel(kernel) {};
	virtual ~MatrixFilter() = default;
};

class BlurKernel : public Kernel
{
public:
	using Kernel::Kernel;
	BlurKernel(size_t radius = 2) :Kernel(radius)
	{
		for (size_t i = 0; i < getLen(); i++)
			data[i] = 1.0f / getLen();
	}
};

class BlurFilter : public MatrixFilter
{
public:
	BlurFilter(size_t radius = 1) : MatrixFilter(BlurKernel(radius)) {}
};

//------------------------------------------------------------------------------

class SobelKernel : public Kernel
{
public:
	using Kernel::Kernel;
	SobelKernel(size_t radius = 1) :Kernel(radius)
	{
		data[0] = -1; data[1] = 0; data[2] = 1;
		data[3] = -2; data[4] = 0; data[5] = 2;
		data[6] = -1; data[7] = 0; data[8] = 1;
	}
};
/*
class SobelKernel : public Kernel
{
public:
	using Kernel::Kernel;
	SobelKernel(size_t radius = 1) :Kernel(radius)
	{

		data[0] = -1; data[1] = 0; data[2] = 1;
		data[3] = -2; data[4] = 0; data[5] = 2;
		data[6] = -1; data[7] = 0; data[8] = 1;
	}
};
*/
/*float kernelY[3][3] = { {-1, -2, -1},
												{0,  0,  0},
												{1,  2,  1} };
												*/


class SobelFilter : public MatrixFilter
{
public:
	SobelFilter(size_t radius = 1) : MatrixFilter(SobelKernel(radius)) {}
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

//------------------------------------------------------------------------------

class GuassianKernel : public Kernel
{
public:
	using Kernel::Kernel;
	GuassianKernel(size_t radius = 2, float sigma = 3.f) :Kernel(radius)
	{
		//���� ���������� ����
		float norm = 0;
		int signed_radius = static_cast<int>(radius); //����� ������������ ���������� ���� ������
		//������ ���� �������
		for (int x = -signed_radius; x < signed_radius; x++)
			for (int y = -signed_radius; y < signed_radius; y++)
			{
				size_t idx = (x + radius) * getSize() + (y + radius);
				data[idx] = exp(-(x * x + y * y) / (sigma * sigma));
				norm += data[idx];
			}
		//�������� ����
		for (size_t i = 0; i < getLen(); i++)
			data[i] /= norm;
	}
};

class GuassianFilter : public MatrixFilter
{
public:
	GuassianFilter(size_t radius = 1) : MatrixFilter(GuassianKernel(radius)) {}
};

//------------------------------------------------------------------------------

class DilationKernel : public Kernel
{
public:
	using Kernel::Kernel;
	DilationKernel(size_t radius = 1) :Kernel(radius)
	{
		data[0] = 0.0f; data[1] = 1.f; data[2] = 0.f;
		data[3] = 0.f; data[4] = 0.0f; data[5] = 0.f;
		data[6] = 0.f; data[7] = 1.f; data[8] = 0.f;
		/*
		
		data[0] = 0.0f; data[1] = 0.f; data[2] = 1.f; data[3] = 0.f; data[4] = 0.f;
		data[5] = 0.f; data[6] = 1.0f; data[7] = 1.f; data[8] = 1.f; data[9] = 0.f;
		data[10] = 1.f; data[11] = 1.f; data[12] = 1.f; data[13] = 1.f; data[14] = 1.f;
		data[15] = 0.f; data[16] = 1.f; data[17] = 1.f; data[18] = 1.f; data[19] = 0.f;
		data[20] = 0.f; data[21] = 0.f; data[22] = 1.f; data[23] = 0.f; data[24] = 0.f;
		for (size_t i = 0; i < getLen(); i++)
		{
			data[i] = 1.0f;
		}*/
	}
};

class DilationFilter : public MatrixFilter
{
public:
	DilationFilter(size_t radius = 1) : MatrixFilter(DilationKernel(radius)) {}
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

//------------------------------------------------------------------------------

class ErosionKernel : public Kernel
{
public:
	using Kernel::Kernel;
	ErosionKernel(size_t radius = 1) :Kernel(radius)
	{
		
			data[0] = 0.0f; data[1] = 1.f; data[2] = 0.f;
			data[3] = 1.f; data[4] = 1.0f; data[5] = 1.f;
			data[6] = 0.f; data[7] = 1.f; data[8] = 0.f;
	
	/*
		data[0] = 0.0f; data[1] = 0.f; data[2] = -1.f; data[3] = 0.f; data[4] = 0.f;
		data[5] = 0.f; data[6] = -1.0f; data[7] = -1.f; data[8] = -1.f; data[9] = 0.f;
		data[10] = -1.f; data[11] = -1.f; data[12] = 1.f; data[13] = -1.f; data[14] = -1.f;
		data[15] = 0.f; data[16] = -1.f; data[17] = -1.f; data[18] = -1.f; data[19] = 0.f;
		data[20] = 0.f; data[21] = 0.f; data[22] = -1.f; data[23] = 0.f; data[24] = 0.f;
		*/
	}
};

class ErosionFilter :public MatrixFilter
{
protected:
//	ErosionKernel tmpKernel;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	ErosionFilter(size_t radius = 1) : MatrixFilter(ErosionKernel(radius)) {}
};

//------------------------------------------------------------------------------

class MedianKernel : public Kernel
{
public:
	using Kernel::Kernel;
	MedianKernel(size_t radius = 2) :Kernel(radius)
	{
		data[0] = 1.0f; data[1] = 1.f; data[2] = 1.f;
		data[3] = 1.f; data[4] = 1.0f; data[5] = 1.f;
		data[6] = 1.f; data[7] = 1.f; data[8] = 1.f;
	}
};

class MedianFilter :public MatrixFilter
{
protected:
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	MedianFilter(size_t radius = 2) : MatrixFilter(MedianKernel(radius)) {}
};


//------------------------------------------------------------------------------

class EmbossingKernel : public Kernel
{
public:
	using Kernel::Kernel;
	EmbossingKernel(size_t radius = 1) :Kernel(radius)
	{
		data[0] = 0.0f; data[1] = 1.f; data[2] = 0.f;
		data[3] = 1.f; data[4] = 0.0f; data[5] = -1.f;
		data[6] = 0.f; data[7] = -1.f; data[8] = 0.f;
	}
};

class EmbossingFilter : public MatrixFilter
{
public:
	EmbossingFilter(size_t radius = 1) : MatrixFilter(EmbossingKernel(radius)) {}
};

//------------------------------------------------------------------------------

class TopHadKernel : public Kernel
{
public:
	using Kernel::Kernel;
	TopHadKernel(size_t radius = 1) :Kernel(radius)
	{
		data[0] = 1.0f; data[1] = 0.f; data[2] = 1.f;
		data[3] = 1.f; data[4] = 0.0f; data[5] = 1.f;
		data[6] = 1.f; data[7] = 0.f; data[8] = 1.f;
	}
};

class TopHadFilter : public MatrixFilter
{
public:
	TopHadFilter(size_t radius = 1) : MatrixFilter(TopHadKernel(radius)) {}
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override { return QColor(); };
public:
	QImage process(const QImage& img) const override;
};

//------------------------------------------------------------------------------

