#ifndef UNIQUE_PTR_HPP
#define UNIQUE_PTR_HPP

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace not_std {
    struct deleter {
        deleter() = default;
        virtual void release() = 0;
        virtual ~deleter() = default;
    };

    template<typename T, typename Del>
    struct deleter_object: public deleter {
        deleter_object(T* ptr, const Del& del): ptr(ptr), del(del) {}

        deleter_object(T* ptr, Del&& del): ptr(ptr), del(std::move(del)) {}

        void release() override {
            delete ptr;
        }

        ~deleter_object() override {
            if (ptr)
                del(ptr);
        }

        T* ptr;
        Del del;
    };

    template<typename T>
    struct unique_ptr {
    public:
        using pointer = T*;
        using element_type = T;

        unique_ptr() noexcept {}

        unique_ptr(std::nullptr_t) noexcept {}

        unique_ptr(T* ptr) noexcept: ptr(ptr) {}

        template<typename Del>
        unique_ptr(T* ptr, const Del& del): ptr(ptr), del(new deleter_object<T, Del>(ptr, del)) {}

        template<typename Del>
        unique_ptr(T* ptr, Del&& del): ptr(ptr), del(new deleter_object<T, Del>(ptr, std::move(del))) {}

        unique_ptr(unique_ptr<T>&& u) noexcept {
            std::swap(ptr, u.ptr);
            std::swap(del, u.del);
        };

        template<typename U>
        unique_ptr(typename std::enable_if<std::is_convertible<U*, T*>::value, unique_ptr<U>&&> u) noexcept {
            std::swap(ptr, u.ptr);
            std::swap(del, u.del);
        };

        ~unique_ptr() {
            destroy();
        }

        unique_ptr& operator=(unique_ptr<T>&& u) noexcept {
            destroy();
            std::swap(ptr, u.ptr);
            std::swap(del, u.del);
        }

        T* release() noexcept {
            T* temp_ptr = ptr;
            ptr = nullptr;
            if (del)
                del->release();
            destroy();
            return temp_ptr;
        }

        void reset() noexcept {
            destroy();
        }

        void reset(std::nullptr_t) noexcept {
            destroy();
        }

        void reset(T* ptr) {
            unique_ptr<T> other(ptr);
            swap(other);
        }

        template<typename Del>
        void reset(T* ptr, Del del) {
            unique_ptr<T> other(ptr, del);
            swap(other);
        }

        void swap(unique_ptr<T>& other) noexcept {
            std::swap(ptr, other.ptr);
            std::swap(del, other.del);
        }

        T* get() const noexcept {
            return ptr;
        }

        T& operator*() const {
            if (!ptr)
                throw std::runtime_error("can't dereference a null pointer");
            return *ptr;
        }

        T* operator->() const noexcept {
            return ptr;
        }

        template<typename U, typename... Args>
        friend unique_ptr<U> make_unique(Args... args);
    private:
        T* ptr = nullptr;
        deleter* del = nullptr;

        void destroy() {
            if (del)
                delete del;
            else
                delete ptr;
            del = nullptr;
            ptr = nullptr;
        }
    };

    template<typename T, typename... Args>
    unique_ptr<T> make_unique(Args... args) {
        return unique_ptr<T>(new T(std::forward<Args...>(args...)));
    };
};

#endif // UNIQUE_PTR_HPP
