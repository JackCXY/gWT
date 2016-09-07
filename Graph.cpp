/*
 * Graph.cpp
 * Copyright (c) 2013 Yasuo Tabei All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE and * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "WLKernel.hpp"

using namespace std;

namespace GRAPHKERNEL {

template <class T, class Iterator> 
void tokenize (const char *str, Iterator iterator) 
{
  std::istrstream is (str, std::strlen(str));
  std::copy (std::istream_iterator <T> (is), std::istream_iterator <T> (), iterator);
}

int Graph::read(ifstream &ifs) {
  vector<string> result;
  char line[1024];
  clear();
  while (true) {
    unsigned int pos = ifs.tellg();
    if (!ifs.getline(line, 1024)) break;
    if (strlen(line) == 0) break;
    result.clear();
    tokenize<string>(line, back_inserter(result));
    if (result.empty()) {
      // do nothing
    } else if (result[0] == "t") {
      if (!empty()) {
	ifs.seekg(pos, ios_base::beg);
	break;
      }
    } else if (result[0] == "v" && result.size() >= 3) {
      unsigned int id = atoi(result[1].c_str());
      this->resize(id + 1);
      (*this)[id].vlabel = atoi(result[2].c_str());
      //      cout << id << " " << (*this)[id].vlabel << endl;
    } else if (result[0] == "e" && result.size() >= 4) {
      int from   = atoi(result[1].c_str());
      int to     = atoi(result[2].c_str());
      int elabel = atoi(result[3].c_str());
      if ((int)size() <= from || (int)size() <= to) {
	cerr << "Format Error: define lists before edges" << endl;
	return 1;
      }
      (*this)[from].push(from, to, elabel);
      (*this)[to].push(to, from, elabel);
    }
  }

  return 0;
}

}
