## Поиск идентичных файлов в двух папках

### Инструция по сборке
Проект собирается с помощью CMake. 

Для сборки с помощью Qt Creator необходимо отрыть `CMakeLists.txt` через меню _"Open File or Project"_, и при конфигурации указать набор _"Desktop Qt ..."_

Иначе можно выполнить команды в консоли:

1. Клонировать репозиторий:
```
git clone https://github.com/lizardwizardd/BinCompare-Qt
```
2. Сгенерировать файлы сборки:
```
cmake -S . -B ./build
```
3. Скомпилировать проект:
```
cmake --build ./build
```
