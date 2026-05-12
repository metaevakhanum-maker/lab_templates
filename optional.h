#pragma once

#include <new>
#include <type_traits>
#include <utility>

template <typename T>
class Optional {
public:
    // ======================== Constructors ========================

    Optional();
    Optional(const T& value);
    Optional(T&& value);

    // =================== Copy and Move ===========================

    Optional(const Optional& other);
    Optional(Optional&& other);
    Optional& operator=(const Optional& other);
    Optional& operator=(Optional&& other);
    Optional& operator=(const T& value);
    Optional& operator=(T&& value);

    // =================== Destructor ==============================

    ~Optional();

    // =================== Observers ===============================

    bool has_value() const;
    explicit operator bool() const;

    T&       value();
    const T& value() const;

    T        value_or(const T& default_value) const;

    T&       operator*();
    const T& operator*() const;

    T*       operator->();
    const T* operator->() const;

    // =================== Modifiers ===============================

    template <typename... Args>
    T& emplace(Args&&... args);

    void reset();
    void swap(Optional& other);

    // =================== Comparison ==============================

    bool operator==(const Optional& rhs) const;
    bool operator!=(const Optional& rhs) const;

private:
    alignas(T) unsigned char storage_[sizeof(T)];
    bool has_value_ = false;
};
// ======================== РЕАЛИЗАЦИЯ ========================

// Конструктор по умолчанию - пустой Optional
template <typename T>
Optional<T>::Optional() : has_value_(false) {}

// Конструктор от константной ссылки
template <typename T>
Optional<T>::Optional(const T& value) : has_value_(true) {
    new (storage_) T(value);  // placement new
}

// Конструктор от rvalue-ссылки
template <typename T>
Optional<T>::Optional(T&& value) : has_value_(true) {
    new (storage_) T(std::move(value));
}

// Конструктор копирования
template <typename T>
Optional<T>::Optional(const Optional& other) : has_value_(other.has_value_) {
    if (has_value_) {
        new (storage_) T(other.value());
    }
}

// Конструктор перемещения
template <typename T>
Optional<T>::Optional(Optional&& other) : has_value_(other.has_value_) {
    if (has_value_) {
        new (storage_) T(std::move(other.value()));
    }
    other.reset();
}

// Оператор присваивания копированием
template <typename T>
Optional<T>& Optional<T>::operator=(const Optional& other) {
    if (this != &other) {
        if (has_value_ && other.has_value_) {
            value() = other.value();
        } else if (has_value_ && !other.has_value_) {
            reset();
        } else if (!has_value_ && other.has_value_) {
            new (storage_) T(other.value());
            has_value_ = true;
        }
    }
    return *this;
}

// Оператор присваивания перемещением
template <typename T>
Optional<T>& Optional<T>::operator=(Optional&& other) {
    if (this != &other) {
        reset();
        if (other.has_value_) {
            new (storage_) T(std::move(other.value()));
            has_value_ = true;
            other.reset();
        }
    }
    return *this;
}

// Оператор присваивания от значения (константная ссылка)
template <typename T>
Optional<T>& Optional<T>::operator=(const T& value) {
    if (has_value_) {
        this->value() = value;  
    } else {
        new (storage_) T(value);
        has_value_ = true;
    }
    return *this;
}

// Оператор присваивания от значения (rvalue-ссылка)
template <typename T>
Optional<T>& Optional<T>::operator=(T&& value) {
    if (has_value_) {
        this->value() = std::move(value);  
    } else {
        new (storage_) T(std::move(value));
        has_value_ = true;
    }
    return *this;
}

// Деструктор
template <typename T>
Optional<T>::~Optional() {
    reset();
}

// Проверка наличия значения
template <typename T>
bool Optional<T>::has_value() const {
    return has_value_;
}

// Оператор приведения к bool
template <typename T>
Optional<T>::operator bool() const {
    return has_value_;
}

// Получение значения (неконстантная версия)
template <typename T>
T& Optional<T>::value() {
    if (!has_value_) {
        throw std::bad_optional_access();
    }
    return *reinterpret_cast<T*>(storage_);
}

// Получение значения (константная версия)
template <typename T>
const T& Optional<T>::value() const {
    if (!has_value_) {
        throw std::bad_optional_access();
    }
    return *reinterpret_cast<const T*>(storage_);
}

// Получение значения или значения по умолчанию
template <typename T>
T Optional<T>::value_or(const T& default_value) const {
    return has_value_ ? value() : default_value;
}

// Оператор разыменования (неконстантный)
template <typename T>
T& Optional<T>::operator*() {
    return value();
}

// Оператор разыменования (константный)
template <typename T>
const T& Optional<T>::operator*() const {
    return value();
}

// Оператор доступа к членам (неконстантный)
template <typename T>
T* Optional<T>::operator->() {
    return &value();
}

// Оператор доступа к членам (константный)
template <typename T>
const T* Optional<T>::operator->() const {
    return &value();
}

// Создание значения на месте
template <typename T>
template <typename... Args>
T& Optional<T>::emplace(Args&&... args) {
    reset();
    new (storage_) T(std::forward<Args>(args)...);
    has_value_ = true;
    return value();
}

// Сброс Optional (удаление значения)
template <typename T>
void Optional<T>::reset() {
    if (has_value_) {
        value().~T();
        has_value_ = false;
    }
}

// Обмен содержимым с другим Optional
template <typename T>
void Optional<T>::swap(Optional& other) {
    using std::swap;
    if (has_value_ && other.has_value_) {
        swap(value(), other.value());
    } else if (has_value_ && !other.has_value_) {
        new (other.storage_) T(std::move(value()));
        other.has_value_ = true;
        reset();
    } else if (!has_value_ && other.has_value_) {
        new (storage_) T(std::move(other.value()));
        has_value_ = true;
        other.reset();
    }
    // если оба пустые - ничего не делаем
}

// Оператор равенства
template <typename T>
bool Optional<T>::operator==(const Optional& rhs) const {
    if (has_value_ != rhs.has_value_) {
        return false;
    }
    if (!has_value_ && !rhs.has_value_) {
        return true;
    }
    return value() == rhs.value();
}

// Оператор неравенства
template <typename T>
bool Optional<T>::operator!=(const Optional& rhs) const {
    return !(*this == rhs);
}