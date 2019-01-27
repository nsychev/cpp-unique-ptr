#include "unique_ptr.hpp"
#include <iostream>
#include <cassert>

struct deleter {
    void operator()(int *k) {
        std::cerr << "Functor deleter" << std::endl;
        delete k;
    }
};

void f_deleter(int *k) {
    std::cerr << "Function deleter" << std::endl;
    delete k;
}

int main() {
    {
        not_std::unique_ptr<int> u = not_std::make_unique<int>(1);
        int* k = u.release();
        assert(*k == 1);
        delete k;
    }

    {
        int *k = new int(123);
        not_std::unique_ptr<int> p(k, &f_deleter);
        assert(*p == 123);
    }

    {
        int *k = new int(3);
        not_std::unique_ptr<int> p(k, deleter());
        assert(*p == 3);
    }

    {
        int *k = new int(3);
        not_std::unique_ptr<int> p(k, [](int *ptr) { std::cerr << "Lambda deleter" << std::endl; delete ptr; });
        std::cerr << *p << std::endl;
    }

    {
        not_std::unique_ptr<int> first(new int(1337), &f_deleter);
        not_std::unique_ptr<int> second;
        second.reset(first.release());
        assert(first.get() == nullptr);
        assert(*second == 1337);
    }

    return 0;
}