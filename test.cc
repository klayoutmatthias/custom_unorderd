
#include "unordered_set.h"
#include "unordered_map.h"

#include <unordered_set>
#include <unordered_map>

#include <stdio.h>
#include <iostream>
#include <string>

namespace tl
{

std::string to_string (int x)
{
  char b[32];
  sprintf (b, "%d", x);
  return std::string (b);
}

std::string to_string (size_t x)
{
  char b[32];
  sprintf (b, "%ld", (long)x);
  return std::string (b);
}

std::string to_string (const std::string &s)
{
  return s;
}

std::string to_string (const char *cp)
{
  return std::string (cp);
}

std::string to_string (bool f)
{
  return f ? std::string ("true") : std::string ("false");
}

}

static int errors = 0;

#define EXPECT_EQ(a, b) \
  if (a != b) { \
    std::cerr << __FILE__ << ":" << __LINE__ << " : expected " << tl::to_string(b) << ", got " << tl::to_string(a) << std::endl; \
    errors += 1; \
  }


template <class S>
std::string set_to_string (const S &set)
{
  std::string res;
  for (auto i = set.cbegin (); i != set.cend (); ++i) {
    if (i != set.cbegin ()) {
      res += ",";
    }
    res += tl::to_string (*i);
  }
  return res;
}

template <class S>
std::string set_to_string_nc (S &set)
{
  std::string res;
  for (auto i = set.begin (); i != set.end (); ++i) {
    if (i != set.begin ()) {
      res += ",";
    }
    res += tl::to_string (*i);
  }
  return res;
}

template <class S>
std::string map_to_string (const S &map)
{
  std::string res;
  for (auto i = map.cbegin (); i != map.cend (); ++i) {
    if (i != map.cbegin ()) {
      res += ",";
    }
    res += tl::to_string (i->first);
    res += ":";
    res += tl::to_string (i->second);
  }
  return res;
}

template <class S>
std::string map_to_string_nc (S &map)
{
  std::string res;
  for (auto i = map.begin (); i != map.end (); ++i) {
    if (i != map.begin ()) {
      res += ",";
    }
    res += tl::to_string (i->first);
    res += ":";
    res += tl::to_string (i->second);
  }
  return res;
}

//  basic set
void test_1 ()
{
  tl::unordered_set<int, tl::hash<int>, 4> set;
  tl::unordered_set<int, tl::hash<int>, 4> set2, set3;

  int data [] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

  //  Basic insert

  set.clear ();

  EXPECT_EQ(set.size(), 0);
  EXPECT_EQ(set_to_string(set), "");
  EXPECT_EQ(set.__buckets().size(), 0);
  set.insert (1);

  EXPECT_EQ(set.size(), 1)
  EXPECT_EQ(set_to_string(set), "1");
  EXPECT_EQ(set.__buckets().size(), 1);
  set.insert (2);

  EXPECT_EQ(set.size(), 2)
  EXPECT_EQ(set_to_string(set), "1,2");
  EXPECT_EQ(set.__buckets().size(), 1);

  set.insert (3);

  EXPECT_EQ(set.size(), 3)
  EXPECT_EQ(set_to_string(set), "1,2,3");
  EXPECT_EQ(set.__buckets().size(), 1);

  //  inserting the same thing again - no change
  set.insert (3);

  EXPECT_EQ(set.size(), 3)
  EXPECT_EQ(set_to_string(set), "1,2,3");
  EXPECT_EQ(set.__buckets().size(), 1);

  set.insert (4);

  EXPECT_EQ(set.size(), 4)
  EXPECT_EQ(set_to_string(set), "1,2,3,4");
  EXPECT_EQ(set.__buckets().size(), 1);

  set.insert (5);

  EXPECT_EQ(set.size(), 5)
  EXPECT_EQ(set_to_string(set), "2,4,1,3,5");
  EXPECT_EQ(set.__buckets().size(), 2);

  set.insert (6);
  set.insert (7);
  set.insert (8);
  set.insert (9);
  set.insert (10);

  EXPECT_EQ(set.size(), 10)
  EXPECT_EQ(set_to_string(set), "4,8,1,5,9,2,6,10,3,7");
  EXPECT_EQ(set_to_string_nc(set), "4,8,1,5,9,2,6,10,3,7");
  EXPECT_EQ(set.__buckets().size(), 4);

  //  assignment

  set = tl::unordered_set<int, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  set2.clear ();

  set2.insert (-1);
  EXPECT_EQ(set_to_string(set2), "-1");
  EXPECT_EQ(set2.size(), 1)

  set2 = set;
  EXPECT_EQ(set_to_string(set), "4,8,1,5,9,2,6,10,3,7");
  EXPECT_EQ(set.size(), 10)
  EXPECT_EQ(set.empty(), false)
  EXPECT_EQ(set_to_string(set2), "4,8,1,5,9,2,6,10,3,7");
  EXPECT_EQ(set2.size(), 10)
  EXPECT_EQ(set2.empty(), false)

  //  clear

  set = tl::unordered_set<int, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  EXPECT_EQ(set.size(), 10)

  set.clear ();

  EXPECT_EQ(set_to_string(set), "");
  EXPECT_EQ(set.size(), 0)
  EXPECT_EQ(set.empty(), true)

  //  move

  set = tl::unordered_set<int, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  set2.clear ();

  set2 = std::move (set);
  EXPECT_EQ(set_to_string(set), "");
  EXPECT_EQ(set.size(), 0)
  EXPECT_EQ(set_to_string(set2), "4,8,1,5,9,2,6,10,3,7");
  EXPECT_EQ(set2.size(), 10)

  //  swap

  set2 = tl::unordered_set<int, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  set.clear ();

  set2.swap (set);

  EXPECT_EQ(set_to_string(set), "4,8,1,5,9,2,6,10,3,7");
  EXPECT_EQ(set.size(), 10)
  EXPECT_EQ(set_to_string(set2), "");
  EXPECT_EQ(set2.size(), 0)

  //  assignment of empty

  set = tl::unordered_set<int, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  EXPECT_EQ(set.size(), 10)

  set = tl::unordered_set<int, tl::hash<int>, 4> ();

  EXPECT_EQ(set_to_string(set), "");
  EXPECT_EQ(set.size(), 0)
  EXPECT_EQ(set_to_string(set2), "");
  EXPECT_EQ(set2.size(), 0)

  //  creation from iterator

  set = tl::unordered_set<int, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  EXPECT_EQ(set_to_string(set), "4,8,1,5,9,2,6,10,3,7");
  EXPECT_EQ(set.size(), 10)

  //  copy constructor

  set = tl::unordered_set<int, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  set2.clear ();

  set2.clear ();
  EXPECT_EQ(set2.empty (), true);
  EXPECT_EQ(set2.size(), 0)
  set2 = tl::unordered_set<int, tl::hash<int>, 4> (set);
  EXPECT_EQ(set_to_string(set2), "4,8,1,5,9,2,6,10,3,7");
  EXPECT_EQ(set2.size(), 10)

  //  move constructor

  set = tl::unordered_set<int, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  set2.clear ();

  EXPECT_EQ(set2.empty (), true);
  EXPECT_EQ(set2.size(), 0)

  set2 = tl::unordered_set<int, tl::hash<int>, 4> (std::move (set));

  EXPECT_EQ(set_to_string(set2), "4,8,1,5,9,2,6,10,3,7");
  EXPECT_EQ(set2.size(), 10)
  EXPECT_EQ(set.empty (), true);
  EXPECT_EQ(set.size(), 0)

  //  set equality
  
  set = tl::unordered_set<int, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  set2.clear ();
  set3.clear ();
  
  EXPECT_EQ (set == set2, false);
  EXPECT_EQ (set != set2, true);

  set3 = set;
  EXPECT_EQ (set == set3, true);
  EXPECT_EQ (set == set, true);
  EXPECT_EQ (set != set3, false);
  EXPECT_EQ (set != set, false);

  set3.clear ();
  set3.insert (-1);
  EXPECT_EQ (set == set3, false);
  EXPECT_EQ (set != set3, true);

  //  set less operator
  
  set = tl::unordered_set<int, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  set2.clear ();
  set3.clear ();
  
  EXPECT_EQ (set < set2, false);
  EXPECT_EQ (set2 < set, true);
  EXPECT_EQ (set < set, false);

  set3 = set;
  EXPECT_EQ (set < set3, false);
  EXPECT_EQ (set3 < set, false);

  set3.clear ();
  set3.insert (-1);
  EXPECT_EQ (set < set3, false);
  EXPECT_EQ (set3 < set, true);

  set2.clear ();
  set2.insert (0);
  EXPECT_EQ (set2 < set3, false);
  EXPECT_EQ (set3 < set2, true);

  set2.clear ();
  set2.insert (-1);
  EXPECT_EQ (set2 < set3, false);
  EXPECT_EQ (set3 < set2, false);

  //  find

  set = tl::unordered_set<int, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  EXPECT_EQ(set_to_string(set), "4,8,1,5,9,2,6,10,3,7");

  for (int i = 1; i <= 10; ++i) {
    auto f = set.find (i);
    EXPECT_EQ (f != set.end (), true);
    EXPECT_EQ (*f, i);
  }

  {
    auto f = set.cfind (-1);
    EXPECT_EQ (f != set.end (), false);
  }

  //  erase

  set = tl::unordered_set<int, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  EXPECT_EQ(set_to_string(set), "4,8,1,5,9,2,6,10,3,7");

  set.erase (5);
  EXPECT_EQ(set_to_string(set), "4,8,1,9,2,6,10,3,7");
  EXPECT_EQ(set.size(), 9);

  set.erase (1);
  set.erase (10);
  set.erase (9);
  set.erase (8);
  set.erase (2);
  set.erase (3);
  set.erase (4);
  set.erase (6);
  EXPECT_EQ(set_to_string(set), "7");
  EXPECT_EQ(set.size(), 1);

  set.erase (7);
  EXPECT_EQ(set_to_string(set), "");
  EXPECT_EQ(set.size(), 0);

  set.insert (-1);
  EXPECT_EQ(set_to_string(set), "-1");
  EXPECT_EQ(set.size(), 1);
  set.erase (set.begin ());
  EXPECT_EQ(set_to_string(set), "");
  EXPECT_EQ(set.size(), 0);
}

const int n_perf = 1000000;

//  performance insert
void test_2()
{
  tl::unordered_set<std::string> set;

  for (int i = 0; i < n_perf; ++i) {
    set.insert (tl::to_string (i));
  }
}

//  performance find
void test_3()
{
  tl::unordered_set<std::string> set;

  for (int i = 0; i < n_perf; ++i) {
    set.insert (tl::to_string (i));
  }

  //  find
  for (int i = 0; i < n_perf; ++i) {
    set.find (tl::to_string (i));
  }
}

//  performance insert again
void test_4()
{
  tl::unordered_set<std::string> set;

  for (int i = 0; i < n_perf; ++i) {
    set.insert (tl::to_string (i));
  }

  for (int i = 0; i < n_perf; ++i) {
    set.insert (tl::to_string (i));
  }
}

//  performance insert std::unordered_set
void test_5()
{
  std::unordered_set<std::string> set;

  for (int i = 0; i < n_perf; ++i) {
    set.insert (tl::to_string (i));
  }
}

//  performance find std::unordered_set
void test_6()
{
  std::unordered_set<std::string> set;

  for (int i = 0; i < n_perf; ++i) {
    set.insert (tl::to_string (i));
  }

  //  find
  for (int i = 0; i < n_perf; ++i) {
    set.find (tl::to_string (i));
  }
}

//  performance insert again std::unordered_set
void test_7()
{
  std::unordered_set<std::string> set;

  for (int i = 0; i < n_perf; ++i) {
    set.insert (tl::to_string (i));
  }

  for (int i = 0; i < n_perf; ++i) {
    set.insert (tl::to_string (i));
  }
}

void test_8()
{
  tl::unordered_map<int, std::string, tl::hash<int>, 4> map;
  tl::unordered_map<int, std::string, tl::hash<int>, 4> map2, map3;

  std::pair<int, std::string> data [] = { 
    std::make_pair (1, "1"),
    std::make_pair (2, "2"),
    std::make_pair (3, "3"),
    std::make_pair (4, "4"),
    std::make_pair (5, "5"),
    std::make_pair (6, "6"),
    std::make_pair (7, "7"),
    std::make_pair (8, "8"),
    std::make_pair (9, "9"),
    std::make_pair (10, "10")
  };

  //  Basic insert

  map.clear ();

  EXPECT_EQ(map.size(), 0);
  EXPECT_EQ(map_to_string(map), "");
  EXPECT_EQ(map.__buckets().size(), 0);
  map.insert (std::make_pair (1, "1"));

  EXPECT_EQ(map.size(), 1)
  EXPECT_EQ(map_to_string(map), "1:1");
  EXPECT_EQ(map.__buckets().size(), 1);
  map.insert (std::make_pair (2, "2"));

  EXPECT_EQ(map.size(), 2)
  EXPECT_EQ(map_to_string(map), "1:1,2:2");
  EXPECT_EQ(map.__buckets().size(), 1);

  map.insert (std::make_pair (3, "3"));

  EXPECT_EQ(map.size(), 3)
  EXPECT_EQ(map_to_string(map), "1:1,2:2,3:3");
  EXPECT_EQ(map.__buckets().size(), 1);

  //  inserting the same thing again - no change
  map.insert (std::make_pair (3, "33"));

  EXPECT_EQ(map.size(), 3)
  EXPECT_EQ(map_to_string(map), "1:1,2:2,3:3");
  EXPECT_EQ(map.__buckets().size(), 1);

  map.insert (std::make_pair (4, "4"));

  EXPECT_EQ(map.size(), 4)
  EXPECT_EQ(map_to_string(map), "1:1,2:2,3:3,4:4");
  EXPECT_EQ(map.__buckets().size(), 1);

  map.insert (std::make_pair (5, "5"));

  EXPECT_EQ(map.size(), 5)
  EXPECT_EQ(map_to_string(map), "2:2,4:4,1:1,3:3,5:5");
  EXPECT_EQ(map.__buckets().size(), 2);

  map.insert (std::make_pair (6, "6"));
  map.insert (std::make_pair (7, "7"));
  map.insert (std::make_pair (8, "8"));
  map.insert (std::make_pair (9, "9"));
  map.insert (std::make_pair (10, "10"));

  EXPECT_EQ(map.size(), 10)
  EXPECT_EQ(map_to_string(map), "4:4,8:8,1:1,5:5,9:9,2:2,6:6,10:10,3:3,7:7");
  EXPECT_EQ(map_to_string_nc(map), "4:4,8:8,1:1,5:5,9:9,2:2,6:6,10:10,3:3,7:7");
  EXPECT_EQ(map.__buckets().size(), 4);

  //  assignment

  map = tl::unordered_map<int, std::string, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  map2.clear ();

  map2.insert (std::make_pair (-1, "nil"));
  EXPECT_EQ(map_to_string(map2), "-1:nil");
  EXPECT_EQ(map2.size(), 1)

  map2 = map;
  EXPECT_EQ(map_to_string(map), "4:4,8:8,1:1,5:5,9:9,2:2,6:6,10:10,3:3,7:7");
  EXPECT_EQ(map.size(), 10)
  EXPECT_EQ(map.empty(), false)
  EXPECT_EQ(map_to_string(map2), "4:4,8:8,1:1,5:5,9:9,2:2,6:6,10:10,3:3,7:7");
  EXPECT_EQ(map2.size(), 10)
  EXPECT_EQ(map2.empty(), false)

  //  clear

  map = tl::unordered_map<int, std::string, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  EXPECT_EQ(map.size(), 10)

  map.clear ();

  EXPECT_EQ(map_to_string(map), "");
  EXPECT_EQ(map.size(), 0)
  EXPECT_EQ(map.empty(), true)

  //  move

  map = tl::unordered_map<int, std::string, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  map2.clear ();

  map2 = std::move (map);
  EXPECT_EQ(map_to_string(map), "");
  EXPECT_EQ(map.size(), 0)
  EXPECT_EQ(map_to_string(map2), "4:4,8:8,1:1,5:5,9:9,2:2,6:6,10:10,3:3,7:7");
  EXPECT_EQ(map2.size(), 10)

  //  swap

  map2 = tl::unordered_map<int, std::string, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  map.clear ();

  map2.swap (map);

  EXPECT_EQ(map_to_string(map), "4:4,8:8,1:1,5:5,9:9,2:2,6:6,10:10,3:3,7:7");
  EXPECT_EQ(map.size(), 10)
  EXPECT_EQ(map_to_string(map2), "");
  EXPECT_EQ(map2.size(), 0)

  //  assignment of empty

  map = tl::unordered_map<int, std::string, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  EXPECT_EQ(map.size(), 10)

  map = tl::unordered_map<int, std::string, tl::hash<int>, 4> ();

  EXPECT_EQ(map_to_string(map), "");
  EXPECT_EQ(map.size(), 0)
  EXPECT_EQ(map_to_string(map2), "");
  EXPECT_EQ(map2.size(), 0)

  //  creation from iterator

  map = tl::unordered_map<int, std::string, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  EXPECT_EQ(map_to_string(map), "4:4,8:8,1:1,5:5,9:9,2:2,6:6,10:10,3:3,7:7");
  EXPECT_EQ(map.size(), 10)

  //  copy constructor

  map = tl::unordered_map<int, std::string, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  map2.clear ();

  map2.clear ();
  EXPECT_EQ(map2.empty (), true);
  EXPECT_EQ(map2.size(), 0)
  map2 = tl::unordered_map<int, std::string, tl::hash<int>, 4> (map);
  EXPECT_EQ(map_to_string(map2), "4:4,8:8,1:1,5:5,9:9,2:2,6:6,10:10,3:3,7:7");
  EXPECT_EQ(map2.size(), 10)

  //  move constructor

  map = tl::unordered_map<int, std::string, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  map2.clear ();

  EXPECT_EQ(map2.empty (), true);
  EXPECT_EQ(map2.size(), 0)

  map2 = tl::unordered_map<int, std::string, tl::hash<int>, 4> (std::move (map));

  EXPECT_EQ(map_to_string(map2), "4:4,8:8,1:1,5:5,9:9,2:2,6:6,10:10,3:3,7:7");
  EXPECT_EQ(map2.size(), 10)
  EXPECT_EQ(map.empty (), true);
  EXPECT_EQ(map.size(), 0)

  //  map equality
  
  map = tl::unordered_map<int, std::string, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  map2.clear ();
  map3.clear ();
  
  EXPECT_EQ (map == map2, false);
  EXPECT_EQ (map != map2, true);

  map3 = map;
  EXPECT_EQ (map == map3, true);
  EXPECT_EQ (map == map, true);
  EXPECT_EQ (map != map3, false);
  EXPECT_EQ (map != map, false);

  map3.clear ();
  map3.insert (std::make_pair (-1, "nil"));
  EXPECT_EQ (map == map3, false);
  EXPECT_EQ (map != map3, true);

  //  map less operator
  
  map = tl::unordered_map<int, std::string, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  map2.clear ();
  map3.clear ();
  
  EXPECT_EQ (map < map2, false);
  EXPECT_EQ (map2 < map, true);
  EXPECT_EQ (map < map, false);

  map3 = map;
  EXPECT_EQ (map < map3, false);
  EXPECT_EQ (map3 < map, false);

  map3.clear ();
  map3.insert (std::make_pair (-1, "nil"));
  EXPECT_EQ (map < map3, false);
  EXPECT_EQ (map3 < map, true);

  map2.clear ();
  map2.insert (std::make_pair (0, ""));
  EXPECT_EQ (map2 < map3, false);
  EXPECT_EQ (map3 < map2, true);

  map2.clear ();
  map2.insert (std::make_pair (-1, "nil"));
  EXPECT_EQ (map2 < map3, false);
  EXPECT_EQ (map3 < map2, false);

  //  find

  map = tl::unordered_map<int, std::string, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  EXPECT_EQ(map_to_string(map), "4:4,8:8,1:1,5:5,9:9,2:2,6:6,10:10,3:3,7:7");

  for (int i = 1; i <= 10; ++i) {
    auto f = map.find (i);
    EXPECT_EQ (f != map.end (), true);
    EXPECT_EQ (f->first, i);
    EXPECT_EQ (f->second, tl::to_string (i));
  }

  {
    auto f = map.cfind (-1);
    EXPECT_EQ (f != map.end (), false);
  }

  //  erase

  map = tl::unordered_map<int, std::string, tl::hash<int>, 4> (&data[0], &data[sizeof (data) / sizeof (data [0])]);
  EXPECT_EQ(map_to_string(map), "4:4,8:8,1:1,5:5,9:9,2:2,6:6,10:10,3:3,7:7");

  map.erase (5);
  EXPECT_EQ(map_to_string(map), "4:4,8:8,1:1,9:9,2:2,6:6,10:10,3:3,7:7");
  EXPECT_EQ(map.size(), 9);

  map.erase (1);
  map.erase (10);
  map.erase (9);
  map.erase (8);
  map.erase (2);
  map.erase (3);
  map.erase (4);
  map.erase (6);
  EXPECT_EQ(map_to_string(map), "7:7");
  EXPECT_EQ(map.size(), 1);

  map.erase (7);
  EXPECT_EQ(map_to_string(map), "");
  EXPECT_EQ(map.size(), 0);

  map.insert (std::make_pair (-1, "nil"));
  EXPECT_EQ(map_to_string(map), "-1:nil");
  EXPECT_EQ(map.size(), 1);
  map.erase (map.begin ());
  EXPECT_EQ(map_to_string(map), "");
  EXPECT_EQ(map.size(), 0);
}

//  performance insert
void test_9()
{
  tl::unordered_map<std::string, int> map;

  for (int i = 0; i < n_perf; ++i) {
    map.insert (std::make_pair (tl::to_string (i), i));
  }
}

//  performance find
void test_10()
{
  tl::unordered_map<std::string, int> map;

  for (int i = 0; i < n_perf; ++i) {
    map.insert (std::make_pair (tl::to_string (i), i));
  }

  //  find
  for (int i = 0; i < n_perf; ++i) {
    map.find (tl::to_string (i));
  }
}

//  performance insert again
void test_11()
{
  tl::unordered_map<std::string, int> map;

  for (int i = 0; i < n_perf; ++i) {
    map.insert (std::make_pair (tl::to_string (i), i));
  }

  for (int i = 0; i < n_perf; ++i) {
    map.insert (std::make_pair (tl::to_string (i), i));
  }
}

//  performance insert std::unordered_map
void test_12()
{
  std::unordered_map<std::string, int> map;

  for (int i = 0; i < n_perf; ++i) {
    map.insert (std::make_pair (tl::to_string (i), i));
  }
}

//  performance find std::unordered_map
void test_13()
{
  std::unordered_map<std::string, int> map;

  for (int i = 0; i < n_perf; ++i) {
    map.insert (std::make_pair (tl::to_string (i), i));
  }

  //  find
  for (int i = 0; i < n_perf; ++i) {
    map.find (tl::to_string (i));
  }
}

//  performance insert again std::unordered_map
void test_14()
{
  std::unordered_map<std::string, int> map;

  for (int i = 0; i < n_perf; ++i) {
    map.insert (std::make_pair (tl::to_string (i), i));
  }

  for (int i = 0; i < n_perf; ++i) {
    map.insert (std::make_pair (tl::to_string (i), i));
  }
}

int main(int argc, char **argv)
{
  errors = 0;
  void (*tests[])() = { &test_1, &test_2, &test_3, &test_4, &test_5, &test_6, &test_7, &test_8, &test_9, &test_10, &test_11, &test_12, &test_13, &test_14 };
  size_t ntests = sizeof (tests) / sizeof (tests[0]);

  bool all = false;
  if (argc == 1) {
    for (size_t t = 0; t < ntests; ++t) {
      std::cout << "Running test " << (t + 1) << " .." << std::endl;
      (*tests[t]) ();
    }
  } else {
    for (int i = 1; i < argc; ++i) {
      size_t t = size_t(atoi(argv[i]) - 1);
      if (t < ntests) {
        std::cout << "Running test " << (t + 1) << " .." << std::endl;
        (*tests[t]) ();
      }
    }
  }

  return errors;
}


