
				Базовый дистрибутив библиотеки FreeImage (DLL-версия)


Этот мини-набор составлен на базе пакета FreeImage 3.9.2 [WIN32] (1,0 МБ)

http://prdownloads.sourceforge.net/freeimage/FreeImage392Win32.zip?download .


Сюда включены 3 файла из этого пакета:

1. FreeImage.dll - Динамическая библиотека программной реализацией FreeImage.

2. FreeImage.h - Заголовочный файл для FreeImage.dll.

3. FreeImage.lib - компоновочный файл для FreeImage.dll.


Эти 3 файла - это всё, что Вам нужно для того, чтобы добавить в свою СИ (СИ++) программу

поддержку библиотеки FreeImage. В исходный пакет FreeImage 3.9.2 [WIN32] входят также 

многие другие файлы, не нужные нам для работы.

***********************************************************************************************

						Использование


Рассмотрим использование этих 3 файлов на примере компилятора MS Visual C++ v6.0 (в 7 версии 

аналогично).


1. Скопируйте файл "FreeImage.lib" в папку, где находятся базовые lib-файлы компилятора

MS Visual C++ v6.0 - обычно это:


C:\Program Files\Microsoft Visual Studio\VC98\Lib .


Это - единовременный шаг, т.е. потом его уже не потребуется делать при создании новых 

FreeImage-приложений.


2. Скопируйте файл "FreeImage.h" в папку, где находятся базовые h-файлы компилятора
 
MS Visual C++ v6.0 - обычно это:


C:\Program Files\Microsoft Visual Studio\VC98\Include .


Это тоже единовременный шаг.

***********************************************************************************************


Каждый раз, создавая новый проект консольного приложения в компиляторе MS Visual C++ v6.0,

делайте следующее (пункты 3 и 4):


3.

а. Откройте в меню компилятора пункт Project -> Settings... .

б. Выберите вкладку Link.

в. В поле Object/library modules: добавьте в конец строки через пробел строку "freeimage.lib".


4. Кладите копию файла "FreeImage.dll" в папку с получаемым FreeImage-экзешником.

Также на время создания такого приложения дополнительно кладите копию файла "FreeImage.dll"

в папку, содержащую dsw-файл проекта - это даст Вам возможность запускать создаваемый

FreeImage-экзешник прямо из MS Visual C++ v6.0 в процессе отлаживания.


************************************************************************************************

monday2000.

http://www.djvu-soft.narod.ru/bookscanlib/