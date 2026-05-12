#pragma once

#include <cstddef>
#include <memory>
#include <utility>

template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr : private Deleter {  // ← наследуемся для EBO
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
    T* ptr_ = nullptr;
    
    Deleter& deleter_impl() { return *this; }
    const Deleter& deleter_impl() const { return *this; }
};

// =====================================================================
//  Partial specialization for arrays: UniquePtr<T[]>
//  Uses operator[] instead of operator*/operator->.
//  Default deleter is std::default_delete<T[]> (calls delete[]).
// =====================================================================

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> : private Deleter {  // ← наследуемся для EBO
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
    T* ptr_ = nullptr;
    
    Deleter& deleter_impl() { return *this; }
    const Deleter& deleter_impl() const { return *this; }
};

// =================== Free function ===============================

template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args);

// ======================== Constructors ========================

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::UniquePtr() : Deleter(), ptr_(nullptr) {}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::UniquePtr(T* ptr) : Deleter(), ptr_(ptr) {}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::UniquePtr(T* ptr, const Deleter& deleter)
    : Deleter(deleter), ptr_(ptr) {}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::UniquePtr(T* ptr, Deleter&& deleter)
    : Deleter(std::move(deleter)), ptr_(ptr) {}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::UniquePtr(UniquePtr&& other)
    : Deleter(std::move(other.deleter_impl())), ptr_(other.ptr_) {
    other.ptr_ = nullptr;
}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>& UniquePtr<T, Deleter>::operator=(UniquePtr&& other) {
    if (this != &other) {
        reset();
        ptr_ = other.ptr_;
        deleter_impl() = std::move(other.deleter_impl());
        other.ptr_ = nullptr;
    }
    return *this;
}

template <typename T, typename Deleter>
UniquePtr<T, Deleter>::~UniquePtr() {
    if (ptr_) {
        deleter_impl()(ptr_);
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
    return deleter_impl();
}

template <typename T, typename Deleter>
const Deleter& UniquePtr<T, Deleter>::get_deleter() const {
    return deleter_impl();
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
        deleter_impl()(ptr_);
    }
    ptr_ = ptr;
}

template <typename T, typename Deleter>
void UniquePtr<T, Deleter>::swap(UniquePtr& other) {
    std::swap(ptr_, other.ptr_);
    std::swap(deleter_impl(), other.deleter_impl());
}

// ======================== Версия для массивов T[] ========================

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::UniquePtr() : Deleter(), ptr_(nullptr) {}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::UniquePtr(T* ptr) : Deleter(), ptr_(ptr) {}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::UniquePtr(T* ptr, const Deleter& deleter)
    : Deleter(deleter), ptr_(ptr) {}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::UniquePtr(T* ptr, Deleter&& deleter)
    : Deleter(std::move(deleter)), ptr_(ptr) {}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::UniquePtr(UniquePtr&& other)
    : Deleter(std::move(other.deleter_impl())), ptr_(other.ptr_) {
    other.ptr_ = nullptr;
}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>& UniquePtr<T[], Deleter>::operator=(UniquePtr&& other) {
    if (this != &other) {
        reset();
        ptr_ = other.ptr_;
        deleter_impl() = std::move(other.deleter_impl());
        other.ptr_ = nullptr;
    }
    return *this;
}

template <typename T, typename Deleter>
UniquePtr<T[], Deleter>::~UniquePtr() {
    if (ptr_) {
        deleter_impl()(ptr_);
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
    return deleter_impl();
}

template <typename T, typename Deleter>
const Deleter& UniquePtr<T[], Deleter>::get_deleter() const {
    return deleter_impl();
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
        deleter_impl()(ptr_);
    }
    ptr_ = ptr;
}

template <typename T, typename Deleter>
void UniquePtr<T[], Deleter>::swap(UniquePtr& other) {
    std::swap(ptr_, other.ptr_);
    std::swap(deleter_impl(), other.deleter_impl());
}

// ======================== make_unique ========================

template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}