
#if !defined(_TL_UNORDERED_BITS)
#define _TL_UNORDERED_BITS

#include <vector>
#include <functional>  // @@@
#include <algorithm>

namespace tl
{

//  @@@ TODO: remove
template <class X>
struct hash
{
  size_t operator () (const X &x) const
  {
    return std::hash<X> () (x);
  }
};


template <class V, class H, size_t max_bucket_size>
class __unordered_container
{
private:
  class __iterator;
  class __const_iterator;

  typedef std::vector<std::pair<size_t, V> > bucket_type;
  typedef std::vector<bucket_type> buckets_type;

public:
  typedef __iterator iterator;
  typedef __const_iterator const_iterator;

  __unordered_container ()
  {
    __init ();
  }

  ~__unordered_container ()
  {
    __clear ();
  }

  __unordered_container (const __unordered_container &other)
  {
    __init ();
    operator= (other);
  }

  __unordered_container (__unordered_container &&other)
  {
    __init ();
    operator= (std::move (other));
  }

  __unordered_container &operator= (const __unordered_container &other)
  {
    if (this != &other) {

      __clear ();

      m_buckets.reserve (other.m_buckets.size ());

      for (auto b = other.m_buckets.begin (); b != other.m_buckets.end (); ++b) {
        m_buckets.push_back (*b);
      }

      m_size = other.m_size;

    }
    return *this;
  }

  __unordered_container &operator= (__unordered_container &&other)
  {
    if (this != &other) {
      swap (other);
    }
    return *this;
  }

  void swap (__unordered_container &other)
  {
    m_buckets.swap (other.m_buckets);
    std::swap (m_size, other.m_size);
  }

  bool operator== (const __unordered_container &other) const
  {
    if (this == &other) {
      return true;
    }
    if (size () != other.size ()) {
      return false;
    }
    for (auto i = begin (), j = other.begin (); i != end (); ++i, ++j) {
      return *i == *j;
    }
    return true;
  }

  bool operator!= (const __unordered_container &other) const
  {
    return ! operator== (other);
  }

  bool operator< (const __unordered_container &other) const
  {
    if (this == &other) {
      return false;
    }
    if (size () != other.size ()) {
      return size () < other.size ();
    }
    for (auto i = begin (), j = other.begin (); i != end (); ++i, ++j) {
      if (! (*i == *j)) {
        return *i < *j;
      }
    }
    return false;
  }

  iterator begin ()
  {
    if (empty ()) {
      return end ();
    } else {
      auto b = m_buckets.begin ();
      while (b != m_buckets.end () && b->empty ()) {
        ++b;
      }
      return iterator (b, m_buckets.end (), b->begin (), b->end ());
    }
  }

  iterator end ()
  {
    return iterator (m_buckets.end (), m_buckets.end (), typename bucket_type::iterator (), typename bucket_type::iterator ());
  }

  const_iterator begin () const
  {
    return cbegin ();
  }

  const_iterator cbegin () const
  {
    if (empty ()) {
      return end ();
    } else {
      __unordered_container *nc_this = const_cast<__unordered_container *> (this);
      auto b = nc_this->m_buckets.begin ();
      while (b != nc_this->m_buckets.end () && b->empty ()) {
        ++b;
      }
      return const_iterator (b, nc_this->m_buckets.end (), b->begin (), b->end ());
    }
  }

  const_iterator end () const
  {
    return cend ();
  }

  const_iterator cend () const
  {
    __unordered_container *nc_this = const_cast<__unordered_container *> (this);
    return const_iterator (nc_this->m_buckets.end (), nc_this->m_buckets.end (), typename bucket_type::iterator (), typename bucket_type::iterator ());
  }

  bool empty () const
  {
    return m_size == 0;
  }

  void clear ()
  {
    __clear ();
  }

  void erase (iterator i)
  {
    __erase (*i.m_b, i.m_nb);
  }

  size_t size () const
  {
    return m_size;
  }

  //  for testing purposes
  const buckets_type &__buckets () const
  {
    return m_buckets;
  }

protected:
  template <class X, class C>
  const_iterator __find (const X &x, const C &c) const
  {
    __unordered_container *nc_this = const_cast<__unordered_container *> (this);
    auto f = nc_this->__find_impl (x, m_h (x), c);
    if (f.first != nc_this->m_buckets.end ()) {
      return const_iterator (f.first, nc_this->m_buckets.end (), f.first->begin () + f.second, f.first->end ());
    } else {
      return end ();
    }
  }

  template <class X, class C>
  iterator __find (const X &x, const C &c)
  {
    auto f = __find_impl (x, m_h (x), c);
    if (f.first != m_buckets.end ()) {
      return iterator (f.first, m_buckets.end (), f.first->begin () + f.second, f.first->end ());
    } else {
      return end ();
    }
  }

  template <class C>
  void __insert (const V &v, const C &compare)
  {
    size_t ib;
    typename buckets_type::iterator bucket;
    size_t hash = m_h (v);

    if (m_buckets.empty ()) {

      ib = 0;
      m_buckets.push_back (bucket_type ());
      bucket = m_buckets.begin ();

    } else {

      ib = hash % m_buckets.size ();
      bucket = m_buckets.begin () + ib;

      for (auto n = bucket->begin (); n != bucket->end (); ++n) {
        if (n->first == hash && compare (n->second, v)) {
          //  already present
          return;
        }
      }

    }

    if (bucket->size () >= max_bucket_size) {
      this->__split_buckets ();
      ib = hash % m_buckets.size ();
      bucket = m_buckets.begin () + ib;
    }

    bucket->push_back (std::make_pair (hash, v));

    m_size += 1;
  }

  void __erase (bucket_type &bucket, const typename bucket_type::iterator &n)
  {
    bucket.erase (n);
    m_size -= 1;
  }

private:
  H m_h;
  buckets_type m_buckets;
  size_t m_size;

  class __iterator_base
  {
  public:
    friend class __unordered_container;
    typedef typename __unordered_container::buckets_type::iterator buckets_iterator;
    typedef typename __unordered_container::bucket_type::iterator bucket_iterator;

    __iterator_base ()
      : m_b (), m_e (), m_nb (), m_ne ()
    {
    }

    bool operator== (const __iterator_base &other) const
    {
      return m_b == other.m_b && (m_b == m_e || m_nb == other.m_nb);
    }

    bool operator!= (const __iterator_base &other) const
    {
      return ! operator== (other);
    }

    bool at_end () const
    {
      return m_b == m_e;
    }

  protected:
    std::pair<size_t, V> &__node () const
    {
      return *m_nb;
    }

    void __set (buckets_iterator b, buckets_iterator e, bucket_iterator nb, bucket_iterator ne)
    { 
      m_b = b;
      m_e = e;
      m_nb = nb;
      m_ne = ne;
    }

    void __inc ()
    {
      if (m_nb != m_ne) {
        ++m_nb;
      }

      if (m_nb == m_ne) {
        ++m_b;
        if (m_b != m_e) {
          m_nb = m_b->begin ();
          m_ne = m_b->end ();
        }
      }
    }

  private:
    buckets_iterator m_b, m_e;
    bucket_iterator m_nb, m_ne;
  };

  class __iterator
    : public __iterator_base
  {
  public:
    typedef V value_type;
    typedef V &reference;
    typedef V *pointer;
    //  @@@ traits, iterator_category

    __iterator ()
      : __iterator_base ()
    {
    }

    __iterator (const __iterator &other)
    {
      this->__set (other.m_b, other.m_e, other.m_nb, other.m_ne);
    }

    __iterator &operator= (const __iterator &other)
    {
      if (this != &other) {
        this->assign (other);
      }
      return *this;
    }

    reference operator* () const
    {
      return this->__node ().second;
    }

    pointer operator-> () const
    {
      return &this->__node ().second;
    }

    __iterator operator++ ()
    {
      this->__inc ();
      return *this;
    }

  private:
    friend class __unordered_container;

    __iterator (typename __iterator_base::buckets_iterator b, typename __iterator_base::buckets_iterator e, typename __iterator_base::bucket_iterator nb, typename __iterator_base::bucket_iterator ne)
    { 
      this->__set (b, e, nb, ne);
    }
  };

  class __const_iterator
    : public __iterator_base
  {
  public:
    typedef V value_type;
    typedef const V &reference;
    typedef const V *pointer;
    //  @@@ traits, iterator_category

    __const_iterator ()
      : __iterator_base ()
    {
    }

    __const_iterator (const __const_iterator &other)
    {
      this->__set (other.m_b, other.m_e, other.m_nb, other.m_ne);
    }

    __const_iterator &operator= (const __const_iterator &other)
    {
      if (this != &other) {
        this->assign (other);
      }
      return *this;
    }

    reference operator* () const
    {
      return this->__node ().second;
    }

    pointer operator-> () const
    {
      return &this->__node ().second;
    }

    __const_iterator operator++ ()
    {
      this->__inc ();
      return *this;
    }

  private:
    friend class __unordered_container;

    __const_iterator (typename __iterator_base::buckets_iterator b, typename __iterator_base::buckets_iterator e, typename __iterator_base::bucket_iterator nb, typename __iterator_base::bucket_iterator ne)
    { 
      this->__set (b, e, nb, ne);
    }
  };

  void __init ()
  {
    m_size = 0;
  }

  void __clear ()
  {
    m_buckets.clear ();
    m_size = 0;
  }

  template <class C, class X>
  std::pair<typename buckets_type::iterator, size_t> __find_impl (const X &x, size_t hash, const C &compare)
  {
    if (! m_buckets.empty ()) {

      size_t ib = hash % m_buckets.size ();
      auto bucket = m_buckets.begin () + ib;

      for (auto f = bucket->begin (); f != bucket->end (); ++f) {
        if (f->first == hash && compare (f->second, x)) {
          return std::pair<typename buckets_type::iterator, size_t> (bucket, f - bucket->begin ());
        }
      }

    }

    return std::pair<typename buckets_type::iterator, size_t> (m_buckets.end (), 0);
  }

  void __split_buckets ()
  {
    buckets_type new_buckets;
    size_t new_buckets_size = m_buckets.size () * 2;
    size_t new_bucket_mask = new_buckets_size >> 1;

    new_buckets.resize (new_buckets_size, bucket_type ());

    size_t ib = 0;
    for (auto b = m_buckets.begin (); b != m_buckets.end (); ++b, ++ib) {

      bucket_type &b_new = new_buckets [ib];
      bucket_type &b_new_other = new_buckets [ib + m_buckets.size ()];

      b_new.swap (*b);

      size_t other_size = 0;
      for (auto n = b_new.begin (); n != b_new.end (); ++n) {
        if ((n->first & new_bucket_mask) != 0) {
          ++other_size;
        }
      }
      b_new_other.reserve (other_size);

      auto w = b_new.begin ();

      for (auto n = b_new.begin (); n != b_new.end (); ++n) {
        if ((n->first & new_bucket_mask) != 0) {
          b_new_other.push_back (std::move (*n));
        } else {
          if (w != n) {
            *w = std::move (*n);
          }
          ++w;
        }
      }

      b_new.erase (w, b_new.end ());

    }

    m_buckets.swap (new_buckets);
  }
};

}

#endif

