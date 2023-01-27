
#if !defined(_TL_UNORDERED_SET)
#define _TL_UNORDERED_SET

#include "unordered_bits.h"

namespace tl
{

template <class V, class H>
struct __hash_extractor_set
{
  __hash_extractor_set ()
    : m_h ()
  {
  }

  __hash_extractor_set (const H &h)
    : m_h (h)
  {
  }

  size_t operator () (const V &v) const
  {
    return m_h (v);
  }

private:
  H m_h;
};

template <class V, class H = tl::hash<V>, size_t max_bucket_size = 128>
class unordered_set
  : public __unordered_container<V, __hash_extractor_set<V, H>, max_bucket_size>
{
private:
  struct __compare
  {
    bool operator() (const V &a, const V &b) const
    {
      return a == b;
    }
  };

public:
  typedef __unordered_container<V, __hash_extractor_set<V, H>, max_bucket_size> basic_container;
  typedef typename basic_container::iterator iterator;
  typedef typename basic_container::const_iterator const_iterator;

  unordered_set ()
    : __unordered_container<V, __hash_extractor_set<V, H>, max_bucket_size> ()
  {
  }

  template <class I>
  unordered_set (const I &from, const I &to)
    : __unordered_container<V, __hash_extractor_set<V, H>, max_bucket_size> (from, to)
  {
  }

  unordered_set (const unordered_set &other)
    : __unordered_container<V, __hash_extractor_set<V, H>, max_bucket_size> (other)
  {
  }

  unordered_set (unordered_set &&other)
    : __unordered_container<V, __hash_extractor_set<V, H>, max_bucket_size> (other)
  {
  }

  unordered_set &operator= (const unordered_set &other)
  {
    __unordered_container<V, __hash_extractor_set<V, H>, max_bucket_size>::operator= (other);
    return *this;
  }

  unordered_set &operator= (unordered_set &&other)
  {
    __unordered_container<V, __hash_extractor_set<V, H>, max_bucket_size>::operator= (other);
    return *this;
  }

  const_iterator find (const V &v) const
  {
    return this->__find (v, __compare ());
  }

  iterator find (const V &v) 
  {
    return this->__find (v, __compare ());
  }

  void insert (const V &v)
  {
    iterator i = find (v);
    if (i.at_end ()) {
      __unordered_container<V, __hash_extractor_set<V, H>, max_bucket_size>::insert (v);
    }
  }

  template <class I>
  void insert (const I &from, const I &to)
  {
    for (auto i = from; i != to; ++i) {
      this->insert (*i);
    }
  }

  void erase (const V &v)
  {
    iterator i = find (v);
    if (i.mp_n) {
      erase (i);
    }
  }
};

}

#endif

