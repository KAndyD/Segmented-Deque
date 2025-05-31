// SegmentedDeque.h
#pragma once
#include "Sequence.h"
#include "DynamicArray.h"
#include <functional>
#include <algorithm>

template <class T>
class SegmentedDeque : public Sequence<T> {
    private:
    struct Segment {
        DynamicArray<T> data;  // Динамический массив для хранения элементов сегмента
        Segment* prev;         // Указатель на предыдущий сегмент
        Segment* next;         // Указатель на следующий сегмент

        Segment(int segmentSize) : data(segmentSize), prev(nullptr), next(nullptr) {}
    };

    int segmentSize;          // Максимальное количество элементов в одном сегменте
    Segment* firstSegment;    // Указатель на первый сегмент
    Segment* lastSegment;     // Указатель на последний сегмент
    int count;               // Общее количество элементов в деке
    int firstIndex;          // Индекс первого элемента в первом сегменте
    int lastIndex;           // Индекс последнего элемента в последнем сегменте

    void ensureCapacity() {
        if (count == 0) {  // Если дек пуст
            firstSegment = lastSegment = new Segment(segmentSize);  // Создаем первый сегмент
            firstIndex = 0;    // Начинаем заполнять с начала
            lastIndex = -1;     // Пока нет элементов
        }
        else if (lastIndex == segmentSize - 1) {  // Если последний сегмент заполнен
            Segment* newSegment = new Segment(segmentSize);  // Создаем новый сегмент
            newSegment->prev = lastSegment;  // Связываем с предыдущим
            lastSegment->next = newSegment;  // Связываем предыдущий с новым
            lastSegment = newSegment;        // Делаем новый сегмент последним
            lastIndex = -1;                  // Начинаем заполнять с начала
        }
        else if (firstIndex == 0 && firstSegment->prev == nullptr) {  // Если первый сегмент заполнен с начала
            Segment* newSegment = new Segment(segmentSize);  // Создаем новый сегмент перед первым
            newSegment->next = firstSegment;  // Связываем новый с текущим первым
            firstSegment->prev = newSegment;  // Связываем текущий первый с новым
            firstSegment = newSegment;        // Делаем новый сегмент первым
            firstIndex = segmentSize - 1;    // Начинаем заполнять с конца сегмента
        }
    }

    void freeSegments() {
        Segment* current = firstSegment;
        while (current != nullptr) {
            Segment* next = current->next;
            delete current;
            current = next;
        }
        firstSegment = lastSegment = nullptr;
        count = 0;
        firstIndex = 0;
        lastIndex = -1;
    }

    void copySegments(const SegmentedDeque& other) {
        Segment* current = other.firstSegment;  // Начинаем с первого сегмента исходного дека
        while (current != nullptr) {           // Пока не дойдем до конца
            for (int i = 0; i < current->data.GetSize(); ++i) {  // Перебираем все элементы сегмента
                Append(current->data.Get(i));  // Добавляем каждый элемент в новый дек
            }
            current = current->next;  // Переходим к следующему сегменту
        }
    }

public:
    // Конструкторы
    explicit SegmentedDeque(int segmentSize = 16)
    : segmentSize(segmentSize > 0 ? segmentSize : 16),  // Размер сегмента (минимум 1)
    firstSegment(nullptr), lastSegment(nullptr),         // Инициализация указателей
    count(0), firstIndex(0), lastIndex(-1) {}           // Начальные индексы

// explicit запрещает неявное преобразование типов при вызове конструктора
// 16 - стандартный размер сегмента по умолчанию (эмпирически выбранное значение)

SegmentedDeque(T* items, int count, int segmentSize = 16)
    : segmentSize(segmentSize > 0 ? segmentSize : 16),  // Проверка размера
    firstSegment(nullptr), lastSegment(nullptr),         // Инициализация
    count(0), firstIndex(0), lastIndex(-1) {            // Начальные значения
    for (int i = 0; i < count; ++i) {                   // Добавляем все элементы
        Append(items[i]);
    }
}

SegmentedDeque(const SegmentedDeque& other)
    : segmentSize(other.segmentSize),                   // Копируем размер сегмента
    firstSegment(nullptr), lastSegment(nullptr),        // Инициализация
    count(0), firstIndex(0), lastIndex(-1) {           // Начальные значения
    copySegments(other);                               // Копируем содержимое
}

    // Деструктор
    ~SegmentedDeque() {
        freeSegments();
    }

    // методы Sequence
    T GetFirst() const override {
        if (count == 0) throw IndexOutOfRangeException();
        return firstSegment->data.Get(firstIndex);
    }

    T GetLast() const override {
        if (count == 0) throw IndexOutOfRangeException();
        return lastSegment->data.Get(lastIndex);
    }

    T Get(int index) const override {
        if (index < 0 || index >= count) throw IndexOutOfRangeException();  // Проверка границ
    
        Segment* current = firstSegment;  // Начинаем с первого сегмента
        int remaining = index;           // Сколько элементов нужно пропустить
        int currentFirst = firstIndex;   // Индекс первого элемента в текущем сегменте
    
        // Пока нужно пропустить больше элементов, чем есть в текущем сегменте
        while (remaining >= (current->data.GetSize() - currentFirst)) {
            remaining -= (current->data.GetSize() - currentFirst);  // Уменьшаем оставшиеся
            current = current->next;      // Переходим к следующему сегменту
            currentFirst = 0;            // В новом сегменте начинаем с начала
        }
    
        // Возвращаем элемент в найденной позиции
        return current->data.Get(currentFirst + remaining);
    }

    int GetLength() const override {
        return count;
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        if (startIndex < 0 || endIndex >= count || startIndex > endIndex) {
            throw IndexOutOfRangeException();  // Проверка границ
        }
    
        SegmentedDeque<T>* subDeque = new SegmentedDeque<T>(segmentSize);  // Новый дек
        for (int i = startIndex; i <= endIndex; ++i) {  // Для каждого элемента в диапазоне
            subDeque->Append(Get(i));                   // Добавляем в подпоследовательность
        }
    
        return subDeque;  // Возвращаем результат
    }

    Sequence<T>* Concat(Sequence<T>* other) const {
        SegmentedDeque<T>* result = new SegmentedDeque<T>(*this);  // Копируем текущий дек
        for (int i = 0; i < other->GetLength(); ++i) {            // Добавляем все элементы другого дека
            result->Append(other->Get(i));
        }
        return result;  // Возвращаем объединенный дек
    }

    
    Sequence<T>* Append(T item) {
        ensureCapacity();           // Проверяем/создаем место
        lastIndex++;                // Увеличиваем индекс последнего элемента
        lastSegment->data.Set(lastIndex, item);  // Записываем элемент
        count++;                    // Увеличиваем счетчик
        return this;                // Возвращаем указатель на себя
    }
    
    Sequence<T>* Prepend(T item) {
        ensureCapacity();           // Проверяем/создаем место
        firstIndex--;               // Уменьшаем индекс первого элемента
        if (firstIndex < 0) {       // Если вышли за начало
            firstIndex = segmentSize - 1;  // Переходим в конец сегмента
        }
        firstSegment->data.Set(firstIndex, item);  // Записываем элемент
        count++;                    // Увеличиваем счетчик
        return this;                // Возвращаем указатель на себя
    }

    Sequence<T>* InsertAt(T item, int index) override {
        if (index < 0 || index > count) throw IndexOutOfRangeException();  // Проверка границ
    
        if (index == 0) return Prepend(item);  // Вставка в начало
        if (index == count) return Append(item);  // Вставка в конец
    
        // Для вставки в середину создаем новый дек
        SegmentedDeque<T>* newDeque = new SegmentedDeque<T>(segmentSize);
        // Копируем элементы до точки вставки
        for (int i = 0; i < index; ++i) {
            newDeque->Append(Get(i));
        }
        newDeque->Append(item);  // Вставляем новый элемент
        // Копируем оставшиеся элементы
        for (int i = index; i < count; ++i) {
            newDeque->Append(Get(i));
        }
    
        return newDeque;  // Возвращаем новый дек
    }

    
    Sequence<T>* PopFront() {
        if (count == 0) throw IndexOutOfRangeException();

        if (count == 1) {
            count = 0;
            firstIndex = 0;
            lastIndex = -1;
            return this;
        }

        firstIndex++;
        count--;
        if (firstIndex >= segmentSize) {
            Segment* toDelete = firstSegment;
            firstSegment = firstSegment->next;
            firstSegment->prev = nullptr;
            firstIndex = 0;
            delete toDelete;
        }

        return this;
    }

    Sequence<T>* PopBack() {
        if (count == 0) throw IndexOutOfRangeException();

        if (count == 1) {
            count = 0;
            firstIndex = 0;
            lastIndex = -1;
            return this;
        }

        lastIndex--;
        count--;
        if (lastIndex < 0) {
            Segment* toDelete = lastSegment;
            lastSegment = lastSegment->prev;
            lastSegment->next = nullptr;
            lastIndex = segmentSize - 1;
            delete toDelete;
        }

        return this;
    }

   
    void Sort(const std::function<bool(const T&, const T&)>& comparator) {
        DynamicArray<T> tempArray(count);  // Временный массив для сортировки
        for (int i = 0; i < count; ++i) {  // Копируем все элементы
            tempArray.Set(i, Get(i));
        }
    
        // Сортировка пузырьком
        for (int i = 0; i < count - 1; ++i) {
            for (int j = i + 1; j < count; ++j) {
                if (comparator(tempArray.Get(i), tempArray.Get(j))) {  // Если нужно поменять
                    T temp = tempArray.Get(i);  // Меняем местами
                    tempArray.Set(i, tempArray.Get(j));
                    tempArray.Set(j, temp);
                }
            }
        }
    
        freeSegments();  // Очищаем текущие сегменты
        for (int i = 0; i < tempArray.GetSize(); ++i) {  // Заполняем отсортированными элементами
            Append(tempArray.Get(i));
        }
    }

    Sequence<T>* Map(const std::function<T(const T&)>& func) const {
        SegmentedDeque<T>* result = new SegmentedDeque<T>(segmentSize);  // Новый дек
        for (int i = 0; i < count; ++i) {  // Для каждого элемента
            result->Append(func(Get(i)));  // Применяем функцию и добавляем результат
        }
        return result;  // Возвращаем новый дек
    }

    Sequence<T>* Where(const std::function<bool(const T&)>& predicate) const {
        SegmentedDeque<T>* result = new SegmentedDeque<T>(segmentSize);  // Новый дек
        for (int i = 0; i < count; ++i) {  // Для каждого элемента
            T item = Get(i);
            if (predicate(item)) {  // Если условие выполняется
                result->Append(item);  // Добавляем элемент
            }
        }
        return result;  // Возвращаем отфильтрованный дек
    }

    T Reduce(const std::function<T(const T&, const T&)>& func, T initial) const {
        T result = initial;  // Начальное значение
        for (int i = 0; i < count; ++i) {  // Для каждого элемента
            result = func(result, Get(i));  // Применяем функцию аккумуляции
        }
        return result;  // Возвращаем итоговое значение
    }

    bool ContainsSubsequence(Sequence<T>* subsequence) const {
        if (subsequence->GetLength() == 0) return true;  // Пустая подпоследовательность всегда есть
        if (subsequence->GetLength() > count) return false;  // Если подпоследовательность больше - нет
    
        // Проверяем все возможные начальные позиции
        for (int i = 0; i <= count - subsequence->GetLength(); ++i) {
            bool match = true;
            // Проверяем совпадение для текущей позиции
            for (int j = 0; j < subsequence->GetLength(); ++j) {
                if (Get(i + j) != subsequence->Get(j)) {  // Если не совпадает
                    match = false;
                    break;
                }
            }
            if (match) return true;  // Если нашли совпадение
        }
    
        return false;  // Не нашли совпадений
    }

    Sequence<T>* Merge(Sequence<T>* other) const {
        // Объединяем деки
        SegmentedDeque<T>* result = static_cast<SegmentedDeque<T>*>(Concat(other));
        // Сортируем результат
        result->Sort([](const T& a, const T& b) { return a < b; });
        return result;
    }

    Sequence<T>* Instance() const override {
        return new SegmentedDeque<T>(segmentSize);  // Создаем новый экземпляр
    }
};