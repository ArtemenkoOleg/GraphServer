#pragma once

/*
-d --dir=<path>	путь до папки сервера, где сохранять файлы
-i --ip=<IP> 	IP адрес, на котором работает listener сервер
-p --port=<uint>	порт, на котором работает listener сервера
-m --maxdegree=<uint>	максимальная степень полинома, которую можно обработать
-n --maxnfiles=<uint>	максимальная число файлов, что клиент может сохранить
-w --workers=<uint>	максимальное количество доступных кэш-таблиц
-v --verbose	печатать в stdout подробно производимые операции сервером 
(без этого флага печатать только ошибки)
-h --help	распечатать в консоль аргументы, как использовать программу
*/
struct GraphServerConfig {
  std::string dir;
  std::string ip;
  size_t port;
  size_t maxdegree;
  size_t maxnfiles;
  size_t workers;
  bool verbose;
};