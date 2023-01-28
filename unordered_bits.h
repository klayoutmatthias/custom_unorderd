
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


template <class V, class H, size_t max_bucket_size = 128>
class __unordered_container
{
private:
  class __iterator;
  class __const_iterator;

  class __node
    : public std::pair<__node *, V>
  {
  public:
    __node () : std::pair<__node *, V> () { }
    __node (__node *n, const V &v) : std::pair<__node *, V> (n, v) { }
  };

  typedef __node node_type;
  typedef std::pair<node_type *, size_t> bucket_type;
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

  template <class I>
  __unordered_container (const I &from, const I &to)
  {
    __init ();
    insert (from, to);
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

      for (auto b = other.m_buckets.begin (); b != other.m_buckets.end (); ++b) {

        m_buckets.push_back (bucket_type (0, b->second));

        node_type **last_node_new = &m_buckets.back ().first;
        
        for (node_type *node = b->first; node; node = node->first) {
          node_type *new_node = new node_type (0, node->second);
          *last_node_new = new_node;
          last_node_new = &new_node->first;
        }

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
      while (b != m_buckets.end () && ! b->first) {
        ++b;
      }
      return iterator (b, m_buckets.end (), b->first);
    }
  }

  iterator end ()
  {
    return iterator (m_buckets.end (), m_buckets.end (), 0);
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
      while (b != nc_this->m_buckets.end () && ! b->first) {
        ++b;
      }
      return const_iterator (b, nc_this->m_buckets.end (), b->first);
    }
  }

  const_iterator end () const
  {
    return cend ();
  }

  const_iterator cend () const
  {
    __unordered_container *nc_this = const_cast<__unordered_container *> (this);
    return const_iterator (nc_this->m_buckets.end (), nc_this->m_buckets.end (), 0);
  }

  bool empty () const
  {
    return m_size == 0;
  }

  void insert (const V &v)
  {
    __insert (v, m_h (v));
  }

  template <class I>
  void insert (const I &from, const I &to)
  {
    for (auto i = from; i != to; ++i) {
      __insert (*i, m_h (*i));
    }
  }

  void clear ()
  {
    __clear ();
  }

  void erase (iterator i)
  {
    __erase (*i.m_b, &i.mp_n->second);
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
    if (f.second) {
      return const_iterator (nc_this->m_buckets.begin () + f.first, nc_this->m_buckets.end (), f.second);
    } else {
      return end ();
    }
  }

  template <class X, class C>
  iterator __find (const X &x, const C &c)
  {
    auto f = __find_impl (x, m_h (x), c);
    if (f.second) {
      return iterator (m_buckets.begin () + f.first, m_buckets.end (), f.second);
    } else {
      return end ();
    }
  }

  void __insert (const V &v, size_t hash)
  {
    size_t ib;
    typename buckets_type::iterator bucket;

    if (m_buckets.empty ()) {
      ib = 0;
      m_buckets.push_back (bucket_type (0, 0));
      bucket = m_buckets.begin ();
    } else {
      ib = hash % m_buckets.size ();
      bucket = m_buckets.begin () + ib;
    }

    if (bucket->second >= max_bucket_size) {
      this->__split_buckets ();
      ib = hash % m_buckets.size ();
      bucket = m_buckets.begin () + ib;
    }

    node_type *n = new node_type (bucket->first, v);
    bucket->first = n;
    bucket->second += 1;

    m_size += 1;
  }

  void __erase (bucket_type &bucket, V *vp)
  {
    node_type **node = &bucket.first;
    while (*node != 0 && &((*node)->second) != vp) {
      node = &(*node)->first;
    }

    if (*node) {
      node_type *next_node = (*node)->first;
      delete *node;
      *node = next_node;
      m_size -= 1;
    }
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
    typedef typename __unordered_container::node_type node_type;

    __iterator_base ()
      : m_b (), m_e (), mp_n (0)
    {
    }

    bool operator== (const __iterator_base &other) const
    {
      return mp_n == other.mp_n;
    }

    bool operator!= (const __iterator_base &other) const
    {
      return mp_n != other.mp_n;
    }

    bool at_end () const
    {
      return ! mp_n;
    }

  protected:
    node_type *__node () const
    {
      return mp_n;
    }

    void __set (buckets_iterator b, buckets_iterator e, node_type *n)
    { 
      m_b = b;
      m_e = e;
      mp_n = n;
    }

    void __inc ()
    {
      if (mp_n) {
        mp_n = mp_n->first;
      }

      if (! mp_n) {
        ++m_b;
        if (m_b != m_e) {
          mp_n = m_b->first;
        }
      }
    }

  private:
    buckets_iterator m_b, m_e;
    node_type *mp_n;
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
      this->__set (other.m_b, other.m_e, other.mp_n);
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
      return this->__node ()->second;
    }

    pointer operator-> () const
    {
      return &this->__node ()->second;
    }

    __iterator operator++ ()
    {
      this->__inc ();
      return *this;
    }

  private:
    friend class __unordered_container;

    __iterator (typename __iterator_base::buckets_iterator b, typename __iterator_base::buckets_iterator e, node_type *n)
    { 
      this->__set (b, e, n);
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
      this->__set (other.m_b, other.m_e, other.mp_n);
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
      return this->__node ()->second;
    }

    pointer operator-> () const
    {
      return &this->__node ()->second;
    }

    __const_iterator operator++ ()
    {
      this->__inc ();
      return *this;
    }

  private:
    friend class __unordered_container;

    __const_iterator (typename __iterator_base::buckets_iterator b, typename __iterator_base::buckets_iterator e, node_type *n)
    { 
      this->__set (b, e, n);
    }
  };

  void __init ()
  {
    m_size = 0;
  }

  void __clear ()
  {
    for (auto b = m_buckets.begin (); b != m_buckets.end (); ++b) {

      node_type *node = b->first;
      b->first = 0;

      while (node != 0) {
        node_type *next_node = node->first;
        delete node;
        node = next_node;
      }

    }

    m_buckets.clear ();
    m_size = 0;
  }

  template <class C, class X>
  std::pair<size_t, node_type *> __find_impl (const X &x, size_t hash, const C &compare)
  {
    if (m_buckets.empty ()) {
      return std::pair<size_t, node_type *> (0, 0);
    }

    size_t ib = hash % m_buckets.size ();
    auto bucket = m_buckets.begin () + ib;

    node_type *node = bucket->first;
    while (node && ! compare (node->second, x)) {
      node = node->first;
    }

    return std::make_pair (ib, node);
  }

  void __split_buckets ()
  {
    buckets_type new_buckets;
    size_t new_buckets_size = m_buckets.size () * 2;

    new_buckets.resize (new_buckets_size, std::make_pair ((node_type *) 0, size_t (0)));

    for (auto b = m_buckets.begin (); b != m_buckets.end (); ++b) {

      node_type *node = b->first;
      b->first = 0;

      while (node != 0) {

        node_type *next_node = node->first;

        //  TODO: exception safety on hash function
        size_t h = m_h (node->second);
        size_t ib_new = h % new_buckets_size;

        auto new_bucket = new_buckets.begin () + ib_new;
        node_type **last_node_new = &new_buckets [h % new_buckets_size].first;

        node->first = new_bucket->first;
        new_bucket->first = node;
        new_bucket->second += 1;

        node = next_node;

      }

    }

    m_buckets.swap (new_buckets);
  }
};

}

#endif

