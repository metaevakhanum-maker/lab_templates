#pragma once

#include <cstddef>
#include <memory>
#include <utility>

template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr : private Deleter {
public:
    // ======================== Constructors ========================

    UniquePtr();
    explicit UniquePtr(T* ptr);
    UniquePtr(T* ptr, const Deleter& deleter);
    UniquePtr(T* ptr, Deleter&& deleter);

    // =================== No copy ==================================

    UniquePtr(const UniquePtr&)            = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // =================== Move ====================================

    UniquePtr(UniquePtr&& other);
    UniquePtr& operator=(UniquePtr&& other);

    // =================== Destructor ==============================

    ~UniquePtr();

    // =================== Observers ===============================

    T*       get();
    const T* get() const;

    explicit operator bool() const;

    T&       operator*();
    const T& operator*() const;

    T*       operator->();
    const T* operator->() const;

    Deleter&       get_deleter();
    const Deleter& get_deleter() const;

    // =================== Modifiers ===============================

    T*   release();
    void reset(T* ptr = nullptr);
    void swap(UniquePtr& other);

private:
    T* ptr_;
    // Deleter уже в базовом классе, не нужно отдельное поле
};

// =====================================================================
//  Partial specialization for arrays: UniquePtr<T[]>
// =====================================================================

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> : private Deleter {
public:
    // ======================== Constructors ========================

    UniquePtr();
    explicit UniquePtr(T* ptr);
    UniquePtr(T* ptr, const Deleter& deleter);
    UniquePtr(T* ptr, Deleter&& deleter);

    // =================== No copy ==================================

    UniquePtr(const UniquePtr&)            = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // =================== Move ====================================

    UniquePtr(UniquePtr&& other);
    UniquePtr& operator=(UniquePtr&& other);

    // =================== Destructor ==============================

    ~UniquePtr();

    // =================== Observers ===============================

    T*       get();
    const T* get() const;

    explicit operator bool() const;

    T&       operator[](size_t index);
    const T& operator[](size_t index) const;

    Deleter&       get_deleter();
    const Deleter& get_deleter() const;

    // =================== Modifiers ===============================

    T*   release();
    void reset(T* ptr = nullptr);
    void swap(UniquePtr& other);

private:
    T* ptr_;
};

// =================== Free function ===============================

template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args);

// ======================== Constructors ========================

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::UniquePtr() 
    : Deleter(), ptr_(nullptr) {}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::UniquePtr(T* ptr) 
    : Deleter(), ptr_(ptr) {}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::UniquePtr(T* ptr, const Deleter& deleter)
    : Deleter(deleter), ptr_(ptr) {}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::UniquePtr(T* ptr, Deleter&& deleter)
    : Deleter(std::move(deleter)), ptr_(ptr) {}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::UniquePtr(UniquePtr&& other)
    : Deleter(std::move(other.get_deleter())), ptr_(other.ptr_) {
    other.ptr_ = nullptr;
}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>& UniquePtr<T, Deleter>::operator=(UniquePtr&& other) {
    if (this != &other) {
        reset();
        ptr_ = other.ptr_;
        get_deleter() = std::move(other.get_deleter());
        other.ptr_ = nullptr;
    }
    return *this;
}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::~UniquePtr() {
    if (ptr_) {
        get_deleter()(ptr_);
    }
}

template <typename T, typename Deleter>
T* UniquePtr<T, Deleter>::get() {
    return ptr_;
}

template <typename T, typename Deleter>
const T* UniquePtr<T, Deleter>::get() const {
    return ptr_;
}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::operator bool() const {
    return ptr_ != nullptr;
}

template <typename T, typename Deleter>
T& UniquePtr<T, Deleter>::operator*() {
    return *ptr_;
}

template <typename T, typename Deleter>
const T& UniquePtr<T, Deleter>::operator*() const {
    return *ptr_;
}

template <typename T, typename Deleter>
T* UniquePtr<T, Deleter>::operator->() {
    return ptr_;
}

template <typename T, typename Deleter>
const T* UniquePtr<T, Deleter>::operator->() const {
    return ptr_;
}

template <typename T, typename Deleter>
Deleter& UniquePtr<T, Deleter>::get_deleter() {
    return *this;
}

template <typename T, typename Deleter>
const Deleter& UniquePtr<T, Deleter>::get_deleter() const {
    return *this;
}

template <typename T, typename Deleter>
T* UniquePtr<T, Deleter>::release() {
    T* old_ptr = ptr_;
    ptr_ = nullptr;
    return old_ptr;
}

template <typename T, typename Deleter>
void UniquePtr<T, Deleter>::reset(T* ptr) {
    if (ptr_) {
        get_deleter()(ptr_);
    }
    ptr_ = ptr;
}

template <typename T, typename Deleter>
void UniquePtr<T, Deleter>::swap(UniquePtr& other) {
    std::swap(ptr_, other.ptr_);
    std::swap(get_deleter(), other.get_deleter());
}

// ======================== Версия для массивов T[] ========================

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::UniquePtr() 
    : Deleter(), ptr_(nullptr) {}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::UniquePtr(T* ptr) 
    : Deleter(), ptr_(ptr) {}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::UniquePtr(T* ptr, const Deleter& deleter)
    : Deleter(deleter), ptr_(ptr) {}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::UniquePtr(T* ptr, Deleter&& deleter)
    : Deleter(std::move(deleter)), ptr_(ptr) {}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::UniquePtr(UniquePtr&& other)
    : Deleter(std::move(other.get_deleter())), ptr_(other.ptr_) {
    other.ptr_ = nullptr;
}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>& UniquePtr<T[], Deleter>::operator=(UniquePtr&& other) {
    if (this != &other) {
        reset();
        ptr_ = other.ptr_;
        get_deleter() = std::move(other.get_deleter());
        other.ptr_ = nullptr;
    }
    return *this;
}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::~UniquePtr() {
    if (ptr_) {
        get_deleter()(ptr_);
    }
}

template <typename T, typename Deleter>
T* UniquePtr<T[], Deleter>::get() {
    return ptr_;
}

template <typename T, typename Deleter>
const T* UniquePtr<T[], Deleter>::get() const {
    return ptr_;
}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::operator bool() const {
    return ptr_ != nullptr;
}

template <typename T, typename Deleter>
T& UniquePtr<T[], Deleter>::operator[](size_t index) {
    return ptr_[index];
}

template <typename T, typename Deleter>
const T& UniquePtr<T[], Deleter>::operator[](size_t index) const {
    return ptr_[index];
}

template <typename T, typename Deleter>
Deleter& UniquePtr<T[], Deleter>::get_deleter() {
    return *this;
}

template <typename T, typename Deleter>
const Deleter& UniquePtr<T[], Deleter>::get_deleter() const {
    return *this;
}

template <typename T, typename Deleter>
T* UniquePtr<T[], Deleter>::release() {
    T* old_ptr = ptr_;
    ptr_ = nullptr;
    return old_ptr;
}

template <typename T, typename Deleter>
void UniquePtr<T[], Deleter>::reset(T* ptr) {
    if (ptr_) {
        get_deleter()(ptr_);
    }
    ptr_ = ptr;
}

template <typename T, typename Deleter>
void UniquePtr<T[], Deleter>::swap(UniquePtr& other) {
    std::swap(ptr_, other.ptr_);
    std::swap(get_deleter(), other.get_deleter());
}

// ======================== make_unique ========================

template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}