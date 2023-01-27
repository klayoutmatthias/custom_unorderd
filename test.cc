
#include "unordered_set.h"
#include "unordered_map.h"

#include <stdio.h>
#include <string>

int main ()
{
  tl::unordered_set<int> set;

  printf("%d", int (set.size ()));
  set.insert (1);

  printf("%d", int (set.size ()));
  set.insert (2);

  printf("%d", int (set.size ()));

  tl::unordered_map<int, std::string> map;

  printf("%d", int (map.size ()));
  map.insert (std::make_pair (1, "a"));

  return 0;
}

