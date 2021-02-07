# Graph server &amp; client

Асинхнонный сервер с пулом потоков, который отрисует графики (можно отрисовать, получить ранее нарисованные, удалить и вывести список всех графиков).


## Запуск
Открыть файл с решением, скомпилировать (в релизе 64 разрядности) клиент и сервер.

### Что необходимо

- x64 компьютер
- MS Visual Studio 2019+ и с++17+
- boost 1.72.0+

### Команды для запуска сервера
  
-d --dir=<path>  путь до папки сервера, где сохранять файлы
-i --ip=<IP>   IP адрес, на котором работает listener сервер
-p --port=<uint>  порт, на котором работает listener сервера
-m --maxdegree=<uint>  максимальная степень полинома, которую можно обработать
-n --maxnfiles=<uint>  максимальная число файлов, что клиент может сохранить
-w --workers=<uint>  максимальное количество доступных кэш-таблиц
-v --verbose  печатать в stdout подробно производимые операции сервером 
(без этого флага печатать только ошибки)
-h --help  распечатать в консоль аргументы, как использовать программу


Примеры:
Запуск сервера:
GraphServer.exe -d &quot;path/to/folder&quot; -i &quot;127.0.0.1&quot; -p 1234 -m 100 -n 10 
GraphServer.exe --help

### Команды клиента

| Команда | Описание | Вывод |
| --- | --- | --- |
| paint   <name.bmp>  H=<uint> W=<uint>   <function>   [<float>, < float>] | создать файл c полотном размера HxW пикселей с именем name для функции и отрезка, сохранить файл и отправить клиенту | Ok + файл в бинарной строке или ошибка Error с описанием |
| "get <name.bmp>"  | получить созданный ранее файл по имени | Ok + файл в бинарной строке или ошибка Error с описанием |
| rem <name.bmp> | удалить созданный ранее файл с именем | Ok и удаление файла из каталога или ошибка Error с описанием
| list | получить список имён имеющихся файлов, доступных для данного клиента.  | Ok и список изображений каталога |

Примеры:
paint gr H=100 W=100 2 0 0 1 [-50.0, 50.0]
list
get gr
rem gr

## Запуск тестов

Открыть проект клиента, скомпилировать и запустить.
