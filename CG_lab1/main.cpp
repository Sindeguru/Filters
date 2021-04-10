#include <QtCore/QCoreApplication>
#include <QImage>
#include "Filters.h"


using namespace std;
int main(int argc, char *argv[])
{

  string s;
  QImage img;
  // Находим адрес картинки и сохраняем его
  for (int i = 0; i < argc; i++)
    if (!strcmp(argv[i], "-p") && (i + 1 < argc)) // Пока символ не будет равен -p и если не заходит за пределы массива, то копируем
      s = argv[i + 1];

  img.load(QString(s.c_str())); // Строка с элемента доходит до NULL
  img.save("C:\\Users\\MrSin\\source\\repos\\Images\\Source.jpg"); // Адрес, куда сохранить
  /*
  InvertFilter invert;
  invert.process(img).save("C:\\Users\\MrSin\\source\\repos\\Images\\Invert.jpg");//Адрес фотки под фильтром invert
  BlurFilter blur;
  blur.process(img).save("C:\\Users\\MrSin\\source\\repos\\Images\\Blur.jpg");
  GuassianFilter guas;
  guas.process(img).save("C:\\Users\\MrSin\\source\\repos\\Images\\Guas.jpg");
  GrayScaleFilter grayScale;
  grayScale.process(img).save("C:\\Users\\MrSin\\source\\repos\\Images\\GrayScale.jpg");
  SepiaFilter sepia;
  sepia.process(img).save("C:\\Users\\MrSin\\source\\repos\\Images\\Sepia.jpg");
  LightFilter light;
  light.process(img).save("C:\\Users\\MrSin\\source\\repos\\Images\\Light.jpg");
  */
  SobelFilter sobel;
  sobel.process(img).save("C:\\Users\\MrSin\\source\\repos\\Images\\Sobel.jpg");
  /*
  PerfectReflectorFilter PerfRefl;
  PerfRefl.process(img).save("C:\\Users\\MrSin\\source\\repos\\Images\\PerfectReflector.jpg");
  AutolevelsFilter autolevels;
  autolevels.process(img).save("C:\\Users\\MrSin\\source\\repos\\Images\\AutoLevels.jpg");
  DilationFilter dilation;
  dilation.process(img).save("C:\\Users\\MrSin\\source\\repos\\Images\\Dilation.jpg");
  ErosionFilter erosion;
  erosion.process(img).save("C:\\Users\\MrSin\\source\\repos\\Images\\Erosion.jpg");
  
  QImage erimg;
  QImage dimg;
  erimg.load("C:\\Users\\MrSin\\source\\repos\\Images\\Erosion.jpg");
  dimg.load("C:\\Users\\MrSin\\source\\repos\\Images\\Dilation.jpg");
  dilation.process(erimg).save("C:\\Users\\MrSin\\source\\repos\\Images\\Opening.jpg");
  erosion.process(dimg).save("C:\\Users\\MrSin\\source\\repos\\Images\\Closing.jpg");

  EmbossingFilter embossing;
  embossing.process(img).save("C:\\Users\\MrSin\\source\\repos\\Images\\Embossing.jpg");
  TopHadFilter tophad;
  tophad.process(img).save("C:\\Users\\MrSin\\source\\repos\\Images\\TopHad.jpg");
  */

  return 0;
  //QCoreApplication a(argc, argv);
  //return a.exec();
}
