
#if !defined(_TL_UNORDERED_MAP)
#define _TL_UNORDERED_MAP

#include "unordered_bits.h"

namespace tl
{

template <class K, class V, class H>
struct __hash_extractor_map
{
  __hash_extractor_map ()
    : m_h ()
  {
  }

  __hash_extractor_map (const H &h)
    : m_h (h)
  {
  }

  size_t operator () (const K &k) const
  {
    return m_h (k);
  }

  size_t operator () (const std::pair<K, V> &kv) const
  {
    return m_h (kv.first);
  }

private:
  H m_h;
};

template <class K, class V, class H = tl::hash<K>, size_t max_bucket_size = 128>
class unordered_map
  : public __unordered_container<std::pair<K, V>, __hash_extractor_map<K, V, H>, max_bucket_size>
{
private:
  struct __compare
  {
    bool operator() (const std::pair<K, V> &a, const K &b) const
    {
      return a.first == b;
    }

    bool operator() (const std::pair<K, V> &a, const std::pair<K, V> &b) const
    {
      return a.first == b.first;
    }
  };

public:
  typedef __unordered_container<std::pair<K, V>, __hash_extractor_map<K, V, H>, max_bucket_size> basic_container;
  typedef typename basic_container::iterator iterator;
  typedef typename basic_container::const_iterator const_iterator;

  unordered_map ()
    : __unordered_container<std::pair<K, V>, __hash_extractor_map<K, V, H>, max_bucket_size> ()
  {
  }

  template <class I>
  unordered_map (const I &from, const I &to)
    : __unordered_container<std::pair<K, V>, __hash_extractor_map<K, V, H>, max_bucket_size> ()
  {
    insert (from, to);
  }

  unordered_map (const unordered_map &other)
    : __unordered_container<std::pair<K, V>, __hash_extractor_map<K, V, H>, max_bucket_size> (other)
  {
  }

  unordered_map (unordered_map &&other)
    : __unordered_container<std::pair<K, V>, __hash_extractor_map<K, V, H>, max_bucket_size> (std::move (other))
  {
  }

  unordered_map &operator= (const unordered_map &other)
  {
    __unordered_container<std::pair<K, V>, __hash_extractor_map<K, V, H>, max_bucket_size>::operator= (other);
    return *this;
  }

  unordered_map &operator= (unordered_map &&other)
  {
    __unordered_container<std::pair<K, V>, __hash_extractor_map<K, V, H>, max_bucket_size>::operator= (std::move (other));
    return *this;
  }

  const_iterator cfind (const K &k) const
  {
    return this->__find (k, __compare ());
  }

  const_iterator find (const K &k) const
  {
    return cfind (k);
  }

  iterator find (const K &k) 
  {
    return this->__find (k, __compare ());
  }

  void insert (const std::pair<K, V> &v)
  {
    this->__insert (v, __compare ());
  }

  template <class I>
  void insert (const I &from, const I &to)
  {
    __compare compare;
    for (auto i = from; i != to; ++i) {
      this->__insert (*i, compare);
    }
  }

  void erase (iterator i)
  {
    __unordered_container<std::pair<K, V>, __hash_extractor_map<K, V, H>, max_bucket_size>::erase (i);
  }

  void erase (const K &k)
  {
    iterator i = find (k);
    if (! i.at_end ()) {
      erase (i);
    }
  }
};

}

#endif

