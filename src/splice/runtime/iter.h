#pragma once

#include "str.h"
#include "tuple.h"
#include "types.h"
#include <type_traits>

namespace py {

// enumerate(c): (index, element) pairs.
template <typename Iterator> class enumerate_iterator {
  public:
    ALWAYS_INLINE enumerate_iterator(Iterator it, _int index) : it_(it), index_(index) {}
    ALWAYS_INLINE auto operator*() const {
        return tuple<_int, std::decay_t<decltype(*it_)>>(index_, *it_);
    }
    ALWAYS_INLINE enumerate_iterator &operator++() {
        ++it_;
        ++index_;
        return *this;
    }
    ALWAYS_INLINE bool operator!=(const enumerate_iterator &o) const { return it_ != o.it_; }

  private:
    Iterator it_;
    _int index_;
};

template <typename Container> class enumerate_view {
  public:
    explicit ALWAYS_INLINE enumerate_view(Container &&c) : c_(std::forward<Container>(c)) {}
    ALWAYS_INLINE auto begin() { return enumerate_iterator<decltype(c_.begin())>(c_.begin(), 0); }
    ALWAYS_INLINE auto end() { return enumerate_iterator<decltype(c_.begin())>(c_.end(), 0); }

  private:
    Container c_;
};

template <typename Container> ALWAYS_INLINE auto enumerate(Container &&c) {
    return enumerate_view<Container>(std::forward<Container>(c));
}

// zip(c1, c2): stops at the shorter of the two.
template <typename It1, typename It2> class zip_iterator {
  public:
    ALWAYS_INLINE zip_iterator(It1 it1, It2 it2) : it1_(it1), it2_(it2) {}
    ALWAYS_INLINE auto operator*() const {
        return tuple<std::decay_t<decltype(*it1_)>, std::decay_t<decltype(*it2_)>>(
            *it1_, *it2_);
    }
    ALWAYS_INLINE zip_iterator &operator++() {
        ++it1_;
        ++it2_;
        return *this;
    }
    // Not equal to end() as long as neither side has reached its own end -
    // matches "stop when either is exhausted".
    ALWAYS_INLINE bool operator!=(const zip_iterator &o) const {
        return it1_ != o.it1_ && it2_ != o.it2_;
    }

  private:
    It1 it1_;
    It2 it2_;
};

template <typename C1, typename C2> class zip_view {
  public:
    ALWAYS_INLINE zip_view(C1 &&c1, C2 &&c2) : c1_(std::forward<C1>(c1)), c2_(std::forward<C2>(c2)) {}
    ALWAYS_INLINE auto begin() {
        return zip_iterator<decltype(c1_.begin()), decltype(c2_.begin())>(
            c1_.begin(), c2_.begin());
    }
    ALWAYS_INLINE auto end() {
        return zip_iterator<decltype(c1_.begin()), decltype(c2_.begin())>(
            c1_.end(), c2_.end());
    }

  private:
    C1 c1_;
    C2 c2_;
};

template <typename C1, typename C2> ALWAYS_INLINE auto zip(C1 &&c1, C2 &&c2) {
    return zip_view<C1, C2>(std::forward<C1>(c1), std::forward<C2>(c2));
}

// map(func, c): func applied to each element.
template <typename Iterator, typename Func> class map_iterator {
  public:
    ALWAYS_INLINE map_iterator(Iterator it, Func func) : it_(it), func_(func) {}
    ALWAYS_INLINE auto operator*() const { return func_(*it_); }
    ALWAYS_INLINE map_iterator &operator++() {
        ++it_;
        return *this;
    }
    ALWAYS_INLINE bool operator!=(const map_iterator &o) const { return it_ != o.it_; }

  private:
    Iterator it_;
    Func func_;
};

template <typename Container, typename Func> class map_view {
  public:
    ALWAYS_INLINE map_view(Func func, Container &&c) : func_(func), c_(std::forward<Container>(c)) {}
    ALWAYS_INLINE auto begin() { return map_iterator<decltype(c_.begin()), Func>(c_.begin(), func_); }
    ALWAYS_INLINE auto end() { return map_iterator<decltype(c_.begin()), Func>(c_.end(), func_); }

  private:
    Func func_;
    Container c_;
};

template <typename Container, typename Func> ALWAYS_INLINE auto map(Func func, Container &&c) {
    return map_view<Container, Func>(func, std::forward<Container>(c));
}

// filter(pred, c): elements pred accepts. begin() and each ++ both need to
// know where to stop skipping, so the iterator carries its own end.
template <typename Iterator, typename Pred> class filter_iterator {
  public:
    filter_iterator(Iterator it, Iterator end, Pred pred)
        : it_(it), end_(end), pred_(pred) {
        advance_to_match();
    }
    ALWAYS_INLINE auto operator*() const { return *it_; }
    filter_iterator &operator++() {
        ++it_;
        advance_to_match();
        return *this;
    }
    ALWAYS_INLINE bool operator!=(const filter_iterator &o) const { return it_ != o.it_; }

  private:
    void advance_to_match() {
        while (it_ != end_ && !pred_(*it_))
            ++it_;
    }
    Iterator it_;
    Iterator end_;
    Pred pred_;
};

template <typename Container, typename Pred> class filter_view {
  public:
    filter_view(Pred pred, Container &&c) : pred_(pred), c_(std::forward<Container>(c)) {}
    auto begin() {
        return filter_iterator<decltype(c_.begin()), Pred>(c_.begin(), c_.end(), pred_);
    }
    auto end() {
        return filter_iterator<decltype(c_.begin()), Pred>(c_.end(), c_.end(), pred_);
    }

  private:
    Pred pred_;
    Container c_;
};

template <typename Container, typename Pred> ALWAYS_INLINE auto filter(Pred pred, Container &&c) {
    return filter_view<Container, Pred>(pred, std::forward<Container>(c));
}

// reversed(c): back to front via indexing, matching Python's reversed() -
// needs len()/operator[], not a bidirectional iterator.
template <typename Container> class reversed_iterator {
  public:
    ALWAYS_INLINE reversed_iterator(Container &c, _int i) : c_(c), i_(i) {}
    ALWAYS_INLINE auto operator*() const { return c_[i_]; }
    ALWAYS_INLINE reversed_iterator &operator++() {
        --i_;
        return *this;
    }
    ALWAYS_INLINE bool operator!=(const reversed_iterator &o) const { return i_ != o.i_; }

  private:
    Container &c_;
    _int i_;
};

template <typename Container> class reversed_view {
  public:
    explicit ALWAYS_INLINE reversed_view(Container &c) : c_(c) {}
    ALWAYS_INLINE auto begin() { return reversed_iterator<Container>(c_, len(c_) - 1); }
    ALWAYS_INLINE auto end() { return reversed_iterator<Container>(c_, -1); }

  private:
    Container &c_;
};

template <typename Container> ALWAYS_INLINE auto reversed(Container &c) {
    return reversed_view<Container>(c);
}

} // namespace py
