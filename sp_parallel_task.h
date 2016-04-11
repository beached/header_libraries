#pragma once

#include <deque>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>


namespace sp {
template <typename N>
array<N, 4> identity_element(const multiply_2x2<N>&) { return { N(1), N(0), N(0), N(1) }; }

template <typename R, typename N>
R fibonacci(N n) {
    if (n == 0) return R(0);
    return power(array<R, 4>{ 1, 1, 1, 0 }, N(n - 1), multiply_2x2<R>())[0];
}

using lock_t = unique_lock<mutex>;

class notification_queue {
    deque<function<void()>> _q;
    bool                    _done{false};
    mutex                   _mutex;
    condition_variable      _ready;
    
public:
    bool try_pop(function<void()>& x) {
        lock_t lock{_mutex, try_to_lock};
        if (!lock || _q.empty()) return false;
        x = move(_q.front());
        _q.pop_front();
        return true;
    }
    
    template<typename F>
    bool try_push(F&& f) {
        {
            lock_t lock{_mutex, try_to_lock};
            if (!lock) return false;
            _q.emplace_back(forward<F>(f));
        }
        _ready.notify_one();
        return true;
    }
    
    void done() {
        {
            unique_lock<mutex> lock{_mutex};
            _done = true;
        }
        _ready.notify_all();
    }
    
    bool pop(function<void()>& x) {
        lock_t lock{_mutex};
        while (_q.empty() && !_done) _ready.wait(lock);
        if (_q.empty()) return false;
        x = move(_q.front());
        _q.pop_front();
        return true;
    }
    
    template<typename F>
    void push(F&& f) {
        {
            lock_t lock{_mutex};
            _q.emplace_back(forward<F>(f));
        }
        _ready.notify_one();
    }
};

/**************************************************************************************************/

class task_system {
    const unsigned              _count{thread::hardware_concurrency()};
    vector<thread>              _threads;
    vector<notification_queue>  _q{_count};
    atomic<unsigned>            _index{0};
    
    void run(unsigned i) {
        while (true) {
            function<void()> f;
            
            for (unsigned n = 0; n != _count * 32; ++n) {
                if (_q[(i + n) % _count].try_pop(f)) break;
            }
            if (!f && !_q[i].pop(f)) break;
            
            f();
        }
    }
    
public:
    task_system() {
        for (unsigned n = 0; n != _count; ++n) {
            _threads.emplace_back([&, n]{ run(n); });
        }
    }
    
    ~task_system() {
        for (auto& e : _q) e.done();
        for (auto& e : _threads) e.join();
    }
    
    template <typename F>
    void async_(F&& f) {
        auto i = _index++;
        
        for (unsigned n = 0; n != _count; ++n) {
            if (_q[(i + n) % _count].try_push(forward<F>(f))) return;
        }
        
        _q[i % _count].push(forward<F>(f));
    }
};

/**************************************************************************************************/

task_system _system;

/**************************************************************************************************/

template <typename>
struct result_of_;

template <typename R, typename... Args>
struct result_of_<R(Args...)> { using type = R; };

template <typename F>
using result_of_t_ = typename result_of_<F>::type;

/**************************************************************************************************/

template <typename R>
struct shared_base {
    vector<R> _r; // optional
    mutex _mutex;
    condition_variable _ready;
    vector<function<void()>> _then;
    
    virtual ~shared_base() { }
    
    void set(R&& r) {
        vector<function<void()>> then;
        {
            lock_t lock{_mutex};
            _r.push_back(move(r));
            swap(_then, then);
        }
        _ready.notify_all();
        for (const auto& f : then) _system.async_(move(f));
    }
    
    template <typename F>
    void then(F&& f) {
        bool resolved{false};
        {
            lock_t lock{_mutex};
            if (_r.empty()) _then.push_back(forward<F>(f));
            else resolved = true;
        }
        if (resolved) _system.async_(move(f));
    }
    
    const R& get() {
        lock_t lock{_mutex};
        while (_r.empty()) _ready.wait(lock);
        return _r.back();
    }
};

template <typename> struct shared; // not defined

template <typename R, typename... Args>
struct shared<R(Args...)> : shared_base<R> {
    function<R(Args...)> _f;
    
    template<typename F>
    shared(F&& f) : _f(forward<F>(f)) { }
    
    template <typename... A>
    void operator()(A&&... args) {
        this->set(_f(forward<A>(args)...));
        _f = nullptr;
    }
};

template <typename> class packaged_task; //not defined
template <typename> class future;

template <typename S, typename F>
auto package(F&& f) -> pair<packaged_task<S>, future<result_of_t_<S>>>;

template <typename R>
class future {
    shared_ptr<shared_base<R>> _p;
    
    template <typename S, typename F>
    friend auto package(F&& f) -> pair<packaged_task<S>, future<result_of_t_<S>>>;
    
    explicit future(shared_ptr<shared_base<R>> p) : _p(move(p)) { }
 public:
    future() = default;
    
    template <typename F>
    auto then(F&& f) {
        auto pack = package<result_of_t<F(R)>()>([p = _p, f = forward<F>(f)](){
            return f(p->_r.back());
        });
        _p->then(move(pack.first));
        return pack.second;
    }
    
    const R& get() const { return _p->get(); }
};

template<typename R, typename ...Args >
class packaged_task<R (Args...)> {
    weak_ptr<shared<R(Args...)>> _p;
    
    template <typename S, typename F>
    friend auto package(F&& f) -> pair<packaged_task<S>, future<result_of_t_<S>>>;
    
    explicit packaged_task(weak_ptr<shared<R(Args...)>> p) : _p(move(p)) { }
    
 public:
    packaged_task() = default;
    
    template <typename... A>
    void operator()(A&&... args) const {
        auto p = _p.lock();
        if (p) (*p)(forward<A>(args)...);
    }
};

template <typename S, typename F>
auto package(F&& f) -> pair<packaged_task<S>, future<result_of_t_<S>>> {
    auto p = make_shared<shared<S>>(forward<F>(f));
    return make_pair(packaged_task<S>(p), future<result_of_t_<S>>(p));
}

template <typename F, typename ...Args>
auto async(F&& f, Args&&... args)
{
    using result_type = result_of_t<F (Args...)>;
    using packaged_type = packaged_task<result_type()>;
    
    auto pack = package<result_type()>(bind(forward<F>(f), forward<Args>(args)...));
    
    _system.async_(move(get<0>(pack)));
    return get<1>(pack);
}

int main() {
    future<cpp_int> x = async([]{ return fibonacci<cpp_int>(100); });
    
    future<cpp_int> y = x.then([](const cpp_int& x){ return cpp_int(x * 2); });
    future<cpp_int> z = x.then([](const cpp_int& x){ return cpp_int(x / 15); });
                                                            
    cout << y.get() << endl;
    cout << z.get() << endl;
}

/**************************************************************************************************/

#endif

#if 0
#include <iostream>
#include <boost/multiprecision/integer.hpp>
using namespace std;
template <typename T, typename N, typename O>
T power(T x, N n, O op)
{
    if (n == 0) return identity_element(op);
    
    while ((n & 1) == 0) {
        n >>= 1;
        x = op(x, x);
    }
    
    T result = x;
    n >>= 1;
    while (n != 0) {
        x = op(x, x);
        if ((n & 1) != 0) result = op(result, x);
        n >>= 1;
    }
    return result;
}
template <typename N>
struct multiply_2x2 {
    array<N, 4> operator()(const array<N, 4>& x, const array<N, 4>& y)
    {
        return { x[0] * y[0] + x[1] * y[2], x[0] * y[1] + x[1] * y[3],
                 x[2] * y[0] + x[3] * y[2], x[2] * y[1] + x[3] * y[3] };
    }
};
template <typename N>
array<N, 4> identity_element(const multiply_2x2<N>&) { return { N(1), N(0), N(0), N(1) }; }
template <typename R, typename N>
R fibonacci(N n) {
    if (n == 0) return R(0);
    return power(array<R, 4>{ 1, 1, 1, 0 }, N(n - 1), multiply_2x2<R>())[0];
}
#include <boost/multiprecision/cpp_int.hpp>
using namespace boost::multiprecision;
#include <boost/thread/future.hpp>
using namespace std::chrono;
int main() {
    
    auto x = boost::async([]{ return fibonacci<cpp_int>(1'000'000); });
    auto y = boost::async([]{ return fibonacci<cpp_int>(2'000'000); });
    
    auto z = when_all(std::move(x), std::move(y)).then([](auto f){
        auto t = f.get();
        return cpp_int(get<0>(t).get() * get<1>(t).get());
    });
    
    cout << z.get() << endl;
    
    // auto z = boost::when_all([](auto x){ }, x, y);
    
#if 0
    z.then([](auto r){
            return get<0>(r.get()).get() *  get<1>(r.get()).get();
    });
#endif
    
   // cout << z.get() << endl;
    
    // Do something
    
    //y.wait();
    //z.wait();
#if 0
    
    this_thread::sleep_for(seconds(60));
    // y.wait();
    auto start = chrono::high_resolution_clock::now();
    
    auto x = fibonacci<cpp_int>(1'000'000);
    
    cout << chrono::duration_cast<chrono::milliseconds>
    (chrono::high_resolution_clock::now()-start).count() << endl;
    
    cout << x << endl;
#endif
    
#if 0
    
    future<cpp_int> x = async(fibonacci<cpp_int, int>, 1'000'000);
    
    // Do something
    
    auto start = chrono::high_resolution_clock::now();
    
    x.wait();
    
    cout << chrono::duration_cast<chrono::milliseconds>
    (chrono::high_resolution_clock::now()-start).count() << endl;
    
    
    cout << x.get() << endl;
#endif
#if 0
    boost::promise<int> p;
    boost::future<int> x = p.get_future();
    auto y = x.then([](auto x){ cout << "then 1: " << x.get() << endl; });
    x.then([](auto x){ cout << "then 2: " << x.get() << endl; });
    p.set_value(5);
    y.wait();
#endif
}
#endif





#if 0
#include <iostream>
using namespace std;
int main() {
    auto body = [n = 10]() mutable {
        cout << n-- << endl;
        return n != 0;
    };
    
    while (body()) ;
}
#endif


#if 0
#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <new>
#include <utility>
#include <string>
#include <vector>
#include <adobe/forest.hpp>
#include <adobe/algorithm/for_each_position.hpp>
using namespace std;
class cirque {
  public:
    class iterator { }; // write this part
    class const_iterator { }; // write this part
    template <typename I>
    iterator insert(const_iterator p, I f, I l) {
        return insert(p, f, l, typename iterator_traits<I>::iterator_category());
    }
  private:
    template <typename I>
    iterator insert(const_iterator p, I f, I l, input_iterator_tag) {
        /* write this part */
    }
    template <typename I>
    iterator insert(const_iterator p, I f, I l, forward_iterator_tag) {
        /* write this part */
    }
};
using namespace adobe;
template <typename R>
void output(const R& r)
{
    cout << "digraph 'forest' {" << endl;
        for_each_position(r, [&](const auto& n) {
            cout << "  " << *n << " -> { ";
            for(const auto& e : child_range(n.base())) cout << e << "; ";
            cout << "};" << endl;
        });
    cout << "}" << endl;
}
int main() {
    typedef adobe::forest<std::string> forest;
    typedef forest::iterator iterator;
    forest f;
    iterator i (f.begin());
    i = adobe::trailing_of(f.insert(i, "grandmother"));
    {
        iterator p (adobe::trailing_of(f.insert(i, "mother")));
        f.insert(p, "me");
        f.insert(p, "sister");
        f.insert(p, "brother");
    }
    {
        iterator p (adobe::trailing_of(f.insert(i, "aunt")));
        f.insert(p, "cousin");
    }
    f.insert(i, "uncle");
    
    output(adobe::preorder_range(f));
}
#endif


#if 0
#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <new>
#include <utility>
#include <string>
#include <vector>
using namespace std;
template <typename I, typename F>
F uninitialized_move(I f, I l, F o) {
    return uninitialized_copy(make_move_iterator(f), make_move_iterator(l), o);
}
template <typename I, typename F>
F uninitialized_move_backward(I f, I l, F o) {
    return uninitialized_move(reverse_iterator<I>(l), reverse_iterator<I>(f),
        reverse_iterator<F>(o)).base();
}
template <typename I, typename F>
F uninitialized_copy_backward(I f, I l, F o) {
    return uninitialized_copy(reverse_iterator<I>(l), reverse_iterator<I>(f),
        reverse_iterator<F>(o)).base();
}
/*
    Modulo arithemtic: a and b are in the range [0, n), result is (a op b) mod n.
*/
size_t add_mod(size_t a, size_t b, size_t n) {
    return (a < n - b) ? a + b : a - (n - b);
}
size_t sub_mod(size_t a, size_t b, size_t n) {
    return (a < b) ? a + (n - b) : a - b;
}
size_t neg_mod(size_t a, size_t n) { return n - a; }
template <typename T, // T models container with capacity
          typename N> // N is size_type(T)
void grow(T& x, N n) { x.reserve(max(x.size() + n, x.capacity() * 2)); }
template <typename T>
class cirque {
    struct remote {
        size_t _allocated;
        size_t _size;
        size_t _begin;
        aligned_storage<sizeof(T)> _buffer;
        T* buffer() { return static_cast<T*>(static_cast<void*>(&_buffer));}
        size_t capacity() { return _allocated - 1; }
        auto data_index() const -> tuple<size_t, size_t, size_t, size_t> {
            auto n = _allocated;
            auto f = _begin;
            auto l = add_mod(f, _size, n);
            if (l < f) return { f, n, 0, l };
            return { f, l, 0, 0 };
        }
    };
    unique_ptr<remote> _object{nullptr};
    T* buffer() const { return _object ? _object->buffer() : nullptr; }
    size_t begin_index() const { return _object ? _object->_begin : 0; }
    void set_size(size_t n) { if (_object) _object->_size = n; }
    void set_begin(size_t n) { _object->_begin = n; }
    auto _data() const -> tuple<T*, T*, T*, T*> {
        if (!_object) return { nullptr, nullptr, nullptr, nullptr };
        auto n = _object->_allocated;
        auto f = _object->_begin;
        auto l = add_mod(f, _object->_size, n);
        auto p = _object->buffer();
        if ( l < f ) return { p + f, p + n, p, p + l };
        return { p + f, p + l, nullptr, nullptr };
    }
    void grow(size_t n) { reserve(max(size() + n, capacity() * 2)); }
  public:
    template <typename U>
    class iterator_ {
        friend class cirque;
        remote* _object{nullptr};
        size_t  _i{0};
        iterator_(remote* object, size_t index) :
            _object(object), _i{index} { }
        size_t add(ptrdiff_t x) {
            if (!_object) return 0;
            if (x < 0) return sub_mod(_i, -x, _object->_allocated);
            return add_mod(_i, x, _object->_allocated);
        }
      public:
        using difference_type = ptrdiff_t;
        using value_type = U;
        using pointer = U*;
        using reference = U&;
        using iterator_category = random_access_iterator_tag;
        iterator_() = default;
        template <typename V>
        iterator_(const iterator_<V>& x, enable_if_t<is_convertible<V*, U*>::value>* = 0) :
            _i{x._i}, _object{x._object} { }
        // copy and assignment defaulted
        reference operator*() const { return *(_object->buffer() + _i); }
        pointer operator->() const { return &*(*this); }
        iterator_& operator++() { return *this += 1; }
        iterator_ operator++(int) { auto result = *this; ++(*this); return result; }
        iterator_& operator--() { return *this -= 1; }
        iterator_ operator--(int) { auto result = *this; --(*this); return result; }
        iterator_& operator+=(difference_type x) { _i = add(x); return *this;}
        iterator_& operator-=(difference_type x) { return *this += -x;}
        reference operator[](difference_type n) const { return *(*this + n); }
        friend inline iterator_ operator+(iterator_ x, difference_type y) { return x += y; }
        friend inline iterator_ operator+(difference_type x, iterator_ y) { return y + x; }
        friend inline iterator_ operator-(iterator_ x, difference_type y) { return x -= y; }
        friend inline difference_type operator-(const iterator_& x, const iterator_& y) {
            if (!x._object) return 0;
            assert(y._object && "iterators must be in same container");
            size_t f, l;
            tie(f, l, ignore, ignore) = x._object->data_index();
            bool x_block = f <= x._i && x._i <= l;
            bool y_block = f <= y._i && y._i <= l;
            if (x_block == y_block) return x._i - y._i;
            return x._object->_allocated - (y._i - x._i);
        }
        friend inline bool operator==(const iterator_& x, const iterator_& y) {
            return x._i == y._i;
        }
        friend inline bool operator!=(const iterator_& x, const iterator_& y) { return !(x == y); }
        friend inline bool operator<(const iterator_& x, const iterator_& y) {
            if (!x._object) return false;
            assert(y._object && "iterators must be in same container");
            size_t f, l;
            tie(f, l, ignore, ignore) = x._object->data_index();
            bool x_block = f <= x._i && x._i <= l;
            bool y_block = f <= y._i && y._i <= l;
            if (x_block == y_block) return x._i < y._i;
            return x_block;
        }
        friend inline bool operator>(const iterator_& x, const iterator_& y) { return y < x; }
        friend inline bool operator<=(const iterator_& x, const iterator_& y) { return !(y < x); }
        friend inline bool operator>=(const iterator_& x, const iterator_& y) { return !(x < y); }
    };
    // types:
    using reference = T&;
    using const_reference = const T&;
    using iterator = iterator_<T>;
    using const_iterator = iterator_<const T>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    // construct/copy/destroy:
    cirque() = default;
    explicit cirque(size_type n) : cirque(n, T()) { }
    cirque(size_type n, const T& x) {
        reserve(n);
        uninitialized_fill_n(buffer(), n, x);
        set_size(n);
    }
    template<typename I>
    cirque(I f, I l, enable_if_t<!is_integral<I>::value>* = 0) { insert(begin(), f, l); }
    cirque(const cirque& x) {
        reserve(x.size());
        const T *f0, *l0, *f1, *l1;
        tie(f0, l0, f1, l1) = x.data();
        uninitialized_copy(f1, l1, uninitialized_copy(f0, l0, buffer()));
        set_size(x.size());
    }
    cirque(cirque&& x) noexcept = default;
    cirque(initializer_list<T> init) : cirque(init.begin(), init.end()) { }
    ~cirque() { clear(); }
    cirque& operator=(const cirque& x) { *this = cirque(x); return *this; }
    cirque& operator=(cirque&& x) noexcept = default;
    void assign(size_type n, const T& x);
    template<typename I>
    void assign(I f, I l);
    void assign(std::initializer_list<T> init);
    auto data() -> tuple<T*, T*, T*, T*> { return _data(); }
    auto data() const -> tuple<const T*, const T*, const T*, const T*> { return _data(); }
    reference at(size_type p) {
        if (!(p < size())) throw out_of_range("cirque index out of range");
        return *(begin() + p);
    }
    const_reference at(size_type p) const {
        if (!(p < size())) throw out_of_range("cirque index out of range");
        return *(begin() + p);
    }
    reference operator[](size_type p) {
        assert(p < size() && "cirque index out of range");
        return *(begin() + p);
    }
    const_reference operator[](size_type p) const {
        assert(p < size() && "cirque index out of range");
        return *(begin() + p);
    }
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    reference back() { return *(end() - 1); }
    const_reference back() const { return *(end() - 1); }
    size_type capacity() const { return _object ? _object->capacity() : 0; }
    void reserve(size_type n) {
        if (!(capacity() < n)) return;
        unique_ptr<remote> p{static_cast<remote*>(operator new(sizeof(remote) + (n * sizeof(T))))};
        p->_size = size();
        p->_begin = 0;
        p->_allocated = n + 1;
        T *f0, *l0, *f1, *l1;
        tie(f0, l0, f1, l1) = data();
        uninitialized_move(f1, l1, uninitialized_move(f0, l0, p->buffer()));
        _object = move(p);
    }
    iterator begin() { return { _object.get(), begin_index() }; }
    iterator end() { return { _object.get(), add_mod(begin_index(), size(), capacity() + 1) }; }
    const_iterator begin() const { return { _object.get(), begin_index() }; }
    const_iterator end() const
    { return { _object.get(), add_mod(begin_index(), size(), capacity() + 1) }; }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }
    reverse_iterator rbegin() { return end(); }
    reverse_iterator rend() { return begin(); }
    const_reverse_iterator rbegin() const { return end(); }
    const_reverse_iterator rend() const { return begin(); }
    const_reverse_iterator crbegin() const { return end(); }
    const_reverse_iterator crend() const { return begin(); }
    /* Test this with uint8 for size_type and int8 for difference_type */
    size_type max_size() const { return numeric_limits<size_type>::max() / 2 - 1; }
    size_type size() const { return _object ? _object->_size : 0; }
    bool empty() const { return size() == 0; }
    void shrink_to_fit() {
        if (size() == capacity()) return;
        *this = *this;
    }
    void clear() { erase(begin(), end()); }
    iterator erase(const_iterator f, const_iterator l) {
        // minimize number of moves
        iterator f0{_object.get(), f._i};
        iterator l0{_object.get(), l._i};
        if (distance(begin(), f0) < distance(l0, end())) {
            auto p = move_backward(begin(), f0, l0);
            for_each(begin(), p, [](auto& e) { e.~T(); });
            set_size(size() - distance(f0, l0));
            set_begin(p._i);
            return {_object.get(), l._i};
        } else {
            auto p = move(l0, end(), f0);
            for_each(p, end(), [](auto& e) { e.~T(); });
            set_size(size() - distance(f0, l0));
            return {_object.get(), f._i};
        }
    }
    template <typename I>
    iterator insert(const_iterator p, I f, I l) {
        return insert(p, f, l, typename iterator_traits<I>::iterator_category());
    }
    iterator insert(const_iterator p, std::initializer_list<T> init) {
        return insert(p, init.begin(), init.end());
    }
    iterator insert(const_iterator p, const T& value) { return insert(p, &value, &value + 1); }
    iterator insert(const_iterator p, T&& value) {
        return insert(p, make_move_iterator(&value), make_move_iterator(&value + 1));
    }
    iterator insert(const_iterator p, size_type count, const T& value) {
    }
    iterator erase(const_iterator p) {
        return erase(p, p + 1);
    }
    void push_back(const T& x) { insert(end(), x); }
    void push_back(T&& x) { insert(end(), move(x)); }
    void push_front(const T& x) { insert(begin(), x); }
    void push_front(T&& x) { insert(begin(), move(x)); }
    void pop_back() {
        assert(size() && "pop_back on empty cirque");
        erase(end() - 1);
    }
    void pop_front() {
        assert(size() && "pop_front on empty cirque");
        erase(begin());
    }
    void swap(cirque& x) { std::swap(*this, x); }
  private:
    template <typename I>
    iterator insert(const_iterator p, I f, I l, forward_iterator_tag) {
        auto i = distance(cbegin(), p);
        auto n = distance(f, l);
        auto pos = begin() + i;
        if (capacity() < size() + n) {
            cirque tmp;
            tmp.reserve(max(size() + n, capacity() * 2));
            tmp.insert(tmp.end(), make_move_iterator(begin()), make_move_iterator(pos));
            tmp.insert(tmp.end(), f, l);
            tmp.insert(tmp.end(), make_move_iterator(pos), make_move_iterator(end()));
            swap(tmp);
            return begin() + i;
        }
        // FIXME - this is relying on iterators working outside the begin() end() range -
        // This could break if move() was implemented interms of move_n() for example (subtraction
        // of iterators not in the begin/end range is undefined.
        if (i < size() - i) {
            auto un = min(n, i);
            auto p0 = uninitialized_move(begin(), begin() + un, begin() - n);
            auto p1 = move(begin() + un, pos, p0);
            auto p2 = uninitialized_copy(f, f + (n - un), p1);
            copy(f + (n - un), l, p2);
            set_size(size() + n);
            set_begin(sub_mod(begin_index(), n, capacity() + 1));
            return pos - n;
        } else {
            auto un = min(n, distance(begin(), end()) - i);
            auto p0 = uninitialized_move_backward(end() - un, end(), end() + n);
            auto p1 = move_backward(pos, end() - un, p0);
            auto p2 = uninitialized_copy_backward(f, f + (n - un), p1);
            copy_backward(f + (n - un), l, p2);
            set_size(size() + n);
            return pos;
        }
    }
    template <typename I>
    iterator insert(const_iterator p, I f, I l, input_iterator_tag) {
        auto i = distance(cbegin(), p);
        auto s = size();
        if (i < s - i) {
            while (f != l) { push_front(*f); ++f; }
            auto n = size() - s;
            reverse(begin(), begin() + (n + i));
            reverse(begin(), begin() + i);
        } else {
            while (f != l) { push_back(*f); ++f; }
            rotate(begin() + i, begin() + s, end());
        }
        return begin() + i;
    }
};
int main() {
    cirque<int> x{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    x.insert(begin(x), { -3, -2, -1, });
    for (const auto& e : x) cout << e << endl;
}
#endif

#if 0
#include <iostream>
#include <utility>
#include <string>
using namespace std;
class example {
    string _title;
  public:
    const string& title() const& { return _title; }
    string&& title() && { return move(_title); }
};
template <typename... T>
struct comp_ : T... {
    template <typename... Args>
    comp_(Args&&... args) : T(args)... { }
};
template <typename... Args>
auto combine(Args&&... args) { return comp_<Args...>(forward<Args>(args)...); }
int main() {
    auto f = combine([](int x){ cout << "int:" << x << endl; },
                     [](const char* x){ cout << "string:" << x << endl; });
    f(10);
    f("Hello");
}
#endif

#if 0
#include <functional>
#include <iostream>
#include <initializer_list>
using namespace std;
struct annotate {
    annotate() { cout << "annotate ctor" << endl; }
    annotate(const annotate&) { cout << "annotate copy-ctor" << endl; }
    annotate(annotate&&) noexcept { cout << "annotate move-ctor" << endl; }
    annotate& operator=(const annotate&) { cout << "annotate assign" << endl; return *this; }
    annotate& operator=(annotate&&) noexcept { cout << "annotate move-assign" << endl; return *this; }
    ~annotate() { cout << "annotate dtor" << endl; }
    friend inline void swap(annotate&, annotate&) { cout << "annotate swap" << endl; }
    friend inline bool operator==(const annotate&, const annotate&) { return true; }
    friend inline bool operator!=(const annotate&, const annotate&) { return false; }
};
template <typename F, typename... Ts>
auto for_each_argument(F f, Ts&&... a) -> F {
 (void)initializer_list<int>{(f(forward<Ts>(a)), 0)...};
 return f;
}
#if 0
struct print {
    template <typename T>
    void operator()(const T& x) const { cout << x << endl; }
};
#endif
template <typename F>
F&& func(F&& x)
{
    (F&&)x += 1;
    return (F&&)x; }
int main() {
   // for_each_argument(print(), 10, "Hello", 42.5);
   for_each_argument([](const auto& x){ cout << x << endl; }, 10, "Hello", 42.5);
    int x{0};
    func(x);
    cout << x << endl;
}
#endif


#if 0
#include <chrono>
#include <iostream>
#include <thread>
#include <list>
#include <vector>
#include <type_traits>
#include <queue>
#include <random>
#include <deque>
#include <future>
#include <boost/lockfree/queue.hpp>
#include <dispatch/dispatch.h>
using namespace std;
using namespace chrono;
using lock_t = unique_lock<mutex>;
class notification_queue {
    deque<function<void()>> _q;
    bool                    _done{false};
    mutex                   _mutex;
    condition_variable      _ready;
  public:
    bool try_pop(function<void()>& x) {
        lock_t lock{_mutex, try_to_lock};
        if (!lock || _q.empty()) return false;
        x = move(_q.front());
        _q.pop_front();
        return true;
    }
    template<typename F>
    bool try_push(F&& f) {
        {
            lock_t lock{_mutex, try_to_lock};
            if (!lock) return false;
            _q.emplace_back(forward<F>(f));
        }
        _ready.notify_one();
        return true;
    }
    void done() {
        {
            unique_lock<mutex> lock{_mutex};
            _done = true;
        }
        _ready.notify_all();
    }
    bool pop(function<void()>& x) {
        lock_t lock{_mutex};
         while (_q.empty() && !_done) _ready.wait(lock);
         if (_q.empty()) return false;
         x = move(_q.front());
        _q.pop_front();
        return true;
    }
    template<typename F>
    void push(F&& f) {
        {
            lock_t lock{_mutex};
            _q.emplace_back(forward<F>(f));
        }
        _ready.notify_one();
    }
};
class task_system {
    const unsigned              _count{thread::hardware_concurrency()};
    vector<thread>              _threads;
    vector<notification_queue>  _q{_count};
    atomic<unsigned>            _index{0};
 void run(unsigned i) {
        while (true) {
            function<void()> f;
            for (unsigned n = 0; n != _count * 32; ++n) {
                if (_q[(i + n) % _count].try_pop(f)) break;
            }
            if (!f && !_q[i].pop(f)) break;
            f();
        }
    }
  public:
    task_system() {
        for (unsigned n = 0; n != _count; ++n) {
            _threads.emplace_back([&, n]{ run(n); });
        }
    }
    ~task_system() {
        for (auto& e : _q) e.done();
        for (auto& e : _threads) e.join();
    }
    template <typename F>
    void async_(F&& f) {
        auto i = _index++;
        for (unsigned n = 0; n != _count; ++n) {
            if (_q[(i + n) % _count].try_push(forward<F>(f))) return;
        }
        _q[i % _count].push(forward<F>(f));
    }
};
task_system _system;
#if 0
#include <array>
template <typename T, T... Args>
constexpr auto sort() -> array<T, sizeof...(Args)> {
    if
    return { Args... };
}
#endif
#if 1
template <typename F>
void async_(F&& f) {
    _system.async_(forward<F>(f));
}
#else
template <typename F>
void async_(F&& f) {
    dispatch_async_f(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
        new F(move(f)), [](void* p){
            auto f = static_cast<F*>(p);
            (*f)();
            delete(f);
        });
}
#endif
  void time() {
    for (int n = 0; n != 1000000; ++n) {
        async_([]{ });
    }
    mutex block;
    condition_variable ready;
    bool done = false;
    async_([&]{
        {
            unique_lock<mutex> lock{block};
            done = true;
        }
        ready.notify_one();
    });
    unique_lock<mutex> lock{block};
    while (!done) ready.wait(lock);
}
int main() {
#if 0
    future<int> r;
    {
    auto p = make_shared<packaged_task<int()>>([]{ return 42; });
    r = p->get_future();
    async_([_p = move(p)]{ (*_p)(); });
    }
    cout << r.get() << endl;
    //auto f = async(long_operation);
#endif
    auto start = chrono::high_resolution_clock::now();
    time();
    cout << chrono::duration_cast<chrono::milliseconds>
        (chrono::high_resolution_clock::now()-start).count() << endl;
    start = chrono::high_resolution_clock::now();
    time();
    cout << chrono::duration_cast<chrono::milliseconds>
        (chrono::high_resolution_clock::now()-start).count() << endl;
}
#endif




#if 0
#include <utility>
#include <iterator>
#include <iostream>
#include <algorithm>
using namespace std;
template <typename I> // I models ForwardIterator
I rotate_n_plus_one_to_n(I f, I m, I l) {
    auto tmp = move(*f);
    while (m != l) {
        *f = move(*m);
        ++f;
        *m = move(*f);
        ++m;
    }
    *f = move(tmp);
    return f;
}
template <typename I> // I models ForwardIterator
void reverse_(I f, I l) {
    auto n = distance(f, l);
    if (n == 0 || n == 1) return;
    auto m = f;
    advance(m, n - (n / 2));
    reverse_(f, m);
    reverse_(m, l);
    if (n % 2) rotate_n_plus_one_to_n(f, m, l);
    else swap_ranges(f, m, m);
}
int main() {
    int a[] = { 0, 1, 2, 3, 4 };
    // auto f = begin(a), l = end(a);
    // rotate_n_plus_one_to_n(f, f + (l - f) / 2 + 1, l);
    reverse_(begin(a), end(a));
    for (const auto& e : a) cout << e << endl;
}
#if 0
    if (n % 2) rotate_n_plus_one_n(f, m, l);
    else rotate_n_n(f, m, l);
#endif
#endif

#if 0
#include <utility>
#include <iterator>
#include <iostream>
template <typename I, // I models RandomAccessIterator
          typename P> // P models UnaryPredicate
I stable_partition_(I f, I l, P p) {
    auto n = distance(f, l);
    if (n == 0) return f;
    if (n == 1) return f + p(*f);
    auto m = f + n / 2;
    return rotate(stable_partition_(f, m, p),
                  m,
                  stable_partition_(m, l, p));
}
template <typename I> // I models RandomAccessIterator
I rotate_(I f, I m, I l) {
    reverse(f, l);
    m = f + (l - m);
    reverse(f, m);
    reverse(m, l);
    return m;
}
template <typename I> // I models ForwardIterator
void reverse_(I f, I l) {
    auto n = distance(f, l);
    if (n == 0 || n == 1) return;
    auto m = f;
    advance(m, n  - (n / 2));
    reverse_(f, m);
    reverse_(m, l);
    if (n % 2) rotate_n_plus_one_n(f, m, l);
    else rotate_n_n(f, m, l);
}
template <typename I> // I models ForwardIterator
I rotate_n_plus_one_n(I f, I m, I l) {
    auto tmp = move(*f);
    while (m != l) {
        *f = move(*m);
        ++f;
        *m = move(*f);
        ++m;
    }
    *f = move(tmp);
    return f;
}
template <typename I> // I models ForwardIterator
I rotate_n_n(I f, I m, I l) {
    while (m != l) {
        swap(*f, *m);
        ++f, ++m;
    }
    return f;
}
#endif

#if 0
// GOOD for concurrency talk!
#include <chrono>
#include <iostream>
#include <thread>
#include <list>
#include <vector>
#include <type_traits>
#include <queue>
#include <random>
#include <deque>
#include <dispatch/dispatch.h>
using namespace std;
using namespace chrono;
class any {
    struct concept {
        virtual ~concept() = default;
        virtual unique_ptr<concept> copy() const = 0;
    };
    template <typename T>
    struct model : concept {
        explicit model(T x) : _data(x) { }
        unique_ptr<concept> copy() const override { return make_unique<model>(*this); }
        T _data;
    };
    unique_ptr<concept> _object;
  public:
    any() = default;
    template <typename T>
    any(T x) : _object(make_unique<model<T>>(move(x))) { }
    any(const any& x) : _object(x._object ? x._object->copy() : nullptr) { }
    any(any&&) noexcept = default;
    any& operator=(const any& x) { any tmp(x); *this = move(tmp); return *this; }
    any& operator=(any&&) noexcept = default;
    template <typename T, typename... Args>
    void emplace(Args&&... args) { _object = make_unique<model<T>>(forward<Args>(args)...); }
    template <typename T> friend T static_cast_(any& x);
    template <typename T> friend T static_cast_(const any& x);
};
template <typename T>
T static_cast_(any& x) {
    static_assert(is_reference<T>::value, "result type must be a reference");
    return static_cast<any::model<T>*>(x._object.get())->_data;
}
template <typename T>
T static_cast_(const any& x) {
    static_assert(is_reference<T>::value, "result type must be a reference");
    return static_cast<const any::model<T>*>(x._object.get())->_data;
}
/**************************************************************************************************/
template <typename F, typename... Args>
void for_each_argument(F f, Args&&... args) {
    using t = int[sizeof...(Args)];
    (void)t{(f(forward<Args>(args)), 0)...};
}
template <typename F, typename T, size_t... I>
void apply_(F f, T&& t, integer_sequence<size_t, I...>) {
    f(get<I>(t)...);
}
template <typename F, typename T>
void apply(F f, T&& t) {
    apply_(f, t, make_integer_sequence<size_t, tuple_size<T>::value>());
}
/**************************************************************************************************/
using lock_t = unique_lock<mutex>;
template <typename T>
struct task_ {
    any                         _f;
    vector<T>                   _r; // optional
    mutex _mutex;
    vector<function<void()>> _then;
    bool _resolved{false};
    // virtual ~task_() = default;
    template <typename F, typename... Args>
    void resolve(F& f, Args&&... args) {
        _r.emplace_back(f(forward<Args>(args)...));
        vector<function<void()>> tmp;
        {
        lock_t lock{_mutex};
        swap(tmp, _then);
        _resolved = true;
        }
        for(const auto& e : tmp) e();
    }
    template <typename F>
    void then(F f) {
        bool resolved{false};
        {
        lock_t lock{_mutex};
        if (_resolved) resolved = true;
        else _then.emplace_back(move(f));
        }
        if (resolved) f();
    }
    const T& get() const {
        return _r.back();
    }
};
template <typename T>
class future;
template <typename T, typename F>
struct task_f {
    F _f;
    weak_ptr<task_<T>> _r;
    template <typename... Args>
    explicit task_f(weak_ptr<task_<T>> r, Args&&... args) : _r(move(r)), _f(forward<Args>(args)...) { }
    template <typename... Args>
    void operator()(Args&&... args) {
        auto r = _r.lock();
        if (r) r->resolve(_f, forward<Args>(args)...);
    }
};
template <typename F, typename... Args>
struct task_group {
    F _f;
    atomic<size_t>  _n{sizeof...(Args)};
    explicit task_group(F f) : _f(move(f)) { }
    void operator()() {
        if (--_n == 0) _f();
    };
};
template <typename T>
class packaged;
template <typename T, typename F>
using packaged_task = packaged<task_f<T, F>>;
template <typename F, typename... Args>
auto when_all(F f, future<Args>... args) -> future<result_of_t<F(Args...)>>;
template <typename T, typename F>
auto package(F f) -> pair<packaged_task<T, F>, future<T>>;
template <typename T>
class future {
    shared_ptr<task_<T>> _p;
    template <typename T1, typename F1>
    friend auto package(F1 f) -> pair<packaged_task<T1, F1>, future<T1>>;
    template <typename F, typename... Args>
    friend auto when_all(F f, future<Args>... args) -> future<result_of_t<F(Args...)>>;
    explicit future(shared_ptr<task_<T>> p) : _p(move(p)) { }
    template <typename F>
    void then(F&& f) { _p->then(forward<F>(f)); }
  public:
    future() = default;
    const T& get() const { return _p->get(); }
};
template <typename T>
class packaged {
    weak_ptr<T> _p;
    template <typename T1, typename F1>
    friend auto package(F1 f) -> pair<packaged_task<T1, F1>, future<T1>>;
    template <typename F1, typename... Args>
    friend auto when_all(F1 f, future<Args>... args) -> future<result_of_t<F1(Args...)>>;
    explicit packaged(weak_ptr<T> p) : _p(move(p)) { }
  public:
    template <typename... Args>
    void operator()(Args&&... args) const {
        auto p = _p.lock();
        if (p) (*p)(forward<Args>(args)...);
    }
};
template <typename T, typename F>
auto package(F f) -> pair<packaged_task<T, F>, future<T>>
{
    auto p = make_shared<task_f<T, F>>(move(f));
    return make_pair(packaged_task<T, F>(p), future<T>(p));
}
template <typename F, typename... Args>
auto when_all(F f, future<Args>... args) -> future<result_of_t<F(Args...)>>
{
    using result_type = result_of_t<F(Args...)>;
    auto p = make_shared<task_group_<result_type>>(move(f), args...);
    for_each_argument([&](auto x){
        x.then([_f = packaged<task_group_<result_type>>(p)]{ _f(); });
    }, args...);
    return future<result_type>(p);
}
#if 0
inline constexpr size_t cstrlen(const char* p) {
    size_t n = 0;
    while (p[n] != '\0') ++n;
    return n;
}
#endif
template <size_t N>
size_t cstrlen(const char(&)[N]) { cout << "O(1)" << endl; return N - 1; }
inline size_t cstrlen(const char* p) {
    size_t n = 0;
    while (p[n] != '\0') ++n;
    return n;
}
int main() {
    for_each_argument([](const auto& x){ cout << x << endl; }, 10, "hello", 42.5);
    auto n = cstrlen("Hello World");
#if 0
    constexpr int a[cstrlen("Hello World")] = { 0 };
#endif
    function<void()> f;
    function<void()> g;
    future<int> g_f;
    {
    future<int> later;
    auto x = package<int>([]{ cout << "x" << endl; return 10; });
    auto y = package<int>([]{ cout << "y" << endl; return 20; });
    later = when_all([](int x, int y){ cout << "when_all: " << x << ", " << y << endl; return x + y; }, x.second, y.second);
    f = x.first; g = y.first;
    g_f = y.second;
    }
    f();
    g();
    when_all([](int x){ cout << "g: " << x << endl; return 1; }, g_f);
    [](auto... args){ int a[]{args...}; for (const auto& e : a) cout << e << endl; }(10, 20, 30);
}
#endif


#if 0
#include <algorithm>
#include <numeric>
#include <vector>
#include <iostream>
using namespace std;
template <typename I>
void reverse_(I f, I l) {
    auto n = distance(f, l);
    if (n == 0 || n == 1) return;
    auto m = f + (n / 2);
    reverse_(f, m);
    reverse_(m, l);
    rotate(f, m, l);
}
int main() {
    vector<int> a(1024 * 1024);
    iota(begin(a), end(a), 0);
    reverse_(begin(a), end(a));
    for (const auto& e : a) cout << e << endl;
}
#endif


#if 0
#include <chrono>
#include <iostream>
#include <thread>
#include <list>
#include <vector>
#include <type_traits>
#include <queue>
#include <random>
#include <deque>
#include <boost/lockfree/queue.hpp>
//#include <boost/thread/future.hpp>
#include <dispatch/dispatch.h>
using namespace std;
using namespace chrono;
class any {
    struct concept {
        virtual ~concept() = default;
        virtual unique_ptr<concept> copy() const = 0;
    };
    template <typename T>
    struct model : concept {
        explicit model(T x) : _data(x) { }
        unique_ptr<concept> copy() const override { return make_unique<model>(*this); }
        T _data;
    };
    unique_ptr<concept> _object;
  public:
    any() = default;
    template <typename T>
    any(T x) : _object(make_unique<model<T>>(move(x))) { }
    any(const any& x) : _object(x._object ? x._object->copy() : nullptr) { }
    any(any&&) noexcept = default;
    any& operator=(const any& x) { any tmp(x); *this = move(tmp); return *this; }
    any& operator=(any&&) noexcept = default;
    template <typename T> friend T static_cast_(any& x);
    template <typename T> friend T static_cast_(const any& x);
};
template <typename T>
T static_cast_(any& x) {
    static_assert(is_reference<T>::value, "result type must be a reference");
    return static_cast<any::model<T>*>(x._object.get())->_data;
}
template <typename T>
T static_cast_(const any& x) {
    static_assert(is_reference<T>::value, "result type must be a reference");
    return static_cast<const any::model<T>*>(x._object.get())->_data;
}
/**************************************************************************************************/
template <typename R>
struct shared_result;
template <typename R, typename F>
struct shared_task {
    F _f;
    weak_ptr<shared_result<R>> _r;
    explicit shared_task(F f) : _f(move(f)) { }
    template <typename... Args>
    void operator()(Args... args) const {
        auto r = _r.lock();
        if (r) (*r)(_f, forward<Args>(args)...);
    }
};
using lock_t = unique_lock<mutex>;
template <typename R>
struct shared_result {
    vector<R> _r; // optional
    any _f;
    mutex _mutex;
    vector<function<void()>> _then;
    bool _resolved{false};
    template <typename F, typename... Args>
    void operator()(F f, Args... args) {
        _r.emplace_back(f(forward<Args>(args)...));
        vector<function<void()>> tmp;
        {
        lock_t lock{_mutex};
        swap(tmp, _then);
        _resolved = true;
        }
        for(const auto& e : tmp) e();
    }
    template <typename F>
    void then(F f) {
        bool resolved{false};
        {
        lock_t lock{_mutex};
        if (_resolved) resolved = true;
        else _then.emplace_back(move(f));
        }
        if (resolved) f();
    }
};
template <typename R, typename F>
struct packaged_task {
    shared_ptr<shared_task<R, F>> _f;
    explicit packaged_task(shared_ptr<shared_task<R, F>>& f) : _f(move(f)) { }
    template <typename... Args>
    void operator()(Args... args) const {
        (*_f)(forward<Args>(args)...);
    }
};
template <typename R, typename F>
auto package(F f);
template <typename T>
class future {
    shared_ptr<shared_result<T>> _r;
    template <typename R, typename F>
    friend auto package(F f);
  public:
    // Need a friend template function?
    explicit future(shared_ptr<shared_result<T>>& r) : _r(move(r)) { }
    future() = default;
    const T& get() const { return _r->_r.back(); }
    template <typename F>
    auto then(F&& f) {
        auto p = package<T>(forward<F>(f));
        _r.then([_r = _r, _f = move(p.first)]{ _f(_r->_r.back()); });
        return move(p.second);
    }
    template <typename F>
    void then_(F&& f) { _r->then(forward<F>(f)); }
};
template <typename R, typename F>
auto package(F f) {
    auto f_ = make_shared<shared_task<R, F>>(move(f));
    auto r_ = make_shared<shared_result<R>>();
    r_->_f = f_;
    f_->_r = r_;
    return make_pair(packaged_task<R, F>(f_), future<R>(r_));
}
template <size_t N, typename F>
struct group {
    F                           _f;
    mutable atomic<size_t>      _n{N};
    explicit group(F f) : _f(move(f)) { }
    void operator()() const {
        if (--_n == 0) _f();
    };
};
template <size_t N, typename F>
auto make_group(F f) { return make_shared<group<N, F>>(move(f)); }
template <typename F, typename T, typename... Args>
auto when_all(F f, future<T> arg0, future<Args>... args) {
    auto p = package<result_of_t<F(T, Args...)>>(f);
    auto s = make_group<1 + sizeof...(Args)>([_f = move(p.first), arg0, args...]{ _f(arg0.get(), args.get()...); });
    arg0.then_([_s = s]{ (*_s)(); });
    [&](auto x){ x.then_([_s = s]{ (*_s)(); }); }(args...);
    return p.second;
}
int main() {
    auto p1 = package<string>([]{ return "Hello"; });
    auto p2 = package<string>([]{ return "World"; });
    auto f = when_all([](string x, string y){ cout << x << y << endl; return 0; }, p1.second, p2.second);
    p1.first();
    p2.first();
    cout << "end" << endl;
}
#endif



#if 0
#include <chrono>
#include <iostream>
#include <thread>
#include <list>
#include <vector>
#include <type_traits>
#include <queue>
#include <random>
#include <deque>
#include <boost/lockfree/queue.hpp>
#include <dispatch/dispatch.h>
using namespace std;
using namespace chrono;
class notification_queue {
    deque<function<void()>> _q;
    bool                    _done{false};
    mutex                   _mutex;
    condition_variable      _ready;
    using lock_t = unique_lock<mutex>;
  public:
    void done() {
        {
            unique_lock<mutex> lock{_mutex};
            _done = true;
        }
        _ready.notify_one();
    }
    bool pop(function<void()>& x) {
        lock_t lock{_mutex};
         while (_q.empty() && !_done) _ready.wait(lock);
         if (_q.empty()) return false;
         x = move(_q.front());
        _q.pop_front();
        return true;
    }
    bool pop_try(function<void()>& x) {
        lock_t lock{_mutex, try_to_lock};
        if (!lock || _q.empty()) return false;
        x = move(_q.front());
        _q.pop_front();
        return true;
    }
    template<typename F>
    void push(F&& f) {
        {
            lock_t lock{_mutex};
            _q.emplace_back(forward<F>(f));
        }
        _ready.notify_one();
    }
    template<typename F>
    bool push_try(F&& f) {
        {
            lock_t lock{_mutex, try_to_lock};
            if (!lock) return false;
            _q.emplace_back(forward<F>(f));
        }
        _ready.notify_one();
        return true;
    }
};
class task_system {
    const unsigned              _count{thread::hardware_concurrency()};
    vector<thread>              _threads;
    vector<notification_queue>  _q{_count};
    atomic<unsigned>            _index{0};
  public:
    task_system() {
        for (unsigned n = 0; n != _count; ++n) {
            _threads.emplace_back([&, n]{ run(n); });
        }
    }
    ~task_system() {
        for (auto& e : _q) e.done();
        for (auto& e : _threads) e.join();
    }
    void run(unsigned i) {
        while (true) {
            function<void()> f;
            for (unsigned n = 0; n != _count; ++n) {
                if (_q[(i + n) % _count].pop_try(f)) break;
            }
            if (!f && !_q[i].pop(f)) break;
            f();
        }
    }
    template <typename F>
    void async_(F&& f) {
        unsigned i = _index++;
        for (unsigned n = 0; n != _count; ++n) {
            if (_q[(i + n) % _count].push_try(forward<F>(f))) return;
        }
        _q[i % _count].push(forward<F>(f));
    }
};
class any {
    struct concept {
        virtual ~concept() = default;
        virtual unique_ptr<concept> copy() const = 0;
    };
    template <typename T>
    struct model : concept {
        explicit model(T x) : _data(x) { }
        unique_ptr<concept> copy() const override { return make_unique<model>(*this); }
        T _data;
    };
    unique_ptr<concept> _object;
  public:
    any() = default;
    template <typename T>
    any(T x) : _object(make_unique<model<T>>(move(x))) { }
    any(const any& x) : _object(x._object ? x._object->copy() : nullptr) { }
    any(any&&) noexcept = default;
    any& operator=(const any& x) { any tmp(x); *this = move(tmp); return *this; }
    any& operator=(any&&) noexcept = default;
    template <typename T> friend T static_cast_(any& x);
    template <typename T> friend T static_cast_(const any& x);
};
template <typename T>
T static_cast_(any& x) {
    static_assert(is_reference<T>::value, "result type must be a reference");
    return static_cast<any::model<T>*>(x._object.get())->_data;
}
template <typename T>
T static_cast_(const any& x) {
    static_assert(is_reference<T>::value, "result type must be a reference");
    return static_cast<const any::model<T>*>(x._object.get())->_data;
}
// ...
// namespace detail {
template <typename T>
struct shared {
    using cont_t = vector<function<void(T)>>;
    any         _f;
    vector<T>   _result;
    mutex       _mutex;
    bool        _resolved{false};
    cont_t      _cont;
    template <typename F>
    explicit shared(F f) : _f(move(f)) { }
    template <typename F, typename... Args>
    void resolve(F f, Args&&... args) {
        _result.emplace_back(f(forward<Args>(args)...));
        cont_t tmp;
        {
            unique_lock<mutex> lock(_mutex);
            swap(_cont, tmp);
            _resolved = true;
        }
        for (const auto& e : tmp) e(_result.back());
    }
    template <typename F>
    void then(F f) {
        bool resolved;
        {
            unique_lock<mutex> lock(_mutex);
            resolved = _resolved;
            if (!resolved) _cont.emplace_back(move(f));
        }
        if (resolved) f(_result.back());
    }
};
template <typename, typename> class group;
template <typename F, typename R, typename... Args>
struct group<F, R(Args...)> {
    atomic<size_t>  _count{sizeof...(Args)};
    F    _f;
    void available() {
        if (--_count == 0) _f();
    }
};
// Can I avoid void case in example code?
template <>
struct shared<void> {
    using cont_t = vector<function<void()>>;
    any     _f;
    mutex   _mutex;
    bool    _resolved{false};
    cont_t  _cont;
    template <typename F>
    explicit shared(F f) : _f(move(f)) { }
    // virtual ~shared() = default;
    template <typename F, typename... Args>
    void resolve(F f, Args&&... args) {
        f(forward<Args>(args)...);
        cont_t tmp;
        {
            unique_lock<mutex> lock(_mutex);
            swap(_cont, tmp);
            _resolved = true;
        }
        for (const auto& e : tmp) e();
    }
    template <typename F>
    void then(F f) {
        bool resolved;
        {
            unique_lock<mutex> lock(_mutex);
            resolved = _resolved;
            if (!resolved) _cont.emplace_back(move(f));
        }
        if (resolved) f();
    }
};
#if 0
// Better to write a quick any?
template <typename F, typename T>
struct shared_function : shared<T> {
    F _f;
    explicit shared_function(F f) : _f(move(f)) { }
};
#endif
template <typename F, typename T>
struct packaged {
    weak_ptr<shared<T>> _p;
    template <typename... Args>
    void operator()(Args&&... args) const {
        auto p = _p.lock();
        if (p) {
        p->resolve(static_cast_<const F&>(p->_f), forward<Args>(args)...);
        }
    }
};
// } // detail
#if 0
template <typename R, typename F, typename... Args>
auto when_all(F f, future<Args>... args) {
    package_task<R>([args...]() {
    });
}
#endif
// split a function into a function returning void and a future
template <typename T>
class future;
template <typename R, typename F>
auto package_task(F f) {
    auto p = make_shared<shared<R>>(move(f)); // one use of p could be a move
    return make_pair(packaged<F, R>{p}, future<R>{p});
}
template <typename T>
class future {
    shared_ptr<shared<T>> _shared;
  public:
    explicit future(shared_ptr<shared<T>> x) : _shared(move(x)) { }
    future() = default;
    template <typename F>
    auto then(F f) {
        auto p = package_task<result_of_t<F(T)>>([_hold = _shared, _f = move(f)](const T& x){ _f(x); });
        _shared->then(move(p.first));
        return move(p.second);
    }
};
task_system _system;
#if 0
#include <array>
template <typename T, T... Args>
constexpr auto sort() -> array<T, sizeof...(Args)> {
    if
    return { Args... };
}
#endif
#if 0
template <typename F>
void async_(F&& f) {
    _system.async_(forward<F>(f));
}
#else
template <typename F>
void async_(F&& f) {
    dispatch_async_f(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
        new F(move(f)), [](void* p){
            auto f = static_cast<F*>(p);
            (*f)();
            delete(f);
        });
}
#endif
  void time() {
    for (int n = 0; n != 1000000; ++n) {
        async_([]{
        #if 0
            int a[1000];
            iota(begin(a), end(a), 0);
            shuffle(begin(a), end(a), default_random_engine{});
            sort(begin(a), end(a));
        #endif
        });
    }
    mutex block;
    condition_variable ready;
    bool done = false;
    async_([&]{
        {
            unique_lock<mutex> lock{block};
            done = true;
        }
        ready.notify_one();
    });
    unique_lock<mutex> lock{block};
    while (!done) ready.wait(lock);
}
int main() {
    auto start = chrono::high_resolution_clock::now();
    time();
    cout << chrono::duration_cast<chrono::milliseconds>
        (chrono::high_resolution_clock::now()-start).count() << endl;
    start = chrono::high_resolution_clock::now();
    time();
    cout << chrono::duration_cast<chrono::milliseconds>
        (chrono::high_resolution_clock::now()-start).count() << endl;
    function<void(int)> package;
    {
    future<void> y;
    auto x = package_task<int>([](int x) { cout << "call 1" << endl; return x; });
    package = move(x.first);
    y = x.second.then([](int x){ cout << "result:" << x << endl; });
    }
    package(42);
}
#endif


#if 0
#include <chrono>
#include <iostream>
#include <thread>
#include <list>
#include <vector>
#include <type_traits>
#include <queue>
#include <random>
#include <deque>
#include <boost/lockfree/queue.hpp>
#include <dispatch/dispatch.h>
using namespace std;
using namespace chrono;
#define VARIENT 99
#if VARIENT == 0
template <typename T>
class notification_queue {
    list<T> _q;
    bool    _done{false};
    mutex   _mutex;
    condition_variable _ready;
  public:
    void push_notify(T x) {
        list<T> tmp{move(x)};
        {
            unique_lock<mutex> lock{_mutex};
            _q.splice(_q.end(), tmp);
        }
        _ready.notify_one();
    }
    void done_notify() {
        {
            unique_lock<mutex> lock{_mutex};
            _done = true;
        }
        _ready.notify_all();
    }
    bool wait_pop(T& x) {
        list<T> tmp;
        {
            unique_lock<mutex> lock{_mutex};
            while (_q.empty() && !_done) _ready.wait(lock);
            if (_q.empty()) return false;
            tmp.splice(tmp.end(), _q, _q.begin());
        }
        x = move(tmp.front());
        return true;
    }
};
#elif VARIENT == 1
template <typename T>
class notification_queue {
    struct cmp {
        template <typename U>
        bool operator()(const U& x, const U& y) const { return x.first > y.first; }
    };
    priority_queue<pair<uint64_t, T>, vector<pair<uint64_t, T>>, cmp> _q;
    uint64_t _count{0};
    bool    _done{false};
    mutex   _mutex;
    condition_variable _ready;
  public:
    void push_notify(T x) {
        {
            unique_lock<mutex> lock{_mutex};
            _q.emplace(_count, move(x));
            ++_count;
        }
        _ready.notify_one();
    }
    void done_notify() {
        {
            unique_lock<mutex> lock{_mutex};
            _done = true;
        }
        _ready.notify_all();
    }
    bool wait_pop(T& x) {
        unique_lock<mutex> lock{_mutex};
        while (_q.empty() && !_done) {  _ready.wait(lock); }
        if (_q.empty()) return false;
        x = move(_q.top().second);
        _q.pop();
        return true;
    }
};
#elif VARIENT == 2
template <typename T>
class notification_queue {
    list<T> _q;
    mutex   _q_mutex;
    bool    _done{false};
    mutex   _mutex;
    atomic<int> _idle{0};
    condition_variable _ready;
  public:
    bool pop(T& x) {
        list<T> tmp;
        {
            unique_lock<mutex> lock{_q_mutex};
            if (_q.empty()) return false;
            tmp.splice(tmp.end(), _q, _q.begin());
        }
        x = move(tmp.front());
        return true;
    }
    bool pop_try(T& x) {
        list<T> tmp;
        {
            unique_lock<mutex> lock{_q_mutex, try_to_lock};
            if (!lock || _q.empty()) return false;
            tmp.splice(tmp.end(), _q, _q.begin());
        }
        x = move(tmp.front());
        return true;
    }
    void push_notify(T x) {
        list<T> tmp{move(x)};
        {
            unique_lock<mutex> lock{_q_mutex};
            _q.splice(_q.end(), tmp);
        }
        if (_idle) _ready.notify_one();
    }
    void done_notify() {
        {
            unique_lock<mutex> lock{_mutex};
            _done = true;
        }
        _ready.notify_all();
    }
    bool wait_pop(T& x) {
        if (pop(x)) return true;
        ++_idle;
        bool result = pop(x);
        if (!result) {
            unique_lock<mutex> lock{_mutex};
            while (!result && !_done) {
                _ready.wait(lock);
                result = pop(x);
            }
        }
        --_idle;
        return result;
    }
};
#endif
class notification_queue {
    deque<function<void()>> _q;
    bool                    _done{false};
    mutex                   _mutex;
    condition_variable      _ready;
    using lock_t = unique_lock<mutex>;
  public:
    void done_notify() {
        {
            unique_lock<mutex> lock{_mutex};
            _done = true;
        }
        _ready.notify_all();
    }
    bool pop(function<void()>& x) {
        lock_t lock{_mutex};
         while (_q.empty() && !_done) _ready.wait(lock);
         if (_q.empty()) return false;
         x = move(_q.front());
        _q.pop_front();
        return true;
    }
    bool pop_try(function<void()>& x) {
        lock_t lock{_mutex, try_to_lock};
        if (!lock || _q.empty()) return false;
        x = move(_q.front());
        _q.pop_front();
        return true;
    }
    template<typename F>
    void push(F&& f) {
        {
            lock_t lock{_mutex};
            _q.emplace_back(forward<F>(f));
        }
        _ready.notify_one();
    }
    template<typename F>
    bool push_try(F&& f) {
        {
            lock_t lock{_mutex, try_to_lock};
            if (!lock) return false;
            _q.emplace_back(forward<F>(f));
        }
        _ready.notify_one();
        return true;
    }
};
class task_system {
    const unsigned              _count{thread::hardware_concurrency()};
    vector<thread>              _threads;
    vector<notification_queue>  _q{_count};
    atomic<unsigned>            _index{0};
  public:
    task_system() {
        for (unsigned n = 0; n != _count; ++n) {
            _threads.emplace_back([&, n]{ run(n); });
        }
    }
    ~task_system() {
        for (auto& e : _q) e.done_notify();
        for (auto& e : _threads) e.join();
    }
    void run(unsigned i) {
        while (true) {
            function<void()> f;
            for (unsigned n = 0; n != _count; ++n) {
                if (_q[(i + n) % _count].pop_try(f)) break;
            }
            if (!f && !_q[i % _count].pop(f)) break;
            f();
        }
    }
    template <typename F>
    void async_(F&& f) {
        unsigned i = _index++;
        for (unsigned n = 0; n != _count; ++n) {
            if (_q[(i + n) % _count].push_try(forward<F>(f))) return;
        }
        _q[i % _count].push(forward<F>(f));
    }
};
#if 0
class thread_pool {
    vector<thread> _pool;
  public:
    template <typename F>
    explicit thread_pool(F f) {
        for (auto n = thread::hardware_concurrency(); n != 0; --n) {
            _pool.emplace_back(move(f));
        }
    }
    ~thread_pool() {
        for (auto& e : _pool) e.join();
    }
};
class task_system {
    struct element_type {
        void (*_f)(void*);
        void* _x;
    };
    notification_queue<function<void()>> _q;
    thread_pool _pool;
  public:
    task_system() : _pool{[&]{
        function<void()> f;
        while (_q.wait_pop(f)) f();
    }} { }
    ~task_system() { _q.done_notify(); }
    template <typename F>
    void async_(F&& f) {
        _q.push_notify(forward<F>(f));
    }
};
#endif
task_system _system;
#if 0
template <typename F>
void async_(F&& f) {
    _system.async_(forward<F>(f));
}
#else
template <typename F>
void async_(F&& f) {
    dispatch_async_f(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
        new F(move(f)), [](void* p){
            auto f = static_cast<F*>(p);
            (*f)();
            delete(f);
        });
}
#endif
__attribute__ ((noinline)) void time() {
    condition_variable ready;
    bool done = false;
    for (int n = 0; n != 1000000; ++n) {
        async_([]{
        #if 0
            int a[1000];
            iota(begin(a), end(a), 0);
            shuffle(begin(a), end(a), default_random_engine{});
            sort(begin(a), end(a));
        #endif
        });
    }
    async_([&]{ done = true; ready.notify_one(); });
    mutex block;
    unique_lock<mutex> lock{block};
    while (!done) ready.wait(lock);
}
int main() {
    auto start = chrono::high_resolution_clock::now();
    time();
    cout << chrono::duration_cast<chrono::milliseconds>
        (chrono::high_resolution_clock::now()-start).count() << endl;
    start = chrono::high_resolution_clock::now();
    time();
    cout << chrono::duration_cast<chrono::milliseconds>
        (chrono::high_resolution_clock::now()-start).count() << endl;
}
#endif


#if 0
#include <chrono>
#include <iostream>
#include <thread>
#include <list>
#include <vector>
#include <type_traits>
#include <queue>
#include <random>
#include <boost/lockfree/queue.hpp>
using namespace std;
using namespace chrono;
// namespace detail {
template <typename, typename> class group;
template <typename F, typename R, typename... Args>
struct group<F, R(Args...)> {
    atomic<size_t>  _count{sizeof...(Args)};
    F    _f;
    void available() {
        if (--_count == 0) _f();
    }
};
template <typename T>
struct shared {
    using queue_type = list<function<void()>>;
    aligned_storage_t<sizeof(T)> _result;
    mutex   _mutex;
    bool _resolved{false};
    queue_type _q;
    template <typename F, typename... Args>
    void resolve(F f, Args&&... args) {
        new (&_result)T(f(forward<Args>(args)...));
        queue_type tmp;
        {
            unique_lock<mutex> lock(_mutex);
            swap(_q, tmp);
            _resolved = true;
        }
        for (const auto& e : tmp) e();
    }
};
template <typename F, typename T>
struct packaged {
    F _f;
    weak_ptr<T> _p;
    template <typename... Args>
    void operator()(Args&&... args) const {
        auto p = _p.lock();
        if (p) p->resolve(_f, forward<Args>(args)...);
    }
};
// } // detail
template <typename T>
class future {
    shared_ptr<shared<T>> _shared;
  public:
    template <typename F>
    auto then(F f) { return when_all(f, *this); }
};
// split a function into a function returning void and a future
template <typename F, typename S>
auto package_task(F f) {
    using result_type = result_of_t<S>;
    auto p = make_shared<shared<result_type>>();
    return make_pair(packaged<F, result_type>{f, p}, future<result_type>{p});
}
// given a function return a function bound to the futures and a
// a future for  the result
/*
    begin
    end
*/
template <typename T>
class concurrent_queue {
    std::list<T>      _q;
    mutex             _mutex;
  public:
    bool pop(T& x) {
        list<T> tmp;
        {
            unique_lock<mutex> lock{_mutex};
            if (_q.empty()) return false;
            tmp.splice(tmp.end(), _q, _q.begin());
        }
        x = move(tmp.front());
        return true;
    }
    bool try_to_pop(T& x) {
        list<T> tmp;
        {
            unique_lock<mutex> lock{_mutex, try_to_lock};
            if (!lock || _q.empty()) return false;
            tmp.splice(tmp.end(), _q, _q.begin());
        }
        x = move(tmp.front());
        return true;
    }
    void push(T x) {
        list<T> tmp{move(x)};
        {
            unique_lock<mutex> lock{_mutex};
            _q.splice(_q.end(), tmp);
        }
    }
};
#define VARIANT 3
#if VARIANT == 0
template <typename T>
class notification_queue {
    list<T> _q;
    mutex   _q_mutex;
    bool    _done = false;
    mutex   _mutex;
    condition_variable _ready;
  public:
    bool pop(T& x) {
        list<T> tmp;
        {
            unique_lock<mutex> lock{_q_mutex};
            if (_q.empty()) return false;
            tmp.splice(tmp.end(), _q, _q.begin());
        }
        x = move(tmp.front());
        return true;
    }
    bool try_to_pop(T& x) {
        list<T> tmp;
        {
            unique_lock<mutex> lock{_q_mutex, try_to_lock};
            if (!lock || _q.empty()) return false;
            tmp.splice(tmp.end(), _q, _q.begin());
        }
        x = move(tmp.front());
        return true;
    }
    void push_notify(T x) {
        list<T> tmp{move(x)};
        {
            unique_lock<mutex> lock{_q_mutex};
            _q.splice(_q.end(), tmp);
        }
        _ready.notify_one();
    }
    void done_notify() {
        {
            unique_lock<mutex> lock{_mutex};
            _done = true;
        }
        _ready.notify_all();
    }
    bool wait_pop(T& x) {
        if (try_to_pop(x)) return true;
        unique_lock<mutex> lock{_mutex};
        bool result = try_to_pop(x);
        while (!result && !_done) {
            _ready.wait(lock);
            result = pop(x);
        }
        return result;
    }
};
#elif VARIANT == 1
template <typename T>
class notification_queue {
    vector<pair<uint64_t, T>> _q;
    mutex   _q_mutex;
    uint64_t _count;
    bool    _done = false;
    mutex   _mutex;
    condition_variable _ready;
  public:
    bool pop(T& x) {
        unique_lock<mutex> lock{_q_mutex};
        if (_q.empty()) return false;
        pop_heap(begin(_q), end(_q), [](const auto& x, const auto& y) { return x.first > y.first; });
        x = move(_q.back().second);
        _q.pop_back();
        return true;
    }
    void push_notify(T x) {
        {
            unique_lock<mutex> lock{_q_mutex};
            _q.emplace_back(_count, move(x)); ++_count;
            push_heap(begin(_q), end(_q), [](const auto& x, const auto& y) { return x.first > y.first; });
        }
        _ready.notify_one();
    }
    void done_notify() {
        {
            unique_lock<mutex> lock{_mutex};
            _done = true;
        }
        _ready.notify_all();
    }
    bool wait_pop(T& x) {
        // if (pop(x)) return true;
        unique_lock<mutex> lock{_mutex};
        bool result = pop(x);
        while (!result && !_done) {
            _ready.wait(lock);
            result = pop(x);
        }
        return result;
    }
};
#elif VARIANT == 2
template <typename T>
class notification_queue {
    boost::lockfree::queue<T> _q{1000000 * 2};
    boost::lockfree::queue<T> _q_hi{1000000 * 2};
    bool    _done = false;
    mutex   _mutex;
    condition_variable _ready;
  public:
    void push_notify(T x) {
        _q.push(move(x));
        _ready.notify_one();
    }
    void done_notify() {
        {
            unique_lock<mutex> lock{_mutex};
            _done = true;
        }
        _ready.notify_all();
    }
    bool wait_pop(T& x) {
        if (_q_hi.pop(x) || _q.pop(x)) return true;
        unique_lock<mutex> lock{_mutex};
        bool result = _q_hi.pop(x) || _q.pop(x);
        while (!result && !_done) {
            _ready.wait(lock);
            result = _q_hi.pop(x) || _q.pop(x);
        }
        return result;
    }
};
#elif VARIANT == 3
template <typename T>
class notification_queue {
#if 1
    concurrent_queue<T> _q;
#else
    boost::lockfree::queue<T> _q{1000000 * 2};
#endif
    bool    _done = false;
    mutex   _mutex;
    condition_variable _ready;
  public:
    void push_notify(T x) {
        _q.push(move(x));
        
       { unique_lock<mutex> lock{_mutex}; }
        _ready.notify_one();
    }
    void done_notify() {
        {
            unique_lock<mutex> lock{_mutex};
            _done = true;
        }
        _ready.notify_all();
    }
    bool wait_pop(T& x) {
        // if (_q.try_to_pop(x)) return true;
        unique_lock<mutex> lock{_mutex};
        bool result = _q.pop(x);
        while (!result && !_done) {
            _ready.wait(lock);
            result = _q.pop(x);
        }
        return result;
    }
};
#endif
#define SIMPLE 1
class thread_pool {
    vector<thread> _pool;
  public:
    template <typename F>
    explicit thread_pool(F f) {
        for (auto n = thread::hardware_concurrency(); n != 0; --n) {
            _pool.emplace_back(f);
        }
    }
    ~thread_pool() {
        for (auto& e : _pool) e.join();
    }
};
class task_system {
    struct element_type {
        void (*_f)(void*);
        void* _x;
    };
#if SIMPLE
    notification_queue<element_type> _q;
#else
    notification_queue<function<void()>> _q;
#endif
    thread_pool _pool;
  public:
#if SIMPLE
    task_system() : _pool{[&]{
        element_type e;
        while (_q.wait_pop(e)) e._f(e._x);
    }} { }
#else
    task_system() : _pool{[&]{
        function<void()> f;
        while (_q.wait_pop(f)) f();
    }} { }
#endif
    ~task_system() { _q.done_notify(); }
#if SIMPLE
    template <typename F>
    void async_(F&& f) {
        _q.push_notify({[](void* x) {
            function<void()>* p = static_cast<function<void()>*>(x);
            (*p)();
            delete p;
        }, new function<void()>(forward<F>(f))});
    }
#else
    template <typename F>
    void async_(F&& f) {
        _q.push_notify(forward<F>(f));
    }
    template <typename F>
    auto async(F f) -> future<result_of_t<F()>> {
        auto p = package_task<result_of_t<F()>()>(move(f));
        _q.push_notify(move(p.first));
        return move(p.second);
    }
#endif
};
template <typename F, typename... Args>
auto when_all(F f, future<Args>&&...) {
    return package_task<result_of_t<F(Args...)>>(f);
}
task_system _system;
template <typename F>
void async_(F&& f) {
    _system.async_(forward<F>(f));
}
__attribute__ ((noinline)) void time() {
    condition_variable ready;
    bool done = false;
    for (int n = 0; n != 1000000; ++n) {
        async_([]{
        #if 0
            int a[1000];
            iota(begin(a), end(a), 0);
            shuffle(begin(a), end(a), default_random_engine{});
            sort(begin(a), end(a));
        #endif
        });
    }
    async_([&]{ done = true; ready.notify_one(); });
    mutex block;
    unique_lock<mutex> lock{block};
    while (!done) ready.wait(lock);
}
#include <future>
int main() {
#if 0
    condition_variable _ready;
    mutex              _hold;
    unique_lock<mutex> lock{_hold};
    auto t = async([&]{
        this_thread::sleep_for(seconds(1)); // time for pop
        {
        unique_lock<mutex> lock{_hold}; // get a lock for the push
        cout << "push()" << endl;
        }
        _ready.notify_one();
        cout << "notify_one()" << endl;
    }); // does this noification wait?
    cout << "pop()" << endl; // should be empty
    this_thread::sleep_for(seconds(2)); // time for push and notify before wait
    _ready.wait(lock); // hang...
    cout << "wait()" << endl;
#endif
    auto start = chrono::high_resolution_clock::now();
    time();
    cout << chrono::duration_cast<chrono::milliseconds>
        (chrono::high_resolution_clock::now()-start).count() << endl;
    start = chrono::high_resolution_clock::now();
    time();
    cout << chrono::duration_cast<chrono::milliseconds>
        (chrono::high_resolution_clock::now()-start).count() << endl;
#if 0
    pair<int, int> a[100];
    int count = 0;
    auto cmp = [](const auto& x, const auto& y){ return x.first <= y.first; };
    transform(begin(a), end(a), begin(a), [&](pair<int, int>& x){ ++count; return make_pair(count % 10, count); });
    make_heap(begin(a), end(a), cmp);
    for (auto f = begin(a), l = end(a); f != l; --l) {
        pop_heap(f, l, cmp);
        cout << (l - 1)->first << ", " << (l - 1)->second << endl;
    }
#endif
}
#endif

#if 0
/*
promise - - - > shared
future  ------> shared
shared  } - - > group
group   }-----> shared (via futures)
group
    task
*/
template <typename T>
class packaged_task;
template <typename T>
class promise {
    struct shared_state {
        using queue_type = list<function<void(T)>>;
        aligned_storage_t<sizeof(T)> _result;
        mutex   _mutex;
        bool _resolved{false};
        queue_type _q;
        const T& get() const {
            return *static_cast<const T*>(static_cast<const void*>(&_result));
        }
        ~shared_state() {
            if (_resolved) get().~T();
        }
        void set_value(T x) {
            new (&_result)T(move(x));
            queue_type tmp;
            {
                unique_lock<mutex> lock{_mutex};
                _resolved = true;
                swap(_q, tmp);
            }
            execute(tmp);
        }
        void execute(const queue_type& q) {
            for (const auto& f : q) f(get());
        }
        template <typename F>
        auto then(F f) {
            packaged_task<result_of_t<F(T)>(T)> p{move(f)};
            auto result = p.get_future();
            queue_type tmp{move(p)};
            {
                unique_lock<mutex> lock{_mutex};
                if (!_resolved) _q.splice(_q.end(), tmp);
            }
            execute(tmp);
            return result;
        }
    };
    weak_ptr<shared_state> _shared;
  public:
    class future {
        shared_ptr<shared_state> _shared;
        friend promise;
        explicit future(shared_ptr<shared_state> shared) : _shared(move(shared)) { }
      public:
        future() = default;
        bool valid() const { return _shared; }
        template <typename F>
        auto then(F f) -> typename promise<result_of_t<F(const T&)>>::future {
            return _shared->then(move(f));
        }
    };
    future get_future() {
        shared_ptr<shared_state> shared = make_shared<shared_state>();
        _shared = shared;
        return future(shared);
    }
    template <typename F, typename... Args>
    void set_to_result(F f, Args&&... args) const {
        auto lock = _shared.lock();
        if (lock) lock->set_value(f(std::forward<Args>(args)...));
    }
};
template <typename R, typename... Args>
class packaged_task<R(Args...)> {
    function<R(Args...)> _f;
    promise<R> _p;
  public:
    using result_type = void;
    template <typename F>
    explicit packaged_task(F f) : _f(move(f)) { }
    auto get_future() { return _p.get_future(); }
    void operator()(Args&&... args) const {
        _p.set_to_result(_f, std::forward<Args>(args)...);
    }
};
template <typename T>
using future = typename promise<T>::future;
template <typename> class group;
template <typename... Args>
class group<void(Args...)> {
    atomic<size_t>      _count{sizeof...(Args)};
    function<void()>    _f;
    void available() {
        if (--_count == 0) _f();
    }
};
template <typename T>
class notification_queue {
    list<T> _q;
    bool    _done = false;
    mutex   _mutex;
    condition_variable _ready;
  public:
    void push_notify(T x) {
        list<T> tmp{move(x)};
        {
            unique_lock<mutex> lock{_mutex};
            _q.splice(_q.end(), tmp);
        }
        _ready.notify_one();
    }
    void done_notify() {
        {
            unique_lock<mutex> lock{_mutex};
            _done = true;
        }
        _ready.notify_all();
    }
    list<T> wait_pop() {
        list<T> tmp;
        {
            unique_lock<mutex> lock{_mutex};
            while (_q.empty() && !_done) _ready.wait(lock);
            if (!_q.empty()) tmp.splice(tmp.end(), _q, _q.begin());
        }
        return tmp;
    }
};
class thread_pool {
    vector<thread> _pool;
  public:
    template <typename F>
    explicit thread_pool(F f) {
        for (auto n = thread::hardware_concurrency(); n != 0; --n) {
            _pool.emplace_back(f);
        }
    }
    ~thread_pool() {
        for (auto& e : _pool) e.join();
    }
};
class task_system {
    notification_queue<function<void()>> _q;
    thread_pool _pool;
  public:
    task_system() : _pool{[&]{
        while (true) {
            auto e = _q.wait_pop();
            if (e.empty()) break;
            e.front()();
        }
    }} { }
    ~task_system() { _q.done_notify(); }
    template <typename F>
    auto async(F f) -> future<result_of_t<F()>> {
        auto p = packaged_task<result_of_t<F()>()>(move(f));
        auto result = p.get_future();
        _q.push_notify(p);
        return result;
    }
};
int main() {
    mutex out_mutex;
    task_system tasks;
    auto x = tasks.async([&]{
        unique_lock<mutex> lock(out_mutex);
        cout << "initial" << endl;
        return 42;
    }).then([](int a){
        cout << "a:" << a << endl;
        return a * 2;
    }) /* .then([&](int a){
        unique_lock<mutex> lock(out_mutex);
        cout << "a:" << a << endl;
        return 1;
    }) */;
    vector<future<int>> a;
    for (auto n = 0; n != 20; ++n) {
        a.push_back(tasks.async([n,&out_mutex]{
            this_thread::sleep_for(seconds(n % 3));
            unique_lock<mutex> lock(out_mutex);
            cout << "pass one:" << n << endl;
            return 1;
        }));
    }
    for (auto n = 0; n != 20; ++n) {
        a[n] = tasks.async([n,&out_mutex]{
            this_thread::sleep_for(seconds(n % 3));
            unique_lock<mutex> lock(out_mutex);
            cout << "pass 2:" << n << endl;
            return 1;
        });
    }
   this_thread::sleep_for(seconds(30));
}
#endif



#if 0
#include <iostream>
using namespace std;
template <typename F>
void make_callable(F f) { cout << "non-member" << endl; }
template <typename R, typename T>
void make_callable(R (T::* m)) { cout << "data member" << endl; }
template <typename R, typename T, typename... Args>
void make_callable(R (T::* m)(Args...)) { cout << "member function" << endl; }
template <typename R, typename T, typename... Args>
void make_callable(R (T::* m)(Args...) const) { cout << "const member function" << endl; }
#if 0
struct test {
    int data;
    void function() const { }
};
#endif
template <typename R, typename O>
auto find_if(const R& r, O op)
{ return std::find(begin(r), end(r), std::bind(op, std::placeholders::_1)); }
struct test {
    bool is_selected() const { return true; }
};
int main() {
    test a[] = { test(), test(), test() };
    auto p = find_if(a, &test::is_selected);
}
#endif


#if 0
#include <algorithm>
#include <utility>
#include <cassert>
#include <iostream>
#include <functional>
#include <adobe/algorithm.hpp>
namespace details {
template <typename R, typename T, typename ...Args>
struct callable {
    using result_type = R;
    explicit callable(R (T::* p)(Args...)) : _p(p) { }
    template <typename... Brgs>
    auto operator()(T& x, Brgs&&... args) const { return (x.*_p)(std::forward<Brgs>(args)...); }
    template <typename... Brgs>
    auto operator()(T* x, Brgs&&... args) const { return (x->*_p)(std::forward<Brgs>(args)...); }
    R (T::* _p)(Args...);
};
template <typename T, typename ...Args>
struct callable<void, T, Args...> {
    using result_type = void;
    explicit callable(void (T::* p)(Args...)) : _p(p) { }
    template <typename... Brgs>
    void operator()(T& x, Brgs&&... args) const { (x.*_p)(std::forward<Brgs>(args)...); }
    template <typename... Brgs>
    void operator()(T* x, Brgs&&... args) const { (x->*_p)(std::forward<Brgs>(args)...); }
    void (T::* _p)(Args...);
};
template <typename R, typename T, typename ...Args>
struct callable_const{
    using result_type = R;
    explicit callable_const(R (T::* p)(Args...) const) : _p(p) { }
    template <typename... Brgs>
    auto operator()(const T& x, Brgs&&... args) const { return (x.*_p)(std::forward<Brgs>(args)...); }
    template <typename... Brgs>
    auto operator()(const T* x, Brgs&&... args) const { return (x->*_p)(std::forward<Brgs>(args)...); }
    R (T::* _p)(Args...) const;
};
template <typename T, typename ...Args>
struct callable_const<void, T, Args...> {
    using result_type = void;
    explicit callable_const(void (T::* p)(Args...) const) : _p(p) { }
    template <typename... Brgs>
    void operator()(T& x, Brgs&&... args) const { (x.*_p)(std::forward<Brgs>(args)...); }
    template <typename... Brgs>
    void operator()(const T& x, Brgs&&... args) const { (x.*_p)(std::forward<Brgs>(args)...); }
    template <typename... Brgs>
    void operator()(T* x, Brgs&&... args) const { (x->*_p)(std::forward<Brgs>(args)...); }
    template <typename... Brgs>
    void operator()(const T* x, Brgs&&... args) const { (x->*_p)(std::forward<Brgs>(args)...); }
    void (T::* _p)(Args...) const;
};
}
template <typename F>
auto make_callable(F f) { return f; }
template <typename R, typename T>
auto make_callable(R (T::* m)) { return std::bind(m, std::placeholders::_1); }
template <typename R, typename T, typename... Args>
auto make_callable(R (T::* m)(Args...)) { return details::callable<R, T, Args...>(m); }
template <typename R, typename T, typename... Args>
auto make_callable(R (T::* m)(Args...) const) { return details::callable_const<R, T, Args...>(m); }
using namespace std;
enum class color {
    red,
    green,
    blue,
    magenta,
    cyan
};
const pair<color, size_t> map_[] = {
    { color::red, 3 },
    { color::green, 1 },
    { color::blue, 2 },
    { color::magenta, 0 }};
const auto compare_project_2 = [](auto compare, auto project) {
    return [=](const auto& x, const auto& y) { return compare(project(x), project(y)); };
};
const auto compare_project_1 = [](auto compare, auto project) {
    return [=](const auto& x, const auto& y) { return compare(project(x), y); };
};
struct test {
    void member() const { cout << "called member" << endl; }
    const char* member1() const { return "member1"; }
    const char* member2(int) const { return "member2"; }
    const char* member3(int, int) const { return "member3"; }
    void member4(int, int) const { cout << "called member4" << endl; }
    void member5(int, int) { cout << "called member5" << endl; }
};
struct make_invokable_fn
{
   template<typename R, typename T>
   auto operator()(R T::* p) const -> decltype(std::mem_fn(p))
   {
       return std::mem_fn(p);
   }
   template<typename T, typename U = std::decay_t<T>>
   auto operator()(T && t) const ->
       enable_if_t<!std::is_member_pointer<U>::value, T>
   {
       return std::forward<T>(t);
   }
};
constexpr make_invokable_fn invokable {};
template<typename R, typename O>
auto find_if(const R& r, O op)
{ return std::find_if(begin(r), end(r), std::bind(op, std::placeholders::_1)); }
struct test2 {
   bool is_selected() const { cout << "is_selected" << endl; return true; }
};
int main() {
   test2 a[] = { test2(), test2(), test2() };
   //auto p = find_if(a, &test2::is_selected);
   //auto b = bind(&test2::is_selected, placeholders::_1);
   auto p = find_if(a, bind(&test2::is_selected, placeholders::_1));
}
#if 0
int main() {
    assert(adobe::is_sorted(map_, less<>(), &pair<color, size_t>::first) && "map must be sorted");
    color to_find = color::cyan;
    auto p2 = adobe::lower_bound(map_, to_find, less<>(), &pair<color, size_t>::first);
    if (p2 == end(map_) || p2->first != to_find) cout << "not found" << endl;
    else cout << p2->second << endl;
    auto p = &pair<color, size_t>::second;
    pair<color, size_t> x = { color::cyan, 42 };
    cout <<  x.*p << endl;
    cout << std::bind(&pair<color, size_t>::second, std::placeholders::_1)(make_pair(color::magenta, size_t(42))) << endl;
    cout << make_callable(&pair<color, size_t>::second)(make_pair(color::magenta, size_t(42))) << endl;
    make_callable(&test::member)(test());
    cout << make_callable(&test::member1)(test()) << endl;
    cout << make_callable(&test::member2)(test(), 42) << endl;
    cout << make_callable(&test::member3)(test(), 42, 42) << endl;
    make_callable(&test::member4)(test(), 10, 10);
    auto t = test();
    make_callable(&test::member5)(t, 10, 10);
    invokable(&test::member)(test());
    cout << invokable(&test::member1)(test()) << endl;
    cout << invokable(&test::member2)(test(), 42) << endl;
    cout << invokable(&test::member3)(test(), 42, 42) << endl;
    make_callable(&test::member4)(test(), 10, 10);
    invokable(&test::member5)(t, 10, 10);
}
#endif
#endif

























/*
    Can I build a complete, lock-free, micro tasking system? Using only C++14
    Thread pool / task queue
    Future, then, group
    task cancelation
    producer / consumer co-routine with await
*/





#if 0
#include <cassert>
#include <vector>
#define rcNULL 0
struct ZFileSpec {
};
bool operator!=(const ZFileSpec&, const ZFileSpec&);
struct TrackedFileInfo {
};
using PTrackedFileInfo = TrackedFileInfo*;
struct LinkedFileInfo;
using PLinkedFileInfo = LinkedFileInfo*;
struct LinkedFileInfo {
    PTrackedFileInfo GetTrackedFileInfo();
    ZFileSpec GetTrackedFileSpec(bool);
    bool TreatAsInstance(PLinkedFileInfo);
};
struct TrackedLinkedFileInfoList {
    using iterator = PLinkedFileInfo*;
    bool empty();
    iterator begin();
    iterator end();
};
struct TImageDocument {
    TrackedLinkedFileInfoList* GetTrackedLinkedFileInfoList();
};
struct ImageState {
};
struct ImageStateEditor {
    explicit ImageStateEditor(TImageDocument*);
    ImageState GetImageState();
    void Defloat();
};
bool DocumentExists(TImageDocument*);
bool HasFrontImage();
bool IsImageDocumentFront(TImageDocument*);
bool IsSimpleSheet(TImageDocument*);
using namespace std;
void UpdateLinkedFilesForDocument(TImageDocument* document, const ZFileSpec* onlyThisLink,
        bool forceUpdate )
{
    if (!DocumentExists(document)) return;
    assert(document);
    if (!forceUpdate && (!HasFrontImage() || !IsImageDocumentFront(document))) return;
    if ( IsSimpleSheet(document) ) return;
    bool didMakeChanges = false;
    TrackedLinkedFileInfoList* trackList = document->GetTrackedLinkedFileInfoList();
    if ( trackList->empty () ) return;
    vector<PLinkedFileInfo> updatedFileInfos;
    ImageStateEditor editor(document);
    ImageState oldState = editor.GetImageState ();
    editor.Defloat ();
    // check list of tracked attached to document's current (image) state
    for (TrackedLinkedFileInfoList::iterator item = trackList->begin();
        item != trackList->end();
        ++item )
    {
        PLinkedFileInfo existingInfo = *item;
        PTrackedFileInfo trackInfo = existingInfo->GetTrackedFileInfo ();
        if (trackInfo == rcNULL) continue;
        ZFileSpec itemSpec = existingInfo->GetTrackedFileSpec (true);
        if (onlyThisLink != NULL && itemSpec != *onlyThisLink) continue;
        bool processedPseudoInstance = false;
        for (std::size_t index = 0; index < updatedFileInfos.size(); ++index)
        {
            if (existingInfo->TreatAsInstance (updatedFileInfos[index]))
            {
                processedPseudoInstance = true;
                break;
            }
        }
        if (processedPseudoInstance) continue;
        bool externalFileStatusChanged = false;
        if (UpdateTrackedFileStatus (existingInfo, externalFileStatusChanged))
        {
            bool fileMatchesLastLoadFailure = (trackInfo->fFileStamp == existingInfo->fLastLoadFailureFileStamp);
            bool doUpdateFile = trackInfo->ShouldAutoUpdate()
                && (existingInfo->fExternalFileDiffers && !fileMatchesLastLoadFailure);
            if (doUpdateFile)
            {
                updatedFileInfos.push_back(existingInfo);
                if (externalFileStatusChanged) UpdateLayersPanel();
                CProgress progress ("$$$/Links/Progress/UpdateSmartObjects=Updating Smart Objects");
                AFile theFile( NewFile('    ', '    ', !kDataOpen) );
                theFile->SpecifyWithZFileSpec( itemSpec );
                FailOSErr (theFile->OpenFile ());
                bool isExternalLink = existingInfo->GetIsExternalLinkedFile();
                PLinkedFileInfo newLinkedFileInfo = TLinkedFileInfo::MakeFromFile( theFile.get(), isExternalLink, document, existingInfo );
                theFile.reset();
                bool didMakeChangesToThisItem = UpdateOnePlacedFile( existingInfo, newLinkedFileInfo, editor, document );
                didMakeChanges = didMakeChanges || didMakeChangesToThisItem;
            }
        }
    }
    if ( didMakeChanges )
    {
        document->SetImageState(oldState, csCurrent, isfPreserveVisibility, kRedraw);
        editor.InvalidateAllPixels();
        editor.InvalidateChannels(CompositeChannels ( editor.GetImageMode() ));
        editor.InvalidateSheetChannels();
        editor.InvalidateMerged();
        editor.InvalidateThumbnailCache();
        SetPendingChannelStateToAdjustedState (document, editor.GetImageState(), false);
        ValidatePendingChannelState (document, editor.GetImageState());
        editor.VerifyComps ();
        CommandInfo cmdInfo (cPlacedLayerUpdate,
                "$$$/Commands/UpdateSmartObjects=Update Smart Objects",
                GetStringID(kupdatePlacedLayerStr),
                gNullActionDescriptor,
                kDialogOptional);
                editor.PostCommand (cmdInfo, document, csPending);
                editor.PostNonUndoableCommand (cmdInfo, document, 0, csCurrent);
    }
}
#endif


#if 0
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <array>
#include <boost/multiprecision/cpp_int.hpp>
using namespace std;
using namespace placeholders;
struct annotate {
    annotate() { cout << "annotate ctor" << endl; }
    annotate(const annotate&) { cout << "annotate copy-ctor" << endl; }
    annotate(annotate&&) noexcept { cout << "annotate move-ctor" << endl; }
    annotate& operator=(const annotate&) { cout << "annotate assign" << endl; return *this; }
    annotate& operator=(annotate&&) noexcept { cout << "annotate move-assign" << endl; return *this; }
    ~annotate() { cout << "annotate dtor" << endl; }
    friend inline void swap(annotate&, annotate&) { cout << "annotate swap" << endl; }
    friend inline bool operator==(const annotate&, const annotate&) { return true; }
    friend inline bool operator!=(const annotate&, const annotate&) { return false; }
};
template <typename T>
class remote {
    unique_ptr<T> data_;
 public:
    remote() = default;
    template <typename... Args>
    remote(Args&&... args) : data_(new T(forward<Args>(args)...)) { }
  //   remote(T x) : data_(new T(move(x))) { }
    operator const T& () const { return *data_; }
    operator T& () { return *data_; }
    const T& get() const { return *data_; }
    T& get() { return *data_; }
    remote(const remote& x) : data_(new T(x)) { }
    remote(remote&& x) noexcept = default;
    remote& operator=(remote&& x) noexcept = default;
    remote& operator=(const remote& x) {
        remote tmp(x); *this = move(tmp); return *this;
    }
};
template <class T, class N, class O>
T power(T x, N n, O op)
{
    if (n == 0) return identity_element(op);
    while ((n & 1) == 0) {
        n >>= 1;
        x = op(x, x);
    }
    T result = x;
    n >>= 1;
    while (n != 0) {
        x = op(x, x);
        if ((n & 1) != 0) result = op(result, x);
        n >>= 1;
    }
    return result;
}
template <typename N>
struct multiply_2x2 {
    array<N, 4> operator()(const array<N, 4>& x, const array<N, 4>& y)
    {
        return { x[0] * y[0] + x[1] * y[2],
                 x[0] * y[1] + x[1] * y[3],
                 x[2] * y[0] + x[3] * y[2],
                 x[2] * y[1] + x[3] * y[3] };
    }
};
template <typename N>
array<N, 4> identity_element(const multiply_2x2<N>&) { return { N(1), N(0), N(0), N(1) }; }
template <typename N>
N fibonacci(N n) {
    if (n == 0) return N();
    return power(array<N, 4>{ 1, 1, 1, 0 }, N(n - 1), multiply_2x2<N>())[0];
}
using namespace boost::multiprecision;
int main() {
    cout << fibonacci(cpp_int(20000)) << endl;
}
int main() {
    // auto x = f(annotate());
    remote<pair<int, string>> x = { 52, "Hello World!" };
    auto y = remote<annotate>(annotate());
    auto z = remote<annotate>(annotate());
    assert(y == z);
    swap(y, z);
}
#endif




#if 0
#include <iostream>
#include <vector>
#include <range/v3/container/push_back.hpp>
using namespace std;
using namespace ranges;
int main() {
    vector<int> x { 0, 1, 2, 3, 4 };
    vector<int> y { 5, 6, 7, 8, 9 };
    push_back(x, y);
    for (const auto& e : x) cout << e << " ";
    cout << endl;
}
#endif

#if 0 // KEEP CLASS
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
using namespace std;
using namespace placeholders;
struct annotate {
    annotate() { cout << "annotate ctor" << endl; }
    annotate(const annotate&) { cout << "annotate copy-ctor" << endl; }
    annotate(annotate&&) noexcept { cout << "annotate move-ctor" << endl; }
    annotate& operator=(const annotate&) { cout << "annotate assign" << endl; return *this; }
    annotate& operator=(annotate&&) noexcept { cout << "annotate move-assign" << endl; return *this; }
    ~annotate() { cout << "annotate dtor" << endl; }
    // friend inline void swap(annotate&, annotate&) { cout << "annotate swap" << endl; }
     friend inline bool operator==(const annotate&, const annotate&) { return true; }
     friend inline bool operator!=(const annotate&, const annotate&) { return false; }
};
template <typename T>
class remote {
    unique_ptr<T> data_;
 public:
    remote(); // WHAT SHOULD THIS DO?
    remote(T x) : data_(new T(move(x))) { }
    operator const T& () const { return *data_; }
    operator T& () { return *data_; }
    remote(const remote& x) : data_(new T(x)) { }
    remote(remote&& x) noexcept = default;
    remote& operator=(remote&& x) noexcept = default;
    remote& operator=(const remote& x) { *data_ = x; return *this; } // PROBLEM
};
remote<annotate> f(remote<annotate> x) { return x; }
int main() {
    // auto x = f(annotate());
    auto y = remote<annotate>(annotate());
    auto z = remote<annotate>(annotate());
    assert(y == z);
    swap(y, z);
}
#endif










#if 0
#include <iostream>
#include <functional>
#include <memory>
#include <cassert>
#include <utility>
using namespace std;
/*
    Problem: We want 'any' to be serializable. But 'any' can be implicitly constructed
    from a type T making it difficult to determine if T is serializable or if T converted to an
    'any' is serializable.
    
    Our stream operator is defined as:
    
    ostream& operator<<(ostream& out, const const_explicit_any& x);
    
    The const_explicit_any class provides a simple implicit conversion from 'any'.
    Only one implicit user conversion is allowed, so we cannot go T -> any -> const_explicit_any.
*/
struct any;
struct const_explicit_any {
    const_explicit_any(const any& x) : p_(&x) { }
    const any* p_;
};
class any {
  public:
    template <typename T>
    any(T x) : self(make_unique<model<T>>(move(x))) { }
    //...
    bool is_serializable() const { return self->is_serializable(); }
    friend inline ostream& operator<<(ostream& out, const const_explicit_any& x)
    { return x.p_->self->serialize(out); }
  private:
    struct concept {
        virtual ostream& serialize(ostream& out) const = 0;
        virtual bool is_serializable() const = 0;
    };
    template <typename T>
    struct model : concept {
        model(T x) : object(move(x)) { }
        ostream& serialize(ostream& out) const override { return out << "any:" << object; }
        bool is_serializable() const override { return true; }
        T object;
    };
    unique_ptr<concept> self;
};
int main() {
    cout << any(10) << endl;
    // cout << any(make_pair(10, 10)) << endl; // COMPILER ERROR!
    cout << 10 << endl;
    // cout << make_pair(10, 10) << endl;  // COMPILER ERROR!
}
#endif



#if 0
#include <iostream>
#include <functional>
#include <memory>
#include <cassert>
#include <utility>
using namespace std;
/*
    Problem: We want 'any' to be optionally serializable. But 'any' can be implicitly constructed
    from a type T making it difficult to determine if T is serializable or if T converted to an
    'any' is serializable.
    
    To detect we SFINAE on this expression:
    
        declval<ostream&>() << declval<T>()
    Our stream operator is defined as:
    
    ostream& operator<<(ostream& out, const const_explicit_any& x);
    
    The const_explicit_any class provides a simple implicit conversion from 'any'.
    Only one implicit user conversion is allowed, so we cannot go T -> any -> const_explicit_any.
*/
struct any;
// template <typename T> struct convert { convert(); operator T() const; };
template <typename T> false_type direct_serializable(...);
template <typename T> true_type direct_serializable(decltype(&(declval<ostream&>() << declval<T>())));
struct const_explicit_any {
    const_explicit_any(const any& x) : p_(&x) { }
    const any* p_;
};
class any {
  public:
    template <typename T>
    any(T x) : self(make_unique<model<T, decltype(direct_serializable<T>(0))::value>>(move(x))) { }
    //...
    bool is_serializable() const { return self->is_serializable(); }
    friend inline ostream& operator<<(ostream& out, const const_explicit_any& x)
    { return x.p_->self->serialize(out); }
  private:
    struct concept {
        virtual ostream& serialize(ostream& out) const = 0;
        virtual bool is_serializable() const = 0;
    };
    template <typename, bool> struct model;
    template <typename T>
    struct model<T, true> : concept {
        model(T x) : object(move(x)) { }
        ostream& serialize(ostream& out) const override { return out << "any:" << object; }
        bool is_serializable() const override { return true; }
        T object;
    };
    template <typename T>
    struct model<T, false> : concept {
        model(T x) : object(move(x)) { }
        ostream& serialize(ostream& out) const override { return out << "any: <not serializable>"; }
        bool is_serializable() const override { return false; }
        T object;
    };
    unique_ptr<concept> self;
};
int main() {
    cout << any(10) << endl;
    cout << any(make_pair(10, 10)) << endl; // Runtime failure
    cout << 10 << endl;
    // cout << make_pair(10, 10) << endl;  // COMPILER ERROR!
}
#endif

#if 0
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;
using namespace placeholders;
struct annotate {
    annotate() { cout << "annotate ctor" << endl; }
    annotate(const annotate&) { cout << "annotate copy-ctor" << endl; }
    annotate(annotate&&) noexcept { cout << "annotate move-ctor" << endl; }
    annotate& operator=(const annotate&) { cout << "annotate assign" << endl; return *this; }
    annotate& operator=(annotate&&) noexcept { cout << "annotate move-assign" << endl; return *this; }
    ~annotate() { cout << "annotate dtor" << endl; }
    friend inline void swap(annotate&, annotate&) { cout << "annotate swap" << endl; }
    friend inline bool operator==(const annotate&, const annotate&) { return true; }
    friend inline bool operator!=(const annotate&, const annotate&) { return false; }
};
template <typename T>
void g(T&& x) {
    f(x);
}
template <typename T>
void f(T&& x) {
    cout << is_reference<T>::value << is_const<typename remove_reference<T>::type>::value << endl;
}
const annotate f() { return annotate(); }
int main() {
    g(annotate());
    g(f());
    annotate x;
    g(x);
    const annotate y;
    g(y);
}
#endif


#if 0
// SAVE - class
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;
using namespace placeholders;
struct annotate {
    annotate() { cout << "annotate ctor" << endl; }
    annotate(const annotate&) { cout << "annotate copy-ctor" << endl; }
    annotate(annotate&&) noexcept { cout << "annotate move-ctor" << endl; }
    annotate& operator=(const annotate&) { cout << "annotate assign" << endl; return *this; }
    annotate& operator=(annotate&&) noexcept { cout << "annotate move-assign" << endl; return *this; }
    ~annotate() { cout << "annotate dtor" << endl; }
    friend inline void swap(annotate&, annotate&) { cout << "annotate swap" << endl; }
    friend inline bool operator==(const annotate&, const annotate&) { return true; }
    friend inline bool operator!=(const annotate&, const annotate&) { return false; }
};
template <typename T>
class remote {
    T* data_;
 public:
    remote(T x) : data_(new T(move(x))) { }
    operator const T& () const { return *data_; }
    operator T& () { return *data_; }
    ~remote() { delete data_; }
    remote(const remote& x) : data_(new T(x)) { }
    remote& operator=(const remote& x) { *data_ = x; return *this; }
};
annotate f(annotate x) { return x; }
int main() {
#if 0
    remote<annotate> x = annotate();
    x = annotate();
    // remote<int> y = 20;
#else
    auto x = f(annotate());
#endif
   // x = y;
}
#endif


























































#if 0
#include <iostream>
using namespace std;
struct annotate {
    annotate() { cout << "annotate ctor" << endl; }
    annotate(const annotate&) { cout << "annotate copy-ctor" << endl; }
    annotate(annotate&&) noexcept { cout << "annotate move-ctor" << endl; }
    annotate& operator=(const annotate&) { cout << "annotate assign" << endl; return *this; }
    annotate& operator=(annotate&&) noexcept { cout << "annotate move-assign" << endl; return *this; }
    ~annotate() { cout << "annotate dtor" << endl; }
    friend inline void swap(annotate&, annotate&) { cout << "annotate swap" << endl; }
    friend inline bool operator==(const annotate&, const annotate&) { return true; }
    friend inline bool operator!=(const annotate&, const annotate&) { return false; }
};
template <typename T>
class remote {
    T* data_;
  public:
    remote(T x) : data_(new T(move(x))) { }
    ~remote() { delete data_; }
    operator const T&() const { return *data_; }
    remote(const remote&) = delete;
    remote operator=(const remote&) = delete;
};
int main() {
    remote<annotate> x{annotate()};
    x = annotate();
#if 0
    remote<annotate> x{annotate()};
    annotate y = x;
    remote<annotate> z(y);
    cout << (x == z) << endl;
#endif
};
#endif



#if 0
#include <iostream>
using namespace std;
struct annotate {
    annotate() { cout << "annotate ctor" << endl; }
    annotate(const annotate&) { cout << "annotate copy-ctor" << endl; }
    annotate(annotate&&) noexcept { cout << "annotate move-ctor" << endl; }
    annotate& operator=(const annotate&) { cout << "annotate assign" << endl; return *this; }
    annotate& operator=(annotate&&) noexcept { cout << "annotate move-assign" << endl; return *this; }
    ~annotate() { cout << "annotate dtor" << endl; }
    friend inline void swap(annotate&, annotate&) { cout << "annotate swap" << endl; }
    friend inline bool operator==(const annotate&, const annotate&) { return true; }
    friend inline bool operator!=(const annotate&, const annotate&) { return false; }
};
template <typename T>
T sink(T x) { return x; }
int main() {
    auto x = sink(annotate());
}
#endif


#if 0
#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
using namespace std;
template <typename F> // F models UnaryFunction
struct pipeable {
    F op_;
    template <typename T>
    auto operator()(T&& x) const { return op_(forward<T>(x)); }
};
template <typename F>
auto make_pipeable(F x) { return pipeable<F>{ move(x) }; }
template <typename F, typename... Args>
auto make_pipeable(F x, Args&&... args) {
    return make_pipeable(bind(move(x), placeholders::_1, forward<Args>(args)...));
}
template <typename T, typename F>
auto operator|(T&& x, const pipeable<F>& op) { return op(forward<T>(x)); }
template <typename F>
auto sort(F op)
{
    return make_pipeable([](auto x, auto op) {
        sort(begin(x), end(x), move(op));
        return x;
    }, op);
}
auto sort()
{
    return sort(less<>());
}
auto unique() {
    return make_pipeable([](auto x) {
        auto p = unique(begin(x), end(x));
        x.erase(p, end(x));
        return x;
    });
}
struct annotate {
    annotate() { cout << "annotate ctor" << endl; }
    annotate(const annotate&) { cout << "annotate copy-ctor" << endl; }
    annotate(annotate&&) noexcept { cout << "annotate move-ctor" << endl; }
    annotate& operator=(const annotate&) { cout << "annotate assign" << endl; return *this; }
    annotate& operator=(annotate&&) noexcept { cout << "annotate move-assign" << endl; return *this; }
    ~annotate() { cout << "annotate dtor" << endl; }
    friend inline void swap(annotate&, annotate&) { cout << "annotate swap" << endl; }
    friend inline bool operator==(const annotate&, const annotate&) { return true; }
    friend inline bool operator!=(const annotate&, const annotate&) { return false; }
    friend inline bool operator<(const annotate&, const annotate&) { return false; }
};
int main() {
    vector<annotate> c;
    c.push_back(annotate());
    c.push_back(annotate());
    auto x = move(c) | sort() | unique();
   //  auto x = vector<int>{10, 9, 9, 8, 6, 5, 5, 5, 2, 1 } | sort(greater<>()) | unique();
   // auto x = container::unique(container::sort(vector<int>{10, 9, 9, 8, 6, 5, 5, 5, 2, 1 }));
    //for (const auto& e : x) cout << e << " ";
    cout << endl;
}
#endif


#if 0
#include <memory>
#include <exception>
/*
future with then() taking function directly
exception propegation
task cancelation (throwing exception cancels subsequent tasks), destructing future cancels the task.
simple interface. Do we need promise future pair or just create future and copy it?
no get().
*/
namespace future_ {
template <typename T>
struct shared {
};
} // namespace future_
template <typename T>
class future {
    future();
};
template <typename T>
class promise {
    std::shared_ptr<future_::shared<T>> shared_;
public:
    void set_value(const T&);
    void set_value(T&&);
    void set_exception(std::exception_ptr);
    future<T> get_future();
};
#endif

#if 0
#include <memory>
namespace adobe {
template <typename T>
class remote {
    std::unique_ptr<T> part_;
public:
    template <typename... Args>
    remote(Args&&... args) : part_(new T(std::forward<Args>(args)...)) { }
    remote(const remote& x) : part_(new T(x)) { }
    remote(remote&&) noexcept = default;
    remote& operator=(const remote& x) { remote tmp = x; *this = std::move(tmp); return *this; }
    remote& operator=(remote&&) noexcept = default;
    //remote& operator=(T x) noexcept { *this = remote(std::move(x)); return *this; }
    operator T&() { return *part_; }
    operator const T&() const { return *part_; }
    friend inline bool operator==(const remote& x, const remote& y)
    { return static_cast<const T&>(x) == static_cast<const T&>(y); }
    friend inline bool operator!=(const remote& x, const remote& y)
    { return !(x == y); }
    friend inline bool operator==(const T& x, const remote& y)
    { return x == y.get(); }
    friend inline bool operator!=(const T& x, const remote& y)
    { return !(x == y); }
    friend inline bool operator==(const remote& x, const T& y)
    { return static_cast<const T&>(x) == y; }
    friend inline bool operator!=(const remote& x, const T& y)
    { return !(x == y); }
    T& get() { return *part_; }
    const T& get() const { return *part_; }
};
} // namespace adobe
#include <iostream>
using namespace adobe;
using namespace std;
int main() {
    remote<pair<int, double>> x(5, 3.0);
    remote<pair<int, double>> y = { 5, 32.0 };
    //x = "world";
    cout << x.get().first << y.get().second << endl;
}
#endif



#if 0
#include <iostream>
#include <cfenv>
using namespace std;
 __attribute__((noinline)) double neg_zero()
{ return -0.0; }
int main() {
    cout << 0.0 + neg_zero() << endl;
    fesetround(FE_DOWNWARD);
    cout << 0.0 + neg_zero() << endl;
    cout << neg_zero() << endl;
}
#endif


#if 0
#include <iostream>
using namespace std;
template <typename T> // T is ???
class remote {
    unique_ptr<T> part_;
  public:
    remote(T x) : part_(new T(move(x))) { }
    remote(const remote& x) : part_(new T(*x.part_)) { }
    remote& operator=(const remote& x) { *part_ = *x.part_;  return *this; }
    remote(remote&& x) noexcept = default;
    remote& operator=(remote&&) noexcept = default;
    T& operator*() { return *part_; }
    const T& operator*() const { return *part_; }
    T* operator->() { return &*part_; }
    const T* operator->() const { return &*part_; }
};
template <typename T>
bool operator==(const remote<T>& x, const remote<T>& y) {
    return *x == *y;
}
template <typename T>
bool operator!=(const remote<T>& x, const remote<T>& y) {
    return !(x == y);
}
template <typename T, typename... Args>
remote<T> make_remote(Args&&... args) { return remote<T>(T(forward<Args>(args)...)); }
int main() {
    auto x = make_remote<int>(5);
    auto y = x;
    auto z = move(x);
    x = y;
    cout << *z << endl;
    cout << *x << endl;
    cout << *y << endl;
}
#endif




























#if 0
#include <algorithm>
#include <list>
#include <iostream>
namespace adobe {
template <typename I, // I models ForwardIterator
          typename N> // N is distance_type(I)
I reverse_n(I f, N n) // return f + n
{
    if (n < 2) return std::next(f, n);
    I m = reverse_n(f, n / 2);
    I l = reverse_n(m, n - (n / 2));
    std::rotate(f, m, l);
    return l;
}
template <typename I> // I models ForwardIterator
void reverse(I f, I l)
{
    reverse_n(f, std::distance(f, l));
}
} // namespace adobe
int main() {
    double x = -0.0;
    x += 0.0;
    std::cout << x << std::endl;
    std::list<int> a = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    adobe::reverse(begin(a), end(a));
    for (const auto& e : a) std::cout << e << " ";
    std::cout << std::endl;
}
#endif

#if 0
#include <future>
#include <functional>
#include <iostream>
#include <cmath>
#include <dispatch/dispatch.h>
namespace adobe {
template <typename F, typename ...Args>
auto async(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F (Args...)>::type>
{
    using result_type = typename std::result_of<F (Args...)>::type;
    using packaged_type = std::packaged_task<result_type ()>;
    auto p = new packaged_type(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    auto result = p->get_future();
    dispatch_async_f(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
            p, [](void* f_) {
                packaged_type* f = static_cast<packaged_type*>(f_);
                (*f)();
                delete f;
            });
    
    return result;
}
} // namespace adobe
double foo(double x) { return std::sin(x*x); }
size_t search(int* a, size_t n, int x)
{
    size_t i = 0;
    while (n != 0)  {
        size_t h = n / 2;
        if (a[i + h] < x ) {
            i += h + 1;
            n -= h + 1;
        } else {
            n = h;
        }
    }
    return i;
}
using namespace std;
int main()
{
    int a[] = { 0, 1, 2, 3, 3, 3, 7, 10 };
    cout << search(a, end(a) - begin(a), -1) << endl;
    cout << search(a, end(a) - begin(a), 0) << endl;
    cout << search(a, end(a) - begin(a), 2) << endl;
    cout << search(a, end(a) - begin(a), 3) << endl;
    cout << search(a, end(a) - begin(a), 5) << endl;
    cout << search(a, end(a) - begin(a), 10) << endl;
    cout << search(a, end(a) - begin(a), 11) << endl;
  // compiles and runs ok
  auto result1=adobe::async([](){std::cout << "hello\n";});
  result1.get();
  // doesn't compile
  auto result2=adobe::async(foo,0.5);
  std::cout << result2.get() << "\n";
}
#endif

#if 0
#include <memory>
#include <chrono>
#include <iostream>
#include <functional>
#include <adobe/future.hpp>
using namespace adobe;
using namespace std;
using namespace chrono;
template <typename T>
void async_chain(T x) {
    auto shared =  make_shared<pair<T, function<void()>>>(x, function<void()>());
    shared->second = [=]{
        if (shared->first.while_()) { shared->first.body_(); adobe::async(shared->second); }
        else adobe::async(shared->first);
    };
    adobe::async(shared->second);
}
int main() {
    struct to_ten {
        int count = 0;
        bool while_() { return count != 10; }
        void body_() { ++count; }
        void operator()() { cout << "done:" << count; }
    };
    async_chain(to_ten());
    cout << "waiting..." << endl;
    this_thread::sleep_for(seconds(2));
}
#endif


#if 0
#include <cstddef>
#include <vector>
namespace adobe {
struct point {
    double x;
    double y;
};
struct rectangle {
    point top_left;
    point bottom_right;
};
struct image_t { };
class layer
{
    std::vector<layer> layers_;
  public:
    // REVISIT (sparent) : These operations are a 2x3 transform. Splitting them out for convienence
    // not as basis operations.
    void rotation(double);
    double rotation() const;
    void scale(double);
    double scale() const;
    void position(const point&);
    point position() const;
    void bounds(const rectangle&);
    const rectangle& bounds() const;
    // REVISIT (sparent) : Should use a forest or similar container. But to get going quickly
    // use a traditional approach.
    layer& parent() const;
    // count of sublayers
    std::size_t size() const { return layers_.size(); }
    void insert(std::size_t n, layer x) { layers_.insert(layers_.begin() + n, std::move(x)); }
    // layer content
    void image(image_t);
    const image_t& image() const;
    // non-basis operations (container conventions)
    void push_back(layer b) { layers_.push_back(std::move(b)); }
};
} // namespace adobe
using namespace adobe;
int main()
{
    layer root;
}
#endif

#if 0
#include <cstddef>
#include <iostream>
template <typename T> // T models integral type
std::size_t count_bits_set(T x) {
    std::size_t result = 0;
    while (x) { x &= x - 1; ++result; }
    return result;
}
using namespace std;
int main() {
    cout << count_bits_set(-1) << endl;
}
#endif


#if 0 // start of conceptual-c compiler
struct conceptual_c {
/*
    namespace   = identifier '{' declaration '}'.
    declaration = identifier '=' expression.
    expression  = lambda | ...
    lambda      = '[' ']' [ '(' argument-list ')' ] '{' '}'.
    
    max = [](&args...) require same_type(args...)
    {
    }
    
    swap = [](&x, &y) require type(x) == type(y)
    {
        tmp = unsafe::move(x);
        construct(&x, unsafe::move(y));
        y = tmp;
    }
*/
bool is_namespace()
{
    if (!is_keyword("namespace")) return false;
    auto identifier = require_identifier();
    require_token("{");
    while (is_declaration()) ;
    require_token("}");
}
}; // conceptual_c
#endif


#if 0 // demonstration of libc++ bug
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
using namespace std;
template <class T>
void check_not_copyable(T&& x)
{
    static_assert(!is_copy_constructible<typename remove_reference<T>::type>::value, "fail");
}
int main()
{
    unique_ptr<int> x(new int(42));
    check_not_copyable(bind([](const unique_ptr<int>& p) { return *p; }, move(x)));
}
#endif

#if 0
#include <iostream>
#include <type_traits>
#include <utility>
using namespace std;
template< typename T >
class M {
    T data;
	//...
 public:
    explicit M(T x) : data(move(x)) { }
    template <typename F>
    auto apply(F f) -> M<typename result_of<F(T)>::type> { cout << "case 3" << endl; return f(data); }
#if 0
	template< typename U >
	M< U > apply (M< U > (*f)( T )) { cout << "case 1" << endl; return f(data); }
		// Takes a function mapping T to M< U >
	template< typename V >	// Just to be different
	M< V > apply( V (*f)( T ) ) { cout << "case 2" << endl; return M<V>(f(data)); }
		// Takes a function mapping T to V
#endif
	//...
};
M<double> f1(int x) { cout << "f1:" << x << endl; return M<double>(x); }
double f2(int x) { cout << "f2:" << x << endl; return x; }
int main() {
    M<int> x(5);
    x.apply(f1);
    x.apply(f2);
}
#endif

#if 0
/**************************************************************************************************/
#include <dispatch/dispatch.h>
#include <adobe/future.hpp>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <Block.h>
using namespace std;
using namespace chrono;
using namespace adobe;
auto test() -> int {
    thread_local int x = rand();
    return x;
}
mutex io_;
template <typename R, typename... Args>
class function_;
enum storage_t {
    local,
    remote,
    shared
};
template <typename R, typename... Args>
class function_<R (Args...)>
{
    struct concept_t {
        virtual ~concept_t() { }
        virtual R call_(Args&&... args) = 0;
        virtual void copy_(void*) const = 0;
        virtual void move_(void*) = 0;
    };
    struct empty : concept_t {
        R call_(Args&&... args) override
        { return std::declval<R>(); }
        void copy_(void* x) const override
        { new (x) empty(); }
        void move_(void* x) noexcept override
        { new (x) empty(); this->~empty(); }
    };
    template <typename F, storage_t>
    struct model;
    template <typename F>
    struct local_store : concept_t { F f_; };
    template <typename F>
    struct remote_store :concept_t { unique_ptr<F> f_; };
    template <typename F>
    struct model<F, local> : concept_t {
        model(F&& f) : f_(move(f)) { }
        model(const F& f) : f_(f) { }
        R call_(Args&&... args) override
        { return f_(forward<Args>(args)...); }
        void copy_(void* x) const override
        { new (x) model(*this); }
        void move_(void* x) noexcept override
        { new (x) model(move(*this)); this->~model(); }
        F f_;
    };
    template <typename F>
    struct model<F, remote> : concept_t {
        model(F&& f) : f_(new F(move(f))) { }
        R call_(Args&&... args) override
        { return (*f_)(forward<Args>(args)...); }
        void copy_(void* x) const override
        { new (x) model(new F(f_)); }
        void move_(void* x) noexcept override
        { new (x) model(move(*this)); this->~model(); }
        unique_ptr<F> f_;
    };
    template <typename F>
    struct model<F, shared> : concept_t {
        model(F&& f) : f_(make_shared<F>(move(f))) { }
        model(const F& f) : f_(make_shared<F>(f)) { }
        R call_(Args&&... args) override
        { return (*f_)(forward<Args>(args)...); }
        void copy_(void* x) const override
        { new (x) model(*this); }
        void move_(void* x) noexcept override
        { new (x) model(move(*this)); this->~model(); }
        shared_ptr<F> f_;
    };
    void* data() const { return static_cast<void*>(&data_); }
    concept_t& concept() const { return *static_cast<concept_t*>(data()); }
    template <typename T, typename U>
    using disable_same_t =
        typename enable_if<!is_same<typename decay<T>::type, typename decay<U>::type>::value>::type*;
    using value_type = R (Args...);
    using max_size_t = integral_constant<size_t,
        sizeof(model<value_type, remote>) < sizeof(model<value_type, shared>) ? sizeof(model<value_type, shared>)
        : sizeof(model<value_type, remote>)>;
    template <typename T>
    using use_storage = integral_constant<storage_t,
        !is_copy_constructible<T>::value ? shared
        : (sizeof(local_store<T>) <= max_size_t::value ? local : remote)>;
    mutable typename aligned_storage<sizeof(max_size_t::value)>::type data_;
  public:
    template <typename F>
    function_(F&& f, disable_same_t<F, function_> = 0)
        {
        using type = typename remove_reference<F>::type;
        static_assert(!is_copy_constructible<type>::value, "");
        #if 0
        new (data()) model<type, use_storage<type>::value>(std::forward<F>(f));
        #endif
        }
    ~function_() { concept().~concept_t(); }
    function_(const function_& x) { x.concept().copy_(data()); }
    function_(function_&& x) { x.concept().move_(data()); }
    function_& operator=(const function_& x) { auto tmp = x; *this = move(tmp); return *this; }
    function_& operator=(function_&& x) noexcept { concept().~concept_t(); x.concept().move_(data()); }
    R operator()(Args&&... args) const { return concept().call_(forward<Args>(args)...); }
};
struct mutable_f {
    int x = 7;
    int operator()() { return ++x; }
};
template <class T>
void check_not_copyable(T&& x)
{
    static_assert(!is_copy_constructible<typename remove_reference<T>::type>::value, "copyable");
}
int main()
{
    unique_ptr<int> x(new int(42));
    //function_<int ()> f = bind([](const unique_ptr<int>& p) { return *p; }, move(x));
    check_not_copyable(x);
    //check_not_copyable(bind([](const unique_ptr<int>& p) { return *p; }, move(x)));
#if 0
    std::function<int()> fm = mutable_f();
    cout << fm() << endl;
    cout << fm() << endl;
    function_<int()> fm_ = mutable_f();
    cout << fm_() << endl;
    cout << fm_() << endl;
    function_<void ()> f1 = []{ };
    cout << f() << endl;
#endif
    shared_task_queue q;
    for (int t = 0; t != 10; ++t) {
        q.async([=]{
            this_thread::sleep_for(seconds(2));
            lock_guard<mutex> lock(io_);
            cout << "sequential: " << t << " seconds" << endl;
        });
    }
    {
    auto f = std::async([&]{
        this_thread::sleep_for(seconds(5));
        lock_guard<mutex> lock(io_);
        cout << "task done" << endl;
    });
    }
    {
    lock_guard<mutex> lock(io_);
    cout << "async?" << endl;
    }
    cout << "test: " << test() << endl;
    adobe::async([]() {
        lock_guard<mutex> lock(io_);
        cout << "test: " << test() << endl;
    });
    for (int t = 5; t != 0; --t) {
        adobe::async(steady_clock::now() + seconds(t),[=]{
            lock_guard<mutex> lock(io_);
            cout << "time: " << t << " seconds" << endl;
        });
    }
#if 0
    for (int task = 0; task != 100; ++task) {
        adobe::async([task]() {
            // sleep for 1 or 3 seconds
            this_thread::sleep_for(seconds(1 + 2 * (task & 1)));
            {
            lock_guard<mutex> lock(io_);
            auto now = system_clock::now();
            auto now_c = system_clock::to_time_t(now);
            cout << "task: " << setw(2) << task << " time: " << put_time(localtime(&now_c), "%c") << endl;
            }
        });
    }
#endif
    
    this_thread::sleep_for(seconds(60));
    cout << "Done!" << endl;
    // auto queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
}
#endif


/**************************************************************************************************/


#if 0
#include <algorithm>
#include <utility>
#include <memory>
#include <iterator>
#include <cassert>
#include <vector>
#include <chrono>
#include <iostream>
#include <thread>
#include <cmath>
using namespace std;
namespace adobe {
class unsafe { };
class int_array {
    size_t size_;
    unique_ptr<int[]> data_;
public:
    explicit int_array(size_t size) : size_(size), data_(new int[size]()) { }
    int_array(const int_array& x) : size_(x.size_), data_(new int[x.size_])
    { copy(x.data_.get(), x.data_.get() + x.size_, data_.get()); }
    int_array(int_array& x, unsafe) : size_(x.size_), data_(x.data_.get()) { }
    int_array(int_array&& x) noexcept = default;
    int_array& operator=(int_array&& x) noexcept = default;
    int_array& operator=(const int_array& x);  // **
#if 0
    int_array& operator=(const int_array& x)
    { int_array tmp = x; *this = move(tmp); return *this; }
#endif
    const int* begin() const { return data_.get(); }
    const int* end() const { return data_.get() + size_; }
    size_t size() const { return size_; }
};
bool operator==(const int_array& x, const int_array& y)
{ return (x.size() == y.size()) && equal(begin(x), end(x), begin(y)); }
template <typename T>
void move_unsafe(T& x, void* raw) { new (raw) T(x, unsafe()); }
template <typename T>
void move_unsafe(void* raw, T& x) { new (&x) T(*static_cast<T*>(raw), unsafe()); }
#if 0
void swap(int_array& x, int_array& y)
{
    aligned_storage<sizeof(int_array)>::type tmp;
    move_unsafe(x, &tmp);
    move_unsafe(y, &x);
    move_unsafe(&tmp, y);
    assert(
}
#endif
} // namespace adobe
using namespace adobe;
using container = vector<int_array>; // Change this to a deque to see a huge difference
long long __attribute__ ((noinline)) time(container& r)
{
    auto start = chrono::high_resolution_clock::now();
    
    reverse(begin(r), end(r));
    
    return chrono::duration_cast<chrono::microseconds>
        (chrono::high_resolution_clock::now()-start).count();
}
int main() {
    assert(log2f != log10f);
    this_thread::sleep_for(chrono::seconds(3));
    container a(1000000, int_array(10));
    cout << "time: " << time(a) << endl;
    int_array x(10);
    int_array y = x;
    int_array z = move(x);
    x = y;
    assert(x == y);
}
#if 0
int main() {
    int_array x(10);
    int_array y = x;
    int_array z = move(x);
    x = y;
    assert(x == y);
}
#endif
#endif

#if 0
#include <dispatch/dispatch.h>
/**************************************************************************************************/
#include <adobe/future.hpp>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <mutex>
using namespace std;
auto test() -> int {
    thread_local int x = rand();
    return x;
}
mutex io_;
int main()
{
    {
    auto f = std::async([&]{
        this_thread::sleep_for(chrono::seconds(5));
        lock_guard<mutex> lock(io_);
        cout << "task done" << endl;
    });
    }
    {
    lock_guard<mutex> lock(io_);
    cout << "async?" << endl;
    }
    cout << "test: " << test() << endl;
    adobe::async([]() {
        lock_guard<mutex> lock(io_);
        cout << "test: " << test() << endl;
    });
    for (int task = 0; task != 100; ++task) {
        adobe::async([task]() {
            // sleep for 1 or 3 seconds
            this_thread::sleep_for(chrono::seconds(1 + 2 * (task & 1)));
            {
            lock_guard<mutex> lock(io_);
            auto now = chrono::system_clock::now();
            auto now_c = chrono::system_clock::to_time_t(now);
            cout << "task: " << setw(2) << task << " time: " << put_time(localtime(&now_c), "%c") << endl;
            }
        });
    }
    cout << "Done!" << endl;
    // auto queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
}
#endif


#if 0
#include <iostream>
#include <adobe/dictionary.hpp>
#include <adobe/array.hpp>
using namespace std;
using namespace adobe;
struct annotate {
    annotate() { cout << "annotate ctor" << endl; }
    annotate(const annotate&) { cout << "annotate copy-ctor" << endl; }
    annotate(annotate&&) noexcept { cout << "annotate move-ctor" << endl; }
    annotate& operator=(const annotate&) { cout << "annotate assign" << endl; return *this; }
    annotate& operator=(annotate&&) noexcept { cout << "annotate move-assign" << endl; return *this; }
    ~annotate() { cout << "annotate dtor" << endl; }
    friend inline void swap(annotate&, annotate&) { cout << "annotate swap" << endl; }
    friend inline bool operator==(const annotate&, const annotate&) { return true; }
    friend inline bool operator!=(const annotate&, const annotate&) { return false; }
};
int main() {
    //any_regular_t x = annotate();
    dictionary_t dict;
    //dict[name_t("hello")] = move(x);
    dict[name_t("world")].assign(annotate());
    //array_t array;
    //array.emplace_back(move(x));
    vector<pair<int, int>> v;
    v.emplace_back(0, 3);
    v.push_back(pair<int, int>(0, 3));
}
#endif

#if 0
/*
    Outline-
    
    Goal: Write complete types.
*/
#include <adobe/json.hpp>
/*
    Copyright 2005-2013 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/
/******************************************************************************/
// stdc++
#include <vector>
#include <iostream>
// asl
#include <adobe/any_regular.hpp>
#include <adobe/array.hpp>
#include <adobe/dictionary.hpp>
#include <adobe/iomanip.hpp>
#include <adobe/iomanip_asl_cel.hpp>
#include <adobe/json.hpp>
/******************************************************************************/
struct helper_t {
    typedef adobe::any_regular_t    value_type;
    typedef std::string             string_type;
    typedef string_type             key_type;
    typedef adobe::dictionary_t     object_type;
    typedef adobe::array_t          array_type;
    typedef object_type::value_type pair_type;
    static json_type type(const value_type& x) {
        const std::type_info& type(x.type_info());
        if (type == typeid(object_type))      return json_type::object;
        else if (type == typeid(array_type))  return json_type::array;
        else if (type == typeid(string_type)) return json_type::string;
        else if (type == typeid(double))      return json_type::number;
        else if (type == typeid(bool))        return json_type::boolean;
        else if (type == typeid(adobe::empty_t)) return json_type::null;
        assert(false && "invalid type for serialization");
    }
    
    template <typename T>
    static const T& as(const value_type& x) { return x.cast<T>(); }
    static void move_append(object_type& obj, key_type& key, value_type& value) {
        obj[adobe::name_t(key.c_str())] = std::move(value);
    }
    static void append(string_type& str, const char* f, const char* l) {
        str.append(f, l);
    }
    static void move_append(array_type& array, value_type& value) {
        array.push_back(std::move(value));
    }
};
/******************************************************************************/
int main() {
    std::cout << "-=-=- asl_json_helper_smoke -=-=-\n";
    adobe::any_regular_t x = json_parser_t<helper_t>(u8R"raw(
        [
            42,
            12.536,
            -20.5,
            -1.375e+112,
            3.1415926535897932384626433832795028841971693993751058209,
            null,
            {
                "Country": "US",
                "State": "CA",
                "Address": "",
                "Longitude": -122.3959,
                "Zip": "94107",
                "City": "SAN FRANCISCO",
                "Latitude": 37.7668,
                "precision": "zip"
            },
            {
                "precision": "zip",
                "Latitude": 37.371991,
                "City": "SUNNYVALE",
                "Zip": "94085",
                "Longitude": -122.02602,
                "Address": "",
                "State": "CA",
                "Country": "US"
            }
        ]
    )raw").parse();
    
    json_generator<helper_t, std::ostream_iterator<char>>(std::ostream_iterator<char>(std::cout)).generate(x);
    std::cout << "-=-=- asl_json_helper_smoke -=-=-\n" << std::endl;
}
#endif

/******************************************************************************/



#if 0
#include <iostream>
#include <memory>
#include <vector>
using namespace std;
template <class T>
class foo { };
typedef shared_ptr<foo<int>> fooPtr_t;
typedef vector<fooPtr_t> fooPtrVector_t;
int main() {
    fooPtrVector_t myFooPtrVector;
    fooPtrVector_t::iterator it = myFooPtrVector.begin();
}
#endif




#if 0
#include <utility>  // move
#include <memory>   // unique_ptr
#include <cassert>  // assert
#include <functional> // function
using namespace std;
class value {
public:
    template <typename T>
    value(T x) : object_(new model<T>(move(x))) { }
    
    template <typename R, typename A>
    value(function<R (const A&)> x) : object_(new function_model<R, A>(move(x))) { }
    
    value(const value& x) : object_(x.object_->copy()) { }
    value(value&& x) noexcept = default;
    value& operator=(const value& x) { value tmp(x); *this = move(tmp); return *this; }
    value& operator=(value&& x) noexcept = default;
    
    value operator()(const value& x) const { return (*object_)(x); }
    template <typename T>
    const T* cast() const {
        return typeid(T) == typeid(value) ? static_cast<const T*>(static_cast<const void*>(this))
            : static_cast<const T*>(object_->cast(typeid(T)));
    }
private:
    struct concept {
        virtual ~concept() { }
        virtual concept* copy() const = 0;
        virtual value operator()(const value&) const = 0;
        virtual const void* cast(const type_info&) const = 0;
    };
    
    template <typename T>
    struct model : concept {
        model(T x) : data_(move(x)) { }
        concept* copy() const override { return new model(data_); }
        value operator()(const value&) const override { assert(false); }
        const void* cast(const type_info& t) const override
        { return t == typeid(T) ? &data_ : nullptr; }
        
        T data_;
    };
    
    template <typename R, typename A>
    struct function_model : concept {
        function_model(function<R (const A&)> x) : data_(move(x)) { }
        concept* copy() const override { return new function_model(data_); }
        value operator()(const value& x) const override {
            const A* p = x.cast<A>();
            assert(p);
            return data_(*p);
        }
        const void* cast(const type_info& t) const override
        { return t == typeid(function<R (const A&)>) ? &data_ : nullptr; }
        
        function<R (const A&)> data_;
    };
    
    unique_ptr<concept> object_;
};
#include <iostream>
int main() {
    value x = 10;
    value f = function<int (const int&)>([](const int& x){ return x + 42; });
    
    cout << *f(x).cast<int>() << endl;
    cout << *f(12).cast<int>() << endl;
    
    typedef std::function<value(const value&)> Function; // lambda functions from Value to Value
    value s = function<int (const int&)>([](const int& x){ return x + 1; });
    value t = function<value (const value&)>([](const value& v) { // twice
        function<value(const value&)> f = v;
        function<value(const value&)> ff = [f](const value& x) -> value {return f(f(x));};
        return ff;
    });
    cout << "s(0) = " << *s(0).cast<int>() << "\n";
    cout << "t(s)(0) = " << *t(s)(0).cast<int>() << "\n";
    cout << "t(t)(s)(0) = " << *t(t)(s)(0).cast<int>() << "\n";
    cout << "t(t)(t)(s)(0) = " << *t(t)(t)(s)(0).cast<int>() << "\n";
    cout << "t(t)(t)(t)(s)(0) = " << *t(t)(t)(t)(s)(0).cast<int>() << "\n";
}
#endif


#if 0
#include <algorithm>
#include <forward_list>
#include <iostream>
namespace adobe {
/*
    \c Range is a semiopen interface of the form [position, limiter) where limiter is one of the
    following.
*/
/*!
    \c ConvertibleToRange denotes a sequence of the form <code>[begin, end)</code>. The elements of
    the range are the beginning
    
*/
template <typename T>
using iterator_t = decltype(std::begin(std::declval<T>()));
template <typename R>
using limit_t = decltype(std::end(std::declval<R>()));
template <typename T>
using enable_if_integral = typename std::enable_if<std::is_integral<T>::value>::type*;
template <typename T>
using disable_if_integral = typename std::enable_if<!std::is_integral<T>::value>::type*;
template <typename P, typename L>
struct range {
    range(P p, L l) : position(p), limit(l) { }
    P begin() const { return position; }
    L end() const { return limit; }
    P position;
    L limit;
};
template <typename R>
using range_t = range<iterator_t<R>, limit_t<R>>;
template <typename R> // R models ConvertibleToRange
auto make_range(R&& x) -> range<iterator_t<R>, iterator_t<R>>
{ return { std::begin(x), std::end(x) }; }
template <typename I, // I models InputIterator
          typename N, // N models Integral
          typename T>
range<I, N> find(I p, N n, const T& x, enable_if_integral<N> = 0) {
    while (n && *p != x) { ++p; --n; }
    return { p, n };
}
template <typename I, // I models InputIterator
          typename L, // L models sentinal
          typename T>
range<I, L> find(I f, L l, const T& x, disable_if_integral<L> = 0) {
    while (f != l && *f != x) { ++f; }
    return { f, l };
}
template <typename R,
          typename T>
range_t<R> find(R&& r, const T& x) {
    return adobe::find(std::begin(r), std::end(r), x);
}
template <typename I,
          typename N,
          typename O>
std::pair<I, O> copy(I p, N n, O o, enable_if_integral<N> = 0) {
    while (n) { *o++ = *p; ++p; --n; }
    return { p, o };
}
template <typename I,
          typename L,
          typename O>
std::pair<I, O> copy(I f, L l, O o, disable_if_integral<L> = 0) {
    while (f != l) { *o++ = *f; ++f; }
    return { f, o };
}
template <typename R,
          typename O>
std::pair<iterator_t<R>, O> copy(R&& r, O o) {
    return adobe::copy(std::begin(r), std::end(r), o);
}
template <typename CharT = char, typename Traits = std::char_traits<CharT>>
class ostream_iterator {
  public:
    using iterator_category = std::output_iterator_tag;
    using ostream_type = std::basic_ostream<CharT, Traits>;
    ostream_iterator(ostream_type& s) : stream_m(&s), string_m(nullptr) { }
    ostream_iterator(ostream_type& s, const CharT* c) : stream_m(&s), string_m(c) { }
    template <typename T>
    ostream_iterator& operator=(const T& x) {
        *stream_m << x;
        if (string_m) *stream_m << string_m;
        return *this;
    }
  ostream_iterator& operator*() { return *this; }
  ostream_iterator& operator++() { return *this; }
  ostream_iterator& operator++(int) { return *this; }
private:
  ostream_type* stream_m;
  const CharT* string_m;
};
template <class _Tp, 
          class _CharT = char, class _Traits = std::char_traits<_CharT>,
          class _Dist = ptrdiff_t> 
class istream_iterator {
public:
  typedef _CharT                                char_type;
  typedef _Traits                               traits_type;
  typedef std::basic_istream<_CharT, _Traits>   istream_type;
  typedef std::input_iterator_tag                    iterator_category;
  typedef _Tp                                   value_type;
  typedef _Dist                                 difference_type;
  typedef const _Tp*                            pointer;
  typedef const _Tp&                            reference;
  istream_iterator() : _M_stream(0), _M_ok(false) {}
  istream_iterator(istream_type& __s) : _M_stream(&__s) { _M_read(); }
  reference operator*() const { return _M_value; }
  pointer operator->() const { return &(operator*()); }
  istream_iterator& operator++() { 
    _M_read(); 
    return *this;
  }
  istream_iterator operator++(int)  {
    istream_iterator __tmp = *this;
    _M_read();
    return __tmp;
  }
  bool _M_equal(const istream_iterator& __x) const
    { return (_M_ok == __x._M_ok) && (!_M_ok || _M_stream == __x._M_stream); }
private:
  istream_type* _M_stream;
  _Tp _M_value;
  bool _M_ok;
  void _M_read() {
    _M_ok = (_M_stream && *_M_stream) ? true : false;
    if (_M_ok) {
      *_M_stream >> _M_value;
      _M_ok = *_M_stream ? true : false;
    }
  }
};
template <class _Tp, class _CharT, class _Traits, class _Dist>
inline bool 
operator==(const istream_iterator<_Tp, _CharT, _Traits, _Dist>& __x,
           const istream_iterator<_Tp, _CharT, _Traits, _Dist>& __y) {
  return __x._M_equal(__y);
}
template <class _Tp, class _CharT, class _Traits, class _Dist>
inline bool 
operator!=(const istream_iterator<_Tp, _CharT, _Traits, _Dist>& __x,
           const istream_iterator<_Tp, _CharT, _Traits, _Dist>& __y) {
  return !__x._M_equal(__y);
}
/*
Implement an istream range
template <typename T, typename charT>
range<> range(istream<charT>&)
*/
}
int main() {
    int a[] = { 0, 1, 2, 3, 4, 5 };
    adobe::copy(adobe::find(a, 3), adobe::ostream_iterator<>(std::cout, ", "));
}
#endif



#if 0
using namespace std;
size_t count = 0;
struct annotate {
    annotate() { cout << "annotate ctor" << endl; }
    annotate(const annotate&) { cout << "annotate copy-ctor" << endl; }
    annotate(annotate&&) noexcept { cout << "annotate move-ctor" << endl; }
    annotate& operator=(const annotate&) { cout << "annotate assign" << endl; return *this; }
    annotate& operator=(annotate&&) noexcept { cout << "annotate move-assign" << endl; return *this; }
    ~annotate() { /* cout << "annotate dtor" << endl; */ }
    friend inline void swap(annotate&, annotate&) { cout << "annotate swap" << endl; ++::count; }
};
template <typename I,
          typename N>
I reverse_n(I f, N n) {
    if (n == 0) return f;
    if (n == 1) return ++f;
    auto m = reverse_n(f, n / 2);
    auto l = reverse_n(m, n - (n / 2));
    std::rotate(f, m, l);
    return l;
}
template <typename I> // I models ForwardIterator
void reverse(I f, I l) {
    reverse_n(f, distance(f, l));
}
template <typename I> // I models ForwardIterator
void sort(I f, I l) {
    while (f != l) {
    }
}
int main() {
    forward_list<annotate> x(10000);
    ::reverse(begin(x), end(x));
    cout << "count: " << ::count << endl;
}
#endif

#if 0
#include <functional>
#include <iostream>
using namespace std;
struct type {
    bool member;
};
int main() {
    type x = { true };
    cout << !bind(&type::member, x)() << endl;
}
#endif


#if 0
#ifndef DRAWING
#define DRAWING
#include <memory>
#include <iostream>
namespace concepts{
    namespace drawing{
        struct concept_t{
            concept_t() = default;
            concept_t(concept_t const&) = delete;
            concept_t(concept_t&& ) = delete;
            concept_t& operator = (concept_t const&) = delete;
            concept_t& operator = (concept_t&&) = delete;
            virtual void draw() = 0;
        };
        
        template<class T>
        class model_t: public concept_t{
        public:
            model_t(T const& concept) : concept_(concept){
            }
            
            model_t(T&& concept) : concept_(std::move(concept)){
                
            }
            
            model_t(model_t const&) = delete;
            model_t(model_t&& ) = delete;
            model_t& operator = (model_t const&) = delete;
            model_t& operator = (model_t&&) = delete;
            
            void draw(){
                concept_.draw();
            }
        private:
            T concept_;
        };
        
        class drawable{
        public:
            template<class T>
            drawable(T const& concept) {
                auto ptr = new model_t<T>(concept);
                impl_ = std::shared_ptr<concept_t>(ptr);
                
            }
            
            drawable(drawable const&) = default;
            drawable(drawable&& ) = default;
            drawable& operator = (drawable const&) = default;
            drawable& operator = (drawable&&) = default;
            
            void draw(){
                impl_->draw();
            }
        private:
            std::shared_ptr<concept_t> impl_;
        };
    }
}
namespace graphics {
    class graphics_display{
    public:
        template<class T>
        graphics_display(T const& concept) : control_(concept){
            
        }
        
        void draw(){
            control_.draw();
        }
        
    private:
        concepts::drawing::drawable control_;
    };
    
    class rectangle{
        public:
        void draw(){
            std::cout << "Rectangle\n";
        }
    };
    
    class square{
        public:
        void draw(){
            std::cout << "Square\n";
        }
    };
    
    class circle{
        public:
        void draw(){
            std::cout << "Circle\n";
        }
    };
}
#endif
// Main file starts here
// #include "drawing.hpp"
int main(int argc, const char * argv[])
{
    graphics::circle c;
    graphics::graphics_display g(c);
    g.draw();
    return 0;
}
#endif




#if 0
#include <iostream>
#include <algorithm>
using namespace std;
#define SLOW
struct A {
  A() {}
  ~A() { std::cout << "A d'tor\n"; }
  A(const A&) { std::cout << "A copy\n"; }
  A(A&&) { std::cout << "A move\n"; }
  A &operator=(const A&) { std::cout << "A copy assignment\n"; return *this; }
  A &operator=(A&&) { std::cout << "A move assignment\n"; return *this; }
};
struct B {
  // Using move on a sink. 
  // Nice talk at Going Native 2013 by Sean Parent.
  B(A foo) : a_(std::move(foo)) {}  
  A a_;
};
A MakeA() {
  return A();
}
B MakeB() {  
 // The key bits are in here
#ifdef SLOW
  A a(MakeA());
  return B(move(a));
#else
  return B(MakeA());
#endif
}
int main() {
  std::cout << "Hello World!\n";
  B obj = MakeB();
  std::cout << &obj << "\n";
  return 0;
}
#endif


#if 0
using namespace std;
// For illustration only
class group {
  public:
    template <typename F>
    void async(F&& f) {        
        auto then = then_;
        thread(bind([then](F& f){ f(); }, std::forward<F>(f))).detach();
    }
    
    template <typename F>
    void then(F&& f) {
        then_->f_ = forward<F>(f);
        then_.reset();
    }
    
  private:
    struct packaged {
        ~packaged() { thread(bind(move(f_))).detach(); }
        function<void ()> f_;
    };
    
    shared_ptr<packaged> then_ = make_shared<packaged>();
};
#endif

#if 0
namespace adobe {
template <typename F, typename SIG> class continuable_function;
template <typename F, typename R, typename ...Arg>
class continuable_function<F, R (Arg...)> {
  public:
    typedef R result_type;
    template <typename R1>
    continuable_function(F f, R1(R));
    R1 operator()(Arg&& ...arg) const {
    }
};
}  // namespace adobe
int main() {
}
#endif


#if 0
#include <algorithm>
#include <string>
using namespace std;
tuple<I, I, I> name(I f, I l) {
    return { f, find_if(f + 1, l, [](char c){ return isupper(c) || isspace(c) },
             find_if(f, l, [](char c) { return isspace(c));
}
/*
    pair = name {!alpha} name
    name = upper lower {lower} space *upper lower {lower} *[upper lower {lower}]*;
*/
template <typename I>
I isoccpify_name_pair(I f, I l)
{
    auto n = [&]{f += 2; while(islower(*f++)); f += 1; I a = f; f += 2; while(islower(*f++));
                 I b = f; if (isupper(*f)) { f += 2; while(islower(*f++)); } return {a, b, f}};
    auto p = [&]{ auto a = n(); while(!alpha(*f++)); reuturn { a, n();
}
#endif


#if 0
struct move_only {
    move_only();
    move_only(move_only&&) { }
};
int main() {
    move_only x;
    move_only y;
    x = y;
}
#endif



#if 0
#include <iostream>
using namespace std;
struct annotate {
    annotate() { cout << "annotate ctor" << endl; }
    annotate(const annotate&) { cout << "annotate copy-ctor" << endl; }
    annotate(annotate&&) noexcept { cout << "annotate move-ctor" << endl; }
    annotate& operator=(const annotate&) { cout << "annotate assign" << endl; return *this; }
    annotate& operator=(annotate&&) noexcept { cout << "annotate move-assign" << endl; return *this; }
    ~annotate() { cout << "annotate dtor" << endl; }
};
struct object {
    object& operator=(const object&) & = default;
    object& operator=(object&&) & noexcept = default;
};
struct wrap {
    annotate a_;
    object b_;
};
void f(object& x);
int main() {
#if 0
    object x;
    // Assignment to rvalues will generate an error.
    object() = x;
    object() = object();
#endif
    wrap x;
    x = wrap();
    wrap() = x; // this is still allowed
}
#endif

#if 0
#include <iostream>
#include <algorithm>
#include <vector>
#include <numeric>
using namespace std;
struct annotate {
    annotate() { cout << "annotate ctor" << endl; }
    annotate(const annotate&) { cout << "annotate copy-ctor" << endl; }
    annotate(annotate&&) noexcept { cout << "annotate move-ctor" << endl; }
    annotate& operator=(const annotate&) { cout << "annotate assign" << endl; return *this; }
    annotate& operator=(annotate&&) noexcept { cout << "annotate move-assign" << endl; return *this; }
    ~annotate() { cout << "annotate dtor" << endl; }
};
struct wrap {
    ~wrap() noexcept(false) { throw 0; }
};
struct wrap2 {
    wrap m1_;
    annotate m2_;
};
#define ADOBE_NOEXCEPT noexcept
class example {
    example(string x, string y) : str1_(move(x)), str2_(move(y)) { }
    /* ... */
    // copy - if memberwise copy is not sufficient - provide custom copy
    example(const example&); // custom copy
    
    // move ctor equivalent to = default
    example(example&& x) ADOBE_NOEXCEPT : member_(move(x.member_)) { }
    
    // copy assignment is copy and move assign
    example& operator=(const example& x) { example tmp = x; *this = move(x); return *this; }
    
    // move assignment equivalent to = default
    example& operator=(example&& x) ADOBE_NOEXCEPT { member_ = move(x.member_); return *this; }
    
    /* ... */
    
 private:
    string str1_, str2_;
    int member_;
};
string append_file_suffix(string x)
{
    x += ".jpg";
    return x;
}
vector<int> deck()
{
    vector<int> x(52);
    iota(begin(x), end(x), 0);
    return x;
}
vector<int> shuffle(vector<int> x)
{
    random_shuffle(begin(x), end(x));
    return x;
}
auto multiply(int a, int b) -> int
{ return a * b; }
template <typename T>
auto multiply(T a, T b) -> decltype(a * b)
{ return a * b; }
declval<
vector<string> file_names()
{
    vector<string> x;
    x.push_back(append_file_suffix("best_image"));
    
    //...
    
    return x;
}
enum class color : int32_t { red, green, blue };
color operator+(color x, color y)
{
    return static_cast<color>(static_cast<underlying_type<color>::type>(x)
        + static_cast<underlying_type<color>::type>(y) % 3);
}
int main() {
    vector<string> file_names;
    
    //...
    
    file_names.push_back(append_file_suffix("best_image"));
    
    color x = color::red;
    cout << (x == color::red) << endl;
    
    x = x + color::blue;
    cout << static_cast<underlying_type<color>::type>(x) << endl;
#if 0
    vector<int> a = shuffle(deck());
    /* ... */
    
    for (const auto& e : a) cout << e << endl;
    
   // auto p = begin(a);
    
    auto& last = a.back();
    last += 10;
    
    
    for (auto& e : a) e += 3;
#endif
}
#endif


#if 0
#include <memory>
using namespace std;
template <typename C> // Concept base class
class any {
  public:
    template <typename T>
    any(T x) : self_(new model<T>(move(x))) { }
    
    template <typename T, typename F>
    any(unique_ptr<T>&& x, F cpy) : self_(new model_unique<T, F>(move(x), move(cpy))) { }
    
    any(const any& x) : self_(x.self_->copy()) { }
    any(any&&) noexcept = default;
    
    any& operator=(const any& x) { any tmp = x; *this = move(tmp); return *this; }
    any& operator=(any&&) noexcept = default;
    
    C* operator->() const { return &self_->dereference(); }
    C& operator*() const { return self_->dereference(); }
    
    template <typename T>
    any(any<T>& x) : self_(new model<T>) { }
  private:
    struct regular {
        virtual regular* copy() const = 0;
        virtual C& dereference() = 0;
    };
    
    template <typename T>
    struct model : regular {
        model(T x) : data_(move(x)) { }
        regular* copy() const { return new model(*this); }
        C& dereference() { return data_; }
        
        T data_;
    };
    
    template <typename T, typename F>
    struct model_unique : regular {
        model_unique(unique_ptr<T>&& x, F cpy) : data_(move(x)), copy_(move(cpy)) { }
        regular* copy() const { return new model_unique(copy_(*data_), copy_); }
        C& dereference() { return *data_; }
        
        unique_ptr<T> data_;
        F copy_;
    };
    
    unique_ptr<regular> self_;
};
/**************************************************************************************************/
#include <iostream>
#include <string>
#include <vector>
using namespace std;
class shape {
  public:
    virtual void draw(ostream& out, size_t position) const = 0;
};
class polygon : public shape {
};
class square : public polygon {
    void draw(ostream& out, size_t position) const
    { out << string(position, ' ') << "square" << endl; }
};
class circle : public shape {
    void draw(ostream& out, size_t position) const
    { out << string(position, ' ') << "circle" << endl; }
};
int main() {
    thread_local int local = 5;
    any<shape> x = { unique_ptr<square>(new square()),
        [](const square& x) { return unique_ptr<square>(new square()); } };
    vector<any<shape>> a = { square(), circle(), x };
    for (const auto& e : a) e->draw(cout, 0);
}
#endif


#if 0
/*
    Copyright 2013 Adobe Systems Incorporated
    Distributed under the MIT License (see license at
    http://stlab.adobe.com/licenses.html)
    
    This file is intended as example code and is not production quality.
*/
#include <cassert>
#include <chrono>
#include <thread>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;
/******************************************************************************/
// Library
template <typename T>
void draw(const T& x, ostream& out, size_t position)
{ out << string(position, ' ') << x << endl; }
class object_t {
  public:
    template <typename T>
    object_t(T x) : self_(make_shared<model<T>>(move(x)))
    { }
    
    friend void draw(const object_t& x, ostream& out, size_t position)
    { x.self_->draw_(out, position); }
    
  private:
    struct concept_t {
        virtual ~concept_t() = default;
        virtual void draw_(ostream&, size_t) const = 0;
    };
    template <typename T>
    struct model : concept_t {
        model(T x) : data_(move(x)) { }
        void draw_(ostream& out, size_t position) const 
        { draw(data_, out, position); }
        
        T data_;
    };
    
   shared_ptr<const concept_t> self_;
};
using document_t = vector<object_t>;
void draw(const document_t& x, ostream& out, size_t position)
{
    out << string(position, ' ') << "<document>" << endl;
    for (const auto& e : x) draw(e, out, position + 2);
    out << string(position, ' ') << "</document>" << endl;
}
using history_t = vector<document_t>;
void commit(history_t& x) { assert(x.size()); x.push_back(x.back()); }
void undo(history_t& x) { assert(x.size()); x.pop_back(); }
document_t& current(history_t& x) { assert(x.size()); return x.back(); }
/******************************************************************************/
// Client
class my_class_t {
    /* ... */
};
void draw(const my_class_t&, ostream& out, size_t position)
{ out << string(position, ' ') << "my_class_t" << endl; }
int main()
{
    history_t h(1);
    current(h).emplace_back(0);
    current(h).emplace_back(string("Hello!"));
    
    draw(current(h), cout, 0);
    cout << "--------------------------" << endl;
    
    commit(h);
    
    current(h)[0] = 42.5;
    
    auto document = current(h);
    
    document.emplace_back(make_shared<my_class_t>());
    
    auto saving = async([=]() {
        this_thread::sleep_for(chrono::seconds(3));
        cout << "--------- 'save' ---------" << endl;
        draw(document, cout, 0);
    });
    current(h)[1] = string("World");
    
    current(h).emplace_back(current(h));
    current(h).emplace_back(my_class_t());
    
    draw(current(h), cout, 0);
    cout << "--------------------------" << endl;
    
    undo(h);
    
    draw(current(h), cout, 0);
}
#endif



#if 0
#include <string>
#include <algorithm>
#include <iostream>
#include <iomanip>
using namespace std;
struct person {
    string first_;
    string last_;
    string state_;
};
template <typename T>
void print(const T& a) {
    for (const auto& e: a) cout
        << setw(10) << e.first_
        << setw(10) << e.last_
        << setw(10) << e.state_ << endl;
}
int main() {
    person a[] = {
        { "John", "Smith", "New York" },
        { "John", "Doe", "Ohio" },
        { "John", "Stone", "Alaska" },
        { "Micheal", "Smith", "New York" },
        { "Micheal", "Doe", "Georgia" },
        { "Micheal", "Stone", "Alaska" }
    };
    
    random_shuffle(begin(a), end(a));
    
    cout << setw(10) << "- first -" << setw(10) << "- last -" << setw(10) << "- state -" << endl;
    print(a);
    
    cout << setw(10) << "- first -" << setw(10) << "- last -" << setw(10) << "* state *" << endl;
    stable_sort(begin(a), end(a), [](const person& x, const person& y){ return x.state_ < y.state_; });
    print(a);
    
    cout << setw(10) << "* first *" << setw(10) << "- last -" << setw(10) << "- state -" << endl;
    stable_sort(begin(a), end(a), [](const person& x, const person& y){ return x.first_ < y.first_; });
    print(a);
    
    cout << setw(10) << "- first -" << setw(10) << "* last *" << setw(10) << "- state -" << endl;
    stable_sort(begin(a), end(a), [](const person& x, const person& y){ return x.last_ < y.last_; });
    print(a);
    
    cout << setw(10) << "* first *" << setw(10) << "- last -" << setw(10) << "- state -" << endl;
    stable_sort(begin(a), end(a), [](const person& x, const person& y){ return x.first_ < y.first_; });
    print(a);
}
#endif



#if 0
#include <iostream>
using namespace std;
struct annotate {
    annotate() { cout << "annotate ctor" << endl; }
    annotate(const annotate&) { cout << "annotate copy-ctor" << endl; }
    annotate(annotate&&) noexcept { cout << "annotate move-ctor" << endl; }
    annotate& operator=(const annotate&) { cout << "annotate assign" << endl; return *this; }
    annotate& operator=(annotate&&) noexcept { cout << "annotate move-assign" << endl; return *this; }
    ~annotate() { cout << "annotate dtor" << endl; }
};
struct annotate_value_assign {
    annotate_value_assign() { cout << "annotate_value_assign ctor" << endl; }
    annotate_value_assign(const annotate_value_assign&) { cout << "annotate_value_assign copy-ctor" << endl; }
    annotate_value_assign(annotate_value_assign&&) noexcept { cout << "annotate_value_assign move-ctor" << endl; }
    annotate_value_assign& operator=(annotate_value_assign x) noexcept { cout << "annotate_value_assign value-assign" << endl; return *this; }
    ~annotate_value_assign() { cout << "annotate_value_assign dtor" << endl; }
};
struct wrap1 {
    annotate m_;
};
struct wrap2 {
    annotate_value_assign m_;
};
int main() {
    {
    wrap1 x, y;
    cout << "<move wrap1>" << endl;
    x = move(y);
    cout << "</move wrap1>" << endl;
    }
    {
    wrap2 x, y;
    cout << "<move wrap2>" << endl;
    x = move(y);
    cout << "</move wrap2>" << endl;
    }
    
    cout << boolalpha;
    cout << "wrap1 nothrow move assignable trait:" << is_nothrow_move_assignable<wrap1>::value << endl;
    cout << "wrap2 nothrow move assignable trait:" << is_nothrow_move_assignable<wrap2>::value << endl;
}
#endif



#if 0
template <typename T>
void swap(T& x, T& y)
{
    T t = x;
    x = y;
    y = t;
}
template <typename I>
void reverse(I f, I l)
{
    while (f != l) {
        --l;
        if (f == l) break;
        swap(*f, *l);
        ++f;
    }
}
template <typename I>
auto rotate(I f, I m, I l) -> I
{
    if (f == m) return l;
    if (m == l) return f;
    reverse(f, m);
    reverse(m, l);
    reverse(f, l);
    
    return f + (l - m);
}
template <typename I,
          typename P>
auto stable_partition(I f, I l, P p) -> I
{
    auto n = l - f;
    if (n == 0) return f;
    if (n == 1) return f + p(*f);
    
    auto m = f + (n / 2);
    return rotate(stable_partition(f, m, p),
                  m,
                  stable_partition(m, l, p));
}
#include <iostream>
#include <memory>
#include <vector>
using namespace std;
struct annotate {
    annotate() { cout << "annotate ctor" << endl; }
    annotate(const annotate&) { cout << "annotate copy-ctor" << endl; }
    annotate(annotate&&) noexcept { cout << "annotate move-ctor" << endl; }
    annotate& operator=(const annotate& x) { m_ = x.m_; cout << "annotate assign" << endl; return *this; }
    annotate& operator=(annotate&& x) noexcept { x.m_ = 13; cout << "annotate move-assign" << endl; return *this; }
    ~annotate() { m_ = 255; cout << "annotate dtor" << endl; }
    
    int m_ = 42;
};
int main()
{
    // int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    
    std::vector<annotate> x;
    x.push_back(annotate());
    
    cout << "----" << endl;
    
    x.reserve(2);
    
    cout << "----" << endl;
    
    x.push_back(x.back());
    cout << "----" << endl;
    x.push_back(x.back());
    cout << "----" << endl;
    for (const auto& e : x) std::cout << e.m_ << "\n";
    
    #if 0
    stable_partition(&a[0], &a[10], [](const int& x) -> bool { return x & 1; });
    for (const auto& e : a) std::cout << e << "\n";
    
    std::cout << sizeof(std::shared_ptr<int>) << std::endl;
    #endif
}
#endif


#if 0
#include <iostream>
#include <memory>
#include <future>
#include <vector>
#include <dispatch/dispatch.h>
using namespace std;
// For illustration only
class group {
  public:
    template <typename F>
    void async(F&& f) {        
        auto then = then_;
        thread(bind([then](F& f){ f(); }, std::forward<F>(f))).detach();
    }
    
    template <typename F>
    void then(F&& f) {
        then_->f_ = forward<F>(f);
        then_.reset();
    }
    
  private:
    struct packaged {
        ~packaged() { thread(bind(move(f_))).detach(); }
        function<void ()> f_;
    };
    
    shared_ptr<packaged> then_ = make_shared<packaged>();
};
int main()
{
    group g;
    g.async([]() { 
        this_thread::sleep_for(chrono::seconds(2));
        cout << "task 1" << endl;
    });
    
    g.async([]() { 
        this_thread::sleep_for(chrono::seconds(1));
        cout << "task 2" << endl;
    });
    
    g.then([=](){
        cout << "done!" << endl;
    });
    
    this_thread::sleep_for(chrono::seconds(10));
}
#endif



#if 0
#include <iostream>
#include <memory>
#include <future>
#include <vector>
#include <dispatch/dispatch.h>
using namespace std;
namespace adobe {
template <typename F, typename ...Args>
auto async(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F (Args...)>::type>
{
    using result_type = typename std::result_of<F (Args...)>::type;
    using packaged_type = std::packaged_task<result_type ()>;
    
    auto p = new packaged_type(std::forward<F>(f), std::forward<Args>(args)...);
    auto result = p->get_future();
    dispatch_async_f(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
            p, [](void* f_) {
                packaged_type* f = static_cast<packaged_type*>(f_);
                (*f)();
                delete f;
            });
    
    return result;
}
} // namespace adobe
// For illustration only
class group {
  public:
    template <typename F, typename ...Args>
    auto async(F&& f, Args&&... args)
        -> future<typename result_of<F (Args...)>::type>
    {
        using result_type = typename std::result_of<F (Args...)>::type;
        using packaged_type = std::packaged_task<result_type ()>;
        
        auto p = packaged_type(forward<F>(f), forward<Args>(args)...);
        auto result = p.get_future();
        
        auto then = then_;
        thread(bind([then](packaged_type& p){ p(); }, move(p))).detach();
        
        return result;
    }
    
    template <typename F, typename ...Args>
    auto then(F&& f, Args&&... args)
        -> future<typename result_of<F (Args...)>::type>
    {
        using result_type = typename std::result_of<F (Args...)>::type;
        using packaged_type = std::packaged_task<result_type ()>;
        
        auto p = packaged_type(forward<F>(f), forward<Args>(args)...);
        auto result = p.get_future();
        
        then_->reset(new packaged<packaged_type>(move(p)));
        then_ = nullptr;
        
        return result;
    }
    
  private:
    struct any_packaged {
        virtual ~any_packaged() = default;
    };
    
    template <typename P>
    struct packaged : any_packaged {
        packaged(P&& f) : f_(move(f)) { }
        ~packaged() { thread(bind(move(f_))).detach(); }
        
        P f_;
    };
    
    shared_ptr<unique_ptr<any_packaged>> then_ = make_shared<unique_ptr<any_packaged>>();
};
int main()
{
    group g;
    
    auto x = g.async([]() { 
        this_thread::sleep_for(chrono::seconds(2));
        cout << "task 1" << endl;
        return 10;
    });
    
    auto y = g.async([]() { 
        this_thread::sleep_for(chrono::seconds(1));
        cout << "task 2" << endl;
        return 5;
    });
    
    auto r = g.then(bind([](future<int>& x, future<int>& y) {
        cout << "done:" << (x.get() + y.get()) << endl;
    }, move(x), move(y)));
    
    r.get();
}
#endif


#if 0
/*
    Copyright 2013 Adobe Systems Incorporated
    Distributed under the MIT License (see license at
    http://stlab.adobe.com/licenses.html)
    
    This file is intended as example code and is not production quality.
*/
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <future>
#include <dispatch/dispatch.h>
using namespace std;
/******************************************************************************/
// Library
template <typename T>
void draw(const T& x, ostream& out, size_t position)
{ out << string(position, ' ') << x << endl; }
class object_t {
  public:
    template <typename T>
    object_t(T x) : self_(make_shared<model<T>>(move(x))) { }
        
    friend void draw(const object_t& x, ostream& out, size_t position)
    { x.self_->draw_(out, position); }
    
  private:
    struct concept_t {
        virtual ~concept_t() = default;
        virtual void draw_(ostream&, size_t) const = 0;
    };
    template <typename T>
    struct model : concept_t {
        model(T x) : data_(move(x)) { }
        void draw_(ostream& out, size_t position) const
        { draw(data_, out, position); }
        
        T data_;
    };
    
   shared_ptr<const concept_t> self_;
};
using document_t = vector<object_t>;
void draw(const document_t& x, ostream& out, size_t position)
{
    out << string(position, ' ') << "<document>" << endl;
    for (auto& e : x) draw(e, out, position + 2);
    out << string(position, ' ') << "</document>" << endl;
}
/******************************************************************************/
// Client
class my_class_t {
    /* ... */
};
void draw(const my_class_t&, ostream& out, size_t position)
{ out << string(position, ' ') << "my_class_t" << endl; }
int main()
{
    document_t document;
    
    document.emplace_back(my_class_t());
    document.emplace_back(string("Hello World!"));
    
    auto saving = async([=]() {
        this_thread::sleep_for(chrono::seconds(3));
        cout << "-- 'save' --" << endl;
        draw(document, cout, 0);
    });
    
    document.emplace_back(document);
    
    draw(document, cout, 0);
    saving.get();
}
#endif


/**************************************************************************************************/


//
//  main.cpp
//  scratch
//
//  Created by Sean Parent on 3/18/13.
//  Copyright (c) 2013 stlab. All rights reserved.

/**************************************************************************************************/

#if 0
#include <iostream>
#include <memory>
using namespace std;
struct annotate {
    annotate() { cout << "annotate ctor" << endl; }
    annotate(const annotate&) { cout << "annotate copy-ctor" << endl; }
    annotate(annotate&&) noexcept { cout << "annotate move-ctor" << endl; }
    annotate& operator=(const annotate&) { cout << "annotate assign" << endl; return *this; }
    annotate& operator=(annotate&&) noexcept { cout << "annotate move-assign" << endl; return *this; }
    ~annotate() { cout << "annotate dtor" << endl; }
};
int main() {
auto x = unique_ptr<int>(new int(5));
x = move(x);
cout << *x << endl;
}
#endif

/**************************************************************************************************/


#if 0
#include <list>
#include <future>
#include <iostream>
#include <boost/range/algorithm/find.hpp>
#include <cstddef>
#include <dispatch/dispatch.h>
#include <adobe/algorithm.hpp>
#include <list>
//using namespace std;
using namespace adobe;
using namespace std;
namespace adobe {
template <typename F, typename ...Args>
auto async(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F (Args...)>::type>
{
    using result_type = typename std::result_of<F (Args...)>::type;
    using packaged_type = std::packaged_task<result_type ()>;
    
    auto p = new packaged_type(std::forward<F>(f), std::forward<Args>(args)...);
    auto result = p->get_future();
    dispatch_async_f(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
            p, [](void* f_) {
                packaged_type* f = static_cast<packaged_type*>(f_);
                (*f)();
                delete f;
            });
    
    return result;
}
} // namespace adobe
template <typename T>
class concurrent_queue
{
    mutex   mutex_;
    list<T> q_;
  public:
    void enqueue(T x)
    {
        list<T> tmp;
        tmp.push_back(move(x));
        {
        lock_guard<mutex> lock(mutex);
        q_.splice(end(q_), tmp);
        }
    }
    
    // ...
};
class join_task {
};
struct last_name {
    using result_type = const string&;
    
    template <typename T>
    const string& operator()(const T& x) const { return x.last; }
};
int main() {
#if 0
auto a = { 0, 1, 2, 3, 4, 5 };
auto x = 3;
{
auto p = find(begin(a), end(a), x);
cout << *p << endl;
}
{
auto p = find(a, x);
cout << *p << endl;
}
#endif
struct employee {
    string first;
    string last;
};
#if 0
int f(int x);
int g(int x);
int r[] = { 0, 1, 2, 3, 4, 5 };
for (const auto& e: r) f(g(e));
for (const auto& e: r) { f(e); g(e); };
for (auto& e: r) e = f(e) + g(e);
#endif
employee a[] = { { "Sean", "Parent" }, { "Jaakko", "Jarvi" }, { "Bjarne", "Stroustrup" } };
{
sort(a, [](const employee& x, const employee& y){ return x.last < y.last; });
auto p = lower_bound(a, "Parent",
    [](const employee& x, const string& y){ return x.last < y; });
    
cout << p->first << endl;
}
{
sort(a, less(), &employee::last);
auto p = lower_bound(a, "Parent", less(), &employee::last);
cout << p->first << endl;
}
{
sort(a, less(), &employee::last);
// ...
auto p = lower_bound(a, "Parent", less(), last_name());
cout << p->first << endl;
}
auto get_name = []{ return string("parent"); };
string s = get_name();
//auto p = find_if(a, [&](const string& e){ return e == "Sean"; });
}
#endif

#if 0
#include <iostream>
#include <vector>
using namespace std;
class object_t {
  public:
    template <typename T> // T models Drawable
    object_t(T x) : self_(make_shared<model<T>>(move(x)))
    { }
    
    void draw(ostream& out, size_t position) const
    { self_->draw_(out, position); }
    
  private:
    struct concept_t {
        virtual ~concept_t() = default;
        virtual void draw_(ostream&, size_t) const = 0;
    };
    
    template <typename T>
    struct model : concept_t {
        model(T x) : data_(move(x)) { }
        void draw_(ostream& out, size_t position) const 
        { data_.draw(out, position); }
        
        T data_;
    };
    
    shared_ptr<const concept_t> self_;
};
using document_t = vector<object_t>;
void draw(const document_t& x, ostream& out, size_t position)
{
    out << string(position, ' ') << "<document>" << endl;
    for (const auto& e : x) e.draw(out, position + 2);
    out << string(position, ' ') << "</document>" << endl;
}
class my_class_t
{
  public:
    void draw(ostream& out, size_t position) const
    { out << string(position, ' ') << "my_class_t" << endl; }
    
};
int main()
{
    document_t document;
    
    document.emplace_back(my_class_t());
    
    draw(document, cout, 0);
}
#endif

#if 0
#include <algorithm>
#include <iostream>
using namespace std;
int main()
{
    double a[] = { 2.5, 3.6, 4.7, 5.8, 6.3 };
    
    auto p = lower_bound(begin(a), end(a), 5, [](double e, int x) {  return e < x; });
    
    cout << *p << endl;
}
#endif

#if 0
#include <list>
#include <future>
#include <iostream>
#include <dispatch/dispatch.h>
using namespace std;
namespace adobe {
template <typename F, typename ...Args>
auto async(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F (Args...)>::type>
{
    using result_type = typename std::result_of<F (Args...)>::type;
    using packaged_type = std::packaged_task<result_type ()>;
    
    auto p = new packaged_type(std::forward<F>(f), std::forward<Args>(args)...);
    auto result = p->get_future();
    dispatch_async_f(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
            p, [](void* f_) {
                packaged_type* f = static_cast<packaged_type*>(f_);
                (*f)();
                delete f;
            });
    
    return result;
}
} // namespace adobe
template <typename T>
class concurrent_queue
{
  public:
    using queue_type = list<T>;
    
    bool enqueue(T x)
    {
        bool was_dry;
        
        queue_type tmp;
        tmp.push_back(move(x));
    
        {
        lock_guard<mutex> lock(mutex);
        was_dry = was_dry_; was_dry_ = false;
        q_.splice(end(q_), tmp);
        }
        return was_dry;
    }
    
    queue_type deque()
    {
        queue_type result;
        
        {
        lock_guard<mutex> lock(mutex);
        if (q_.empty()) was_dry_ = true;
        else result.splice(end(result), q_, begin(q_));
        }
        
        return result;
    }
  private:
    mutex       mutex_;
    queue_type  q_;
    bool        was_dry_ = true;
};
class serial_task_queue
{
  public:
    template <typename F, typename... Args>
    auto async(F&& f, Args&&... args) -> future<typename result_of<F (Args...)>::type>
    {
        using result_type = typename result_of<F (Args...)>::type;
        using packaged_type = packaged_task<result_type ()>;
        
        auto p = make_shared<packaged_type>(bind(forward<F>(f), forward<Args>(args)...));
        auto result = p->get_future();
        
        if (shared_->enqueue([=](){
            (*p)();
            queue_type tmp = shared_->deque();
            if (!tmp.empty()) adobe::async(move(tmp.front()));
        })) adobe::async(move(shared_->deque().front()));
        
        return result;
    }
  private:
    using shared_type = concurrent_queue<function<void ()>>;
    using queue_type = shared_type::queue_type;
      
    shared_ptr<shared_type> shared_ = make_shared<shared_type>();
};
int main()
{
    serial_task_queue queue;
    
    cout << "begin async" << endl;
    future<int> x = queue.async([]{ cout << "task 1" << endl; sleep(3); cout << "end 1" << endl; return 42; });
    
    int y = 10;
    queue.async([](int x) { cout << "x = " << x << endl; }, move(y));
    
    #if 1
    queue.async(bind([](future<int>& x) {
        cout << "task 2: " << x.get() << endl; sleep(2); cout << "end 2" << endl;
    }, move(x)));
    
    #else
    
    #if 0
    queue.async([](future<int>&& x) {
        cout << "task 2: " << x.get() << endl; sleep(2); cout << "end 2" << endl;
    }, move(x));
    #endif
    #endif
    queue.async([]{ cout << "task 3" << endl; sleep(1); cout << "end 3" << endl; });
    cout << "end async" << endl;
    sleep(10);
}
#endif

#if 0
#include <algorithm>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>
/*
With C++11 rotate now returns the new mid point.
*/
using namespace std;
template <typename I> // I models RandomAccessIterator
void slide_0(I f, I l, I p)
{
    if (p < f) rotate(p, f, l);
    if (l < p) rotate(f, l, p);
    // else do nothing
}
template <typename I> // I models RandomAccessIterator
auto slide(I f, I l, I p) -> pair<I, I>
{
    if (p < f) return { p, rotate(p, f, l) };
    if (l < p) return { rotate(f, l, p), p };
    return { f, l };
}
#if 0
template <typename I, // I models BidirectionalIterator
          typename S> // S models UnaryPredicate
auto gather(I f, I l, I p, S s) -> pair<I, I>
{
    using value_type = typename iterator_traits<I>::value_type;
    
    return { stable_partition(f, p, [&](const value_type& x){ return !s(x); }),
             stable_partition(p, l, s) };
}
#endif
namespace adobe {
template <typename P>
class unary_negate
{
  public:
    using result_type = bool;
    
    unary_negate(P p) : p_(move(p)) { }
    
    template <typename T>
    bool operator()(const T& x) const { return !p_(x); }
  private:
    P p_;
};
template <typename P>
auto not1(P p) -> unary_negate<P> { return unary_negate<P>(move(p)); }
} // namespace adobe
template <typename I, // I models BidirectionalIterator
          typename S> // S models UnaryPredicate
auto gather(I f, I l, I p, S s) -> pair<I, I>
{
    return { stable_partition(f, p, adobe::not1(s)),
             stable_partition(p, l, s) };
}
struct is_odd
{
    typedef bool result_type;
    typedef int  argument_type;
    template <typename T>
    bool operator()(const T& x) const { return x & 1; }
};
struct Panel {
    int cur_panel_center();
    int cur_panel_left();
    int cur_right();
    int width();
    template <typename T, typename U>
    int Move(T, U);
    int panel_width();
};
#define CHECK(x)
#define CHECK_LT(x, y)
#define CHECK_NE(x, y)
#define CHECK_GE(x, y)
const int kBarPadding = 0;
const int kAnimMs = 0;
struct PanelBar {
    void RepositionExpandedPanels(Panel* fixed_panel);
    template <typename T, typename U> int GetPanelIndex(T, U);
    
    vector<shared_ptr<Panel>> expanded_panels_;
    Panel* wm_;
};
using Panels = vector<shared_ptr<Panel>>;
template <typename T> using ref_ptr = shared_ptr<T>;
void PanelBar::RepositionExpandedPanels(Panel* fixed_panel) {
  CHECK(fixed_panel);
  // First, find the index of the fixed panel.
  int fixed_index = GetPanelIndex(expanded_panels_, *fixed_panel);
  CHECK_LT(fixed_index, expanded_panels_.size());
  
  {
  const int center_x = fixed_panel->cur_panel_center();
  
#if 0
  auto p = find_if(begin(expanded_panels_), end(expanded_panels_),
    [&](const ref_ptr<Panel>& e){ return center_x <= e->cur_panel_center(); });
#endif
  auto f = begin(expanded_panels_) + fixed_index;
  
  auto p = lower_bound(begin(expanded_panels_), f, center_x,
    [](const ref_ptr<Panel>& e, int x){ return e->cur_panel_center() < x; });
      
  rotate(p, f, f + 1);
  }
#if 0
  // Next, check if the panel has moved to the other side of another panel.
  const int center_x = fixed_panel->cur_panel_center();
  for (size_t i = 0; i < expanded_panels_.size(); ++i) {
    Panel* panel = expanded_panels_[i].get();
    if (center_x <= panel->cur_panel_center() ||
        i == expanded_panels_.size() - 1) {
      if (panel != fixed_panel) {
        // If it has, then we reorder the panels.
        ref_ptr<Panel> ref = expanded_panels_[fixed_index];
        expanded_panels_.erase(expanded_panels_.begin() + fixed_index);
        if (i < expanded_panels_.size()) {
          expanded_panels_.insert(expanded_panels_.begin() + i, ref);
        } else {
          expanded_panels_.push_back(ref);
        }
      }
      break;
    }
  }
#endif
#if 0
  // Next, check if the panel has moved to the other side of another panel.
  const int center_x = fixed_panel->cur_panel_center();
  for (size_t i = 0; i < expanded_panels_.size(); ++i) {
    Panel* panel = expanded_panels_[i].get();
    if (center_x <= panel->cur_panel_center() || i == expanded_panels_.size() - 1) {
      if (panel != fixed_panel) {
        // If it has, then we reorder the panels.
        ref_ptr<Panel> ref = expanded_panels_[fixed_index];
        expanded_panels_.erase(expanded_panels_.begin() + fixed_index);
        expanded_panels_.insert(expanded_panels_.begin() + i, ref);
      }
      break;
    }
  }
  
#endif
    
  // Find the total width of the panels to the left of the fixed panel.
  int total_width = 0;
  fixed_index = -1;
  for (int i = 0; i < static_cast<int>(expanded_panels_.size()); ++i) {
    Panel* panel = expanded_panels_[i].get();
    if (panel == fixed_panel) {
      fixed_index = i;
      break;
    }
    total_width += panel->panel_width();
  }
  CHECK_NE(fixed_index, -1);
  int new_fixed_index = fixed_index;
  // Move panels over to the right of the fixed panel until all of the ones
  // on the left will fit.
  int avail_width = max(fixed_panel->cur_panel_left() - kBarPadding, 0);
  while (total_width > avail_width) {
    new_fixed_index--;
    CHECK_GE(new_fixed_index, 0);
    total_width -= expanded_panels_[new_fixed_index]->panel_width();
  }
  // Reorder the fixed panel if its index changed.
  if (new_fixed_index != fixed_index) {
    Panels::iterator it = expanded_panels_.begin() + fixed_index;
    ref_ptr<Panel> ref = *it;
    expanded_panels_.erase(it);
    expanded_panels_.insert(expanded_panels_.begin() + new_fixed_index, ref);
    fixed_index = new_fixed_index;
  }
  // Now find the width of the panels to the right, and move them to the
  // left as needed.
  total_width = 0;
  for (Panels::iterator it = expanded_panels_.begin() + fixed_index + 1;
       it != expanded_panels_.end(); ++it) {
    total_width += (*it)->panel_width();
  }
  avail_width = max(wm_->width() - (fixed_panel->cur_right() + kBarPadding),
                    0);
  while (total_width > avail_width) {
    new_fixed_index++;
    CHECK_LT(new_fixed_index, expanded_panels_.size());
    total_width -= expanded_panels_[new_fixed_index]->panel_width();
  }
  // Do the reordering again.
  if (new_fixed_index != fixed_index) {
    Panels::iterator it = expanded_panels_.begin() + fixed_index;
    ref_ptr<Panel> ref = *it;
    expanded_panels_.erase(it);
    expanded_panels_.insert(expanded_panels_.begin() + new_fixed_index, ref);
    fixed_index = new_fixed_index;
  }
  // Finally, push panels to the left and the right so they don't overlap.
  int boundary = expanded_panels_[fixed_index]->cur_panel_left() - kBarPadding;
  for (Panels::reverse_iterator it =
         // Start at the panel to the left of 'new_fixed_index'.
         expanded_panels_.rbegin() +
         (expanded_panels_.size() - new_fixed_index);
       it != expanded_panels_.rend(); ++it) {
    Panel* panel = it->get();
    if (panel->cur_right() > boundary) {
      panel->Move(boundary, kAnimMs);
    } else if (panel->cur_panel_left() < 0) {
      panel->Move(min(boundary, panel->panel_width() + kBarPadding), kAnimMs);
    }
    boundary = panel->cur_panel_left() - kBarPadding;
  }
  boundary = expanded_panels_[fixed_index]->cur_right() + kBarPadding;
  for (Panels::iterator it = expanded_panels_.begin() + new_fixed_index + 1;
       it != expanded_panels_.end(); ++it) {
    Panel* panel = it->get();
    if (panel->cur_panel_left() < boundary) {
      panel->Move(boundary + panel->panel_width(), kAnimMs);
    } else if (panel->cur_right() > wm_->width()) {
      panel->Move(max(boundary + panel->panel_width(),
                      wm_->width() - kBarPadding),
                  kAnimMs);
    }
    boundary = panel->cur_right() + kBarPadding;
  }
}
int main()
{
    int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto r = slide(&a[5], &a[8], &a[6]);
    copy(begin(a), end(a), ostream_iterator<int>(cout, ", "));
    auto r2 = gather(begin(a), end(a), &a[5], is_odd());
    copy(begin(a), end(a), ostream_iterator<int>(cout, ", "));
    copy(r2.first, r2.second, ostream_iterator<int>(cout, ", "));
    
}
#endif

/**************************************************************************************************/

#if 0
#include <functional>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>
#include <numeric>
#include <deque>
using namespace std;
template <typename I, typename N>
void advance_slow(I& i, N n)
{
    while (n != 0) { --n; ++i; };
}
template <typename I>
size_t distance_slow(I f, I l)
{
    size_t n = 0;
    while (f != l) { ++f; ++n; }
    return n;
}
template <typename I, typename T>
I lower_bound_slow(I f, I l, T x)
{
    size_t n = distance_slow(f, l);
    while (n != 0) {
        size_t h = n / 2;
        I m = f;
        advance_slow(m, h);
        if (*m < x) { f = m; ++f; n -= h + 1; }
        else n = h;
    }
    return f;
}
template <typename I, typename T>
I lower_bound_fast(I f, I l, T x)
{
    size_t n = l - f;
    while (n != 0) {
        size_t h = n / 2;
        I m = f;
        m += h;
        if (*m < x) { f = m; ++f; n -= h + 1; }
        else n = h;
    }
    return f;
}
using container = vector<int>; // Change this to a deque to see a huge difference
long long __attribute__ ((noinline)) time_slow(vector<int>& r, const container& a, const container& b)
{
    auto start = chrono::high_resolution_clock::now();
    
    for (const auto& e : b) r.push_back(*lower_bound_slow(begin(a), end(a), e));
    
    return chrono::duration_cast<chrono::microseconds>
        (chrono::high_resolution_clock::now()-start).count();
}
long long __attribute__ ((noinline)) time_fast(vector<int>& r, const container& a, const container& b)
{
    auto start = chrono::high_resolution_clock::now();
    
    for (const auto& e : b) r.push_back(*lower_bound_fast(begin(a), end(a), e));
    
    return chrono::duration_cast<chrono::microseconds>
        (chrono::high_resolution_clock::now()-start).count();
}
int main() {
    container a(10000);
    iota(begin(a), end(a), 0);
    
    container b = a;
    random_shuffle(begin(b), end(b));
    
    vector<int> r;
    r.reserve(b.size() * 2);
    
    cout << "fast: " << time_fast(r, a, b) << endl;
    cout << "slow: " << time_slow(r, a, b) << endl;
}
#endif

#if 0
#include <string>
#include <iostream>
#include <memory>
using namespace std;
struct annotate {
    annotate() { cout << "annotate ctor" << endl; }
    annotate(const annotate&) { cout << "annotate copy-ctor" << endl; }
    annotate(annotate&&) noexcept { cout << "annotate move-ctor" << endl; }
    annotate& operator=(const annotate&) { cout << "annotate assign" << endl; return *this; }
    annotate& operator=(annotate&&) noexcept { cout << "annotate move-assign" << endl; return *this; }
    ~annotate() { cout << "annotate dtor" << endl; }
};
// base class definition
struct base {
    template <typename T> base(T x) : self_(make_shared<model<T>>(move(x))) { }
    
    friend inline void foo(const base& x) { x.self_->foo_(); }
  private:
    struct concept {
        virtual void foo_() const = 0;
    };
    template <typename T> struct model : concept {
        model(T x) : data_(move(x)) { }
        void foo_() const override { foo(data_); }
        T data_;
    };
    shared_ptr<const concept> self_;
    annotate _;
};
// non-intrusive "inheritance"
class derived { };
void foo(const derived&) { cout << "foo of derived" << endl; }
struct non_random {
    int a = 0xAAAA;
    int b = 0xBBBB;
};
struct contain_annotate {
    contain_annotate() : m_(m_) { }
    int m_;
};
// demonstration
    void print(int x) { cout << x << endl; }
int main() {
    contain_annotate w;
    cout << "----" << endl;
    int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    
    auto x_ = new non_random;
    cout << "new non_random:" << hex << x_->a << endl;
    auto y_ = new non_random();
    cout << "new non_random():" << hex << y_->a << endl;
    cout << "--------------" << endl;
    
    for_each(&a[0], &a[0] + (sizeof(a) / sizeof(int)), &print);
    for_each(begin(a), end(a), [](int x) { cout << x << endl; });
    
    for (int *f = &a[0], *l = &a[0] + (sizeof(a) / sizeof(int)); f != l; ++f) cout << *f << endl;
    
    for (const auto& e : a) cout << e << endl;
    
    transform(begin(a), end(a), begin(a), [](int x) { return x + 1; });
    
    for (auto& e : a) e += 1;
    for (const auto& e : a) cout << e << endl;
    cout << endl << "-- ctor setup --" << endl << endl;
    base x = derived(); // ctor
    base y = derived();
    
    cout << endl << "-- test copy-ctor --" << endl << endl;
    {
    base z = x; // copy-ctor
    }
    cout << endl << "-- test move-ctor --" << endl << endl;
    {
    base z = move(x); // move-ctor
    }
    cout << endl << "-- test assign --" << endl << endl;
    {
    x = y; // assign
    }
    cout << endl << "-- test move-assign --" << endl << endl;
    {
    x = move(y); // assign
    }
    cout << endl << "-- dtor teardown --" << endl << endl;
}
#endif

#if 0
#include <functional>
namespace adobe {
/**************************************************************************************************/
namespace details {
template <typename F> // F models function
struct bind_direct_t {
    using type = F;
    
    static inline type bind(F f) { return f; }
};
template <typename R, typename T>
struct bind_direct_t<R T::*> {
    using type = decltype(std::bind(std::declval<R T::*&>(), std::placeholders::_1));
    
    static inline type bind(R T::* f) { return std::bind(f, std::placeholders::_1); }
};
template <typename R, typename T>
struct bind_direct_t<R (T::*)()> {
    using type = decltype(std::bind(std::declval<R (T::*&)()>(), std::placeholders::_1));
    
    static inline type bind(R (T::*f)()) { return std::bind(f, std::placeholders::_1); }
};
template <typename R, typename T, typename A>
struct bind_direct_t<R (T::*)(A)> {
    using type = decltype(std::bind(std::declval<R (T::*&)(A)>(), std::placeholders::_1,
        std::placeholders::_2));
    
    static inline type bind(R (T::*f)(A)) { return std::bind(f, std::placeholders::_1,
        std::placeholders::_2); }
};
template <typename R, typename T>
struct bind_direct_t<R (T::*)() const> {
    using type = decltype(std::bind(std::declval<R (T::*&)() const>(), std::placeholders::_1));
    
    static inline type bind(R (T::*f)() const) { return std::bind(f, std::placeholders::_1); }
};
template <typename R, typename T, typename A>
struct bind_direct_t<R (T::*)(A) const> {
    using type = decltype(std::bind(std::declval<R (T::*&)(A) const>(), std::placeholders::_1,
        std::placeholders::_2));
    
    static inline type bind(R (T::*f)(A) const) { return std::bind(f, std::placeholders::_1,
        std::placeholders::_2); }
};
} // namespace details;
  
/**************************************************************************************************/
template <typename F> // F models function
typename details::bind_direct_t<F>::type make_callable(F&& f) {
    return details::bind_direct_t<F>::bind(std::forward<F>(f));
}
} // namespace adobe
/**************************************************************************************************/
#include <iostream>
using namespace adobe;
using namespace std;
struct test {
    test() { } 
    int member_ = 3;
    const int cmember_ = 5;
    int member() { cout << "member()" << endl; return 7; }
    int member2(int x) { cout << "member(int)" << endl; return x; }
};
int main() {
    cout << adobe::make_callable(&test::member_)(test()) << endl;
    cout << adobe::make_callable(&test::cmember_)(test()) << endl;
    cout << adobe::make_callable(&test::member)(test()) << endl;
    cout << adobe::make_callable(&test::member2)(test(), 9) << endl;
    cout << adobe::make_callable([]{ cout << "lambda" << endl; return 11; } )() << endl;
}
#endif

/**************************************************************************************************/

#if 0
#include <algorithm>
#include <iostream>
#include <list>
#include <future>
#include <exception>
#include <unistd.h>
using namespace std;
/**************************************************************************************************/
#include <dispatch/dispatch.h>
namespace adobe {
template <typename F, typename ...Args>
auto async(F&& f, Args&&... args) -> std::future<typename std::result_of<F (Args...)>::type> {
    using result_type = typename std::result_of<F (Args...)>::type;
    using packaged_type = std::packaged_task<result_type ()>;
    
    auto p = new packaged_type(std::bind(f, std::forward<Args>(args)...));
    auto r = p->get_future();
    dispatch_async_f(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
            p, [](void* p_) {
                auto p = static_cast<packaged_type*>(p_);
                (*p)();
                delete p;
            });
    
    return r;
}
} // namespace adobe
#if 0
template <typename F, typename ...Args>
struct share {
    using result_type = typename result_of<F (Args...)>::type;
    
    share(F&& f) : f_(move(f)) { }
    
    ~share() {
        cout << "dtor" << endl;
        try {
            promise_.set_value(future_.get());
        } catch(...) {
            promise_.set_exception(current_exception());
        }
    }
    
    result_type invoke(Args&&... args) const { return f_(forward<Args>(args)...); }
    
    promise<result_type> promise_;
    future<result_type> future_;
    F f_;
};
template <typename F, typename ...Args>
auto async_(F&& f, Args&&... args) -> future<typename result_of<F (Args...)>::type>
{
    
    auto shared = make_shared<share<F, Args...>>(move(f));
    shared->future_ = async(bind(&share<F, Args...>::invoke, shared));
    return shared->promise_.get_future();
}
#endif
template <typename T> // models regular
class event_task {
public:
    template <typename F> // models void(T)
    event_task(F receiver) : shared_(make_shared<shared>(std::move(receiver))) { }
    
    void send(T x) {
        shared& s = *shared_;
        list<T> l;
        l.push_back(move(x));
        
        bool flag;
        {
            lock_t lock(s.mutex_);
            s.queue_.splice(s.queue_.end(), l);
            flag = s.flag_; s.flag_ = true;
        }
        if (!flag) {
            wait(); // check for exceptions
            auto task = packaged_task<void()>(bind(&shared::call_receiver, shared_ptr<shared>(shared_)));
            future_ = task.get_future();
            thread(move(task)).detach();
        }
    }
    
    void wait() {
        if (future_.valid()) future_.get(); // check for exceptions
    }
    
private:
    typedef lock_guard<mutex> lock_t;
    
    struct shared {
        
        template <typename F> // models void(T)
        shared(F receiver)
            : receiver_(std::move(receiver)),
              flag_(false)
              { }
        
        ~shared() { cout << "shared dtor" << endl; }
        void call_receiver() {
            list<T> l;
            do {
                {
                lock_t lock(mutex_);
                swap(queue_, l);    // receive
                }
                for (auto&& e : l) receiver_(move(e));
            } while (!l.empty());
            lock_t lock(mutex_);
            flag_ = false;
        }
        std::function<void(T x)>    receiver_;
        std::list<T>                queue_;
        bool                        flag_;
        std::mutex                  mutex_;
    };
    
    future<void>            future_;
    std::shared_ptr<shared> shared_;
};
class opaque {
    class imp;
    imp* self;
  private:
    opaque() noexcept : self(nullptr) { }
    
    // provide other ctors
    
    ~opaque(); // provide dtor
    
    opaque(const opaque&); // provide copy
    opaque(opaque&& x) noexcept : self(x.self) { x.self = nullptr; }
    
    opaque& operator=(const opaque& x) { return *this = opaque(x); }
    opaque& operator=(opaque&& x) noexcept { swap(*this, x); return *this; }
    
    friend inline void swap(opaque& x, opaque& y) noexcept { std::swap(x.self, y.self); }
};
struct talk {
    talk() { cout << "default-ctor" << endl; }
    
    ~talk() { cout << "dtor" << endl; }
    
    talk(const talk&) { cout << "copy-ctor" << endl; }
    talk(talk&& x) noexcept { cout << "move-ctor" << endl; }
    
    talk& operator=(const talk& x) { return *this = talk(x); }
    talk& operator=(talk&& x) noexcept { cout << "move-assign" << endl; return *this; }
};
void sink(talk x) {
    static talk x_;
    x_ = move(x);
}
int main() {
#if 0
    auto f = async_([]{
        sleep(3);
        cout << "testing" << endl;
        return 42;
    });
    
    cout << "sync" << endl;
    
    f.get();
    
    cout << "done" << endl;
#endif
    {
    event_task<std::string> task([](string x){
        cout << x << endl;
        sleep(3);
        // throw "fail";
    });
    
    task.send("Hello");
    task.send("World");
    sleep(1);
    cout << "Doing Work" << endl;
    }
    
    cout << "Waiting" << endl;
    sleep(9);
    cout << "done" << endl;
    
    #if 0
    try {
        task.wait();
    } catch(const char* x) {
        cout << "exception: " << x << endl;
    }
    #endif
    #if 0
    auto&& x = talk();
    x = talk();
    
    cout << "holding x" << endl;
    sink(move(x));
    cout << "done" << endl;
    #endif
}
#endif
#if 0
#include <iostream>
#include <memory>
#include <utility>
using namespace std;
class some_class {
  public:
    some_class(int x) : remote_(new int(x)) { } // ctor
    
    some_class(const some_class& x) : remote_(new int(*x.remote_)) { cout << "copy" << endl; }
    some_class(some_class&& x) noexcept = default;
    
    some_class& operator=(const some_class& x) { *this = some_class(x); return *this; }
    some_class& operator=(some_class&& x) noexcept = default;
    
  private:
    unique_ptr<int> remote_;
};
struct some_struct {
    some_class member_;
};
class some_constructor {
 public:
    some_constructor(string x) : str_(move(x)) { }
    string str_;
};
string append_world(string x) { return x += "World"; }
template <typename T, size_t N>
struct for_each_t {
    for_each_t(T (&r)[N]) : rp_(&r) { }
    T (*rp_)[N];
};
template <typename T, size_t N>
for_each_t<T, N> for_each(T (&r)[N]) { return for_each_t<T, N>(r); }
template <typename T, size_t N, typename F>
void operator|(const for_each_t<T, N>& fe, F f)
{
    for (const T& e : *fe.rp_) f(e);
}
int main() {
    int a[] = { 0, 1, 2, 3 };
    
    for_each(a) | [](int x) { cout << x << endl; };
    
    
    some_class x(42);
    
    x = some_class(53);
#if 0
    string x = "Hello";
    string y = append_world(x);
    
    cout << x << endl;
#endif
    cout << "done" << endl;
}
#endif


#if 0
#include <functional>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <utility>
#include <cassert>
namespace adobe {
typedef std::function<void ()>* cancellation_token_registration;
namespace details {
struct cancel_state {
    typedef std::list<std::function<void ()>> list_type;
    cancel_state() : canceled_(false) { }
    void cancel() {
        list_type list;
        {
            std::lock_guard<std::mutex> lock(guard_);
            if (!canceled_) {
                canceled_ = true;
                swap(list, callback_list_);
            }
        }
        for (const auto& e: list) e();
    }
    
    template <typename F> // F models void ()
    cancellation_token_registration register_callback(F f) {
        list_type list(std::move(f));
        cancellation_token_registration result = nullptr;
        bool canceled;
        {
            std::lock_guard<std::mutex> lock(guard_);
            canceled = canceled_;
            if (!canceled) {
                callback_list_.splice(end(callback_list_), list);
                result = &callback_list_.back();
            }
        }
        if (canceled) list.back()();
        return result;
    }
    
    void deregister_callback(const cancellation_token_registration& token) {
        std::lock_guard<std::mutex> lock(guard_);
        auto i = find_if(begin(callback_list_), end(callback_list_),
                [token](const std::function<void ()>& x) { return &x == token; });
        if (i != end(callback_list_)) callback_list_.erase(i);
    }
    
    bool is_canceled() const { return canceled_; }
    std::mutex guard_;
    volatile bool canceled_; // Write once, read many
    list_type callback_list_;
};
struct cancellation_scope;
template <typename T> // placeholder to allow header only library
cancellation_scope*& get_cancellation_scope() {
    __thread static cancellation_scope* cancellation_stack = nullptr;
    return cancellation_stack;
}
} // namespace details
class cancellation_token_source;
class cancellation_token {
  public:
    template <typename F> // F models void ()
    cancellation_token_registration register_callback(F f) const {
        return state_ ? state_->register_callback(move(f)) : nullptr;
    }
    
    void deregister_callback(const cancellation_token_registration& token) const {
        if (state_) state_->deregister_callback(token);
    }
    
    bool is_cancelable() const { return static_cast<bool>(state_); }
    
    
    bool is_canceled() const { return state_ ? state_->is_canceled() : false; }
    
    static cancellation_token none() { return cancellation_token(); }
    
    friend inline bool operator==(const cancellation_token& x, const cancellation_token& y) {
        return x.state_ == y.state_;
    }
    
  private:
    friend class cancellation_token_source;
    cancellation_token() { }
    explicit cancellation_token(std::shared_ptr<details::cancel_state> s) : state_(std::move(s)) { }
    
    std::shared_ptr<details::cancel_state> state_;
};
inline bool operator!=(const cancellation_token& x, const cancellation_token& y) {
    return !(x == y);
}
namespace details {
struct cancellation_scope {
    cancellation_scope(cancellation_token token) : token_(std::move(token)) {
        cancellation_scope*& scope = get_cancellation_scope<void>();
        prior_ = scope;
        scope = this;
    }
    
    ~cancellation_scope() { get_cancellation_scope<void>() = prior_; }
    
    cancellation_scope* prior_;
    const cancellation_token& token_;
};
} // namespace details
class cancellation_token_source {
  public:
    cancellation_token_source() : state_(std::make_shared<details::cancel_state>()) { }
    
    void cancel() const { state_->cancel(); }
    
    cancellation_token get_token() const { return cancellation_token(state_); }
    
    friend inline bool operator==(const cancellation_token_source& x, const cancellation_token_source& y) {
        return x.state_ == y.state_;
    }
        
  private:
    std::shared_ptr<details::cancel_state> state_;
};
inline bool operator!=(const cancellation_token_source& x, const cancellation_token_source& y) {
    return !(x == y);
}
class task_canceled : public std::exception {
  public:
    const char* what() const noexcept { return "task_canceled"; }
};
inline bool is_task_cancellation_requested() {
    const details::cancellation_scope* scope = details::get_cancellation_scope<void>();
    return scope ? scope->token_.is_canceled() : false;
}
[[noreturn]] inline void cancel_current_task() {
    throw task_canceled();
}
template <typename F> // F models void ()
inline void run_with_cancellation_token(const F& f, cancellation_token token) {
    details::cancellation_scope scope(std::move(token));
    if (is_task_cancellation_requested()) cancel_current_task();
    f();
}
template <typename T>
class optional {
  public:
    optional() : initialized_(false) { }
    
    optional& operator=(T&& x) {
        if (initialized_)reinterpret_cast<T&>(storage_) = std::forward<T>(x);
        else new(&storage_) T(std::forward<T>(x));
        initialized_ = true;
        return *this;
    }
    
    T& get() {
        assert(initialized_ && "getting unset optional.");
        return reinterpret_cast<T&>(storage_);
    }
    
    explicit operator bool() const { return initialized_; }
    
    ~optional() { if (initialized_) reinterpret_cast<T&>(storage_).~T(); }
  private:
    optional(const optional&);
    optional operator=(const optional&);
  
    alignas(T) char storage_[sizeof(T)];
    bool initialized_;
};
template <typename F, typename SIG> class cancelable_function;
template <typename F, typename R, typename ...Arg>
class cancelable_function<F, R (Arg...)> {
  public:
    typedef R result_type;
  
    cancelable_function(F f, cancellation_token token) :
            function_(std::move(f)), token_(std::move(token)) { }
    
    R operator()(Arg&& ...arg) const {
        optional<R> r;
        
        run_with_cancellation_token([&]{
            r = function_(std::forward<Arg>(arg)...);
        }, token_);
        
        if (!r) cancel_current_task();
        return std::move(r.get());
    }
    
  private:
    F function_;
    cancellation_token token_;
};
#if 1
template <typename F, typename ...Arg>
class cancelable_function<F, void (Arg...)> {
  public:
    typedef void result_type;
  
  
    cancelable_function(F f, cancellation_token token) :
            function_(std::move(f)), token_(std::move(token)) { }
    
    void operator()(Arg&& ...arg) const {
        bool executed = false;
        
        run_with_cancellation_token([&]{
            executed = true;
            function_(std::forward<Arg>(arg)...);
        }, token_);
        
        if (!executed) cancel_current_task();
    }
    
  private:
    F function_;
    cancellation_token token_;
};
#endif
template <typename SIG, typename F> // F models void ()
cancelable_function<F, SIG> make_cancelable(F f, cancellation_token token) {
    return cancelable_function<F, SIG>(f, token);
}
} // namespace adobe
#include <iostream>
#include <unistd.h>
#include <future>
#include <string>
using namespace std;
using namespace adobe;
/**************************************************************************************************/
#include <dispatch/dispatch.h>
namespace adobe {
template <typename F, typename ...Args>
auto async(F&& f, Args&&... args) -> std::future<typename std::result_of<F (Args...)>::type> {
    using result_type = typename std::result_of<F (Args...)>::type;
    using packaged_type = std::packaged_task<result_type ()>;
    
    auto p = new packaged_type(std::bind(f, std::forward<Args>(args)...));
    auto r = p->get_future();
    dispatch_async_f(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0),
            p, [](void* p_) {
                auto p = static_cast<packaged_type*>(p_);
                (*p)();
                delete p;
            });
    
    return r;
}
} // namespace adobe
/**************************************************************************************************/
template <typename R>
struct for_each_t {
    for_each_t(const R& r) : rp_(&r);
    const R* rp_;
};
template <typename R, // R models range
          typename F> // F models void (value_type(R))
for_each_t<R> for_each(const R& r) { }
template <typename F>
void operator|(const for_each_t& fe, F f)
{
    for (const auto& e : *fe.rp_) f;
}
int main() {
    int a[] = { 0, 1, 2, 3 };
    
    for_each(a) | [](int x) { cout << x << endl; }
    pair<int, int> x;
    cancellation_token_source cts;
    
    cout << "Creating task..." << endl;
    
    auto f = make_cancelable<int (const string& message)>([](const string& message){
        for (int count = 0; count != 10; ++count) {
            if (is_task_cancellation_requested()) cancel_current_task();
            
            cout << "Performing work:" << message << endl;
            usleep(250);
        }
        return 42;
    }, cts.get_token());
    
    
    // Create a task that performs work until it is canceled.
    auto t = adobe::async(f, "Boogie down!");
    
    cout << "Doing something else..." << endl;
    
    adobe::async(f, "Running Free!");
    
    cout << "Got:" << t.get() << endl;
    
    cout << "Completed Successfully..." << endl;
    cout << "Creating another task..." << endl;
    t = adobe::async(f, "Boogie up!");
    
    cout << "Doing something else..." << endl;
    usleep(1000);
    cout << "Canceling task..." << endl;
    cts.cancel();
    
    try {
        t.get();
    } catch (const task_canceled&) {
        cout << "Cancel exception propogated." << endl;
    }
    cout << "Done." << endl;
}
#endif


#if 0
//
#include <vector>
#include <iostream>
using namespace std;
vector<int> func() {
	vector<int> a = { 0, 1, 2, 3 };
	//...
	return a;
}
struct type {
	explicit type(vector<int> x) : member_(move(x)) { }
	vector<int> member_;
};
int main() {
	vector<int> b = func();
	vector<int> c;
	c = func();
	
	type d(func());
	type e(vector<int>({ 4, 5, 6 }));
}
#endif

#if 0
#include <cstddef>
template <typename I, // I models InputIterator
          typename N> // N models Incrementable
N count_unique(I f, I l, N n) {
    if (f == l) return n;
    I p = f; ++f; ++n;
    
    while (f != l) {
        if (*f != *p) { p = f; ++n; }
        ++f;
    }
    return n;
}
template <typename I> // I models InputIterator
std::size_t count_unique(I f, I l) {
    return count_unique(f, l, std::size_t());
}
#include <iostream>
using namespace std;
int main() {
    int a[] = { 1, 2, 1, 3, 1, 4, 1, 5 };
    sort(begin(a), end(a));
    cout << count_unique(begin(a), end(a)) << endl;
}
#endif 



#if 0
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <adobe/lua.hpp>
using namespace std;
using namespace adobe;
class type {
  public:
    type(string x) : member_(move(x)) { }
    const string& what() const { return member_; }
  private:
    string member_;
};
any user(lua_State* L, int index) {
    return *static_cast<const int*>(lua_touserdata(L, index));
}
extern "C" {
int f_(lua_State* L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; ++i) {
        auto s = adobe::to_any(L, i, &user);
        cout << "<cpp>" << endl;
        if (s.type() == typeid(type)) {
            cout << '\t' << adobe::dynamic_cast_<const type&>(s).what() << endl;
        } else if (s.type() == typeid(int)) {
            cout << '\t' << adobe::dynamic_cast_<const int&>(s) << endl;
        }
        cout << "</cpp>" << endl;
    #if 0
        auto s = adobe::to<type>(L, i);
        cout << "<cpp>" << endl;
        cout << '\t' << s.what() << endl;
        cout << "</cpp>" << endl;
    #endif
    }
    return 0;
}
} // extern "C"
int main() {
    auto L = luaL_newstate();
    luaL_openlibs(L);
    
    luaL_dostring(L,
(R"lua(
                  
function luaFunction(arg1, f)
    print('<lua>')
    print('\t' .. type(arg1))
    print('</lua>/')
    f(arg1)
end
)lua")
    );
    
    lua_getglobal(L, "luaFunction");
    adobe::push(L, type("Testing"));
    lua_pushcfunction(L, f_);
    lua_pcall(L, 2, 0, 0);
    
    lua_getglobal(L, "luaFunction");
    new (lua_newuserdata(L, sizeof(int))) int(42);
    lua_pushcfunction(L, f_);
    lua_pcall(L, 2, 0, 0);
    
    lua_close(L);
    cout.flush();
}
}	// namespace sp
