# **Лабораторная работа № 05**
## **Тема:** Итераторы и аллокаторы
## Что реализовано
- `hw5::vector_tracking_resource` – наследник `std::pmr::memory_resource` с переиспользованием блоков памяти, ведёт учёт выделений через `std::vector`.
- `hw5::pmr_dynamic_array<T>` – шаблонный динамический массив на базе `std::pmr::polymorphic_allocator`, forward‑итератор, поддержка простых и сложных типов.
- Демо (`src/main.cpp`) показывает работу с `int` и структурой `widget`.
- Юнит‑тесты на GoogleTest (`tests/basic_tests.cpp`), проверяют вставку и переиспользование памяти.
- CMake‑сборка и Docker‑образ на Alpine: автоматически компилируют проект и прогоняют тесты.

## Локальная сборка
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build -C Release   
```

## Docker
```bash
docker build -t hw5 .
docker run --rm hw5            # приложение
docker run --rm hw5 tests      # прогнать тесты
```
Образ собирает зависимости, конфигурирует CMake, компилирует код и прогоняет тесты на этапе build; если что-то падает, сборка завершается с ошибкой. Команда `tests` переключает entrypoint в режим запуска `ctest`.

