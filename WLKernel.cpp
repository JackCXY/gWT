/*
 * WLKernel.cpp
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

WLKernel::WLKernel(const WLKernel &gk) {
  iteration = 0;
  s2c       = gk.s2c;
  counter   = gk.counter;
}

int WLKernel::readfile(const char *fname) {
  ifstream ifs(fname);

  if (!ifs) {
    cerr << "cannot open: " << fname << endl;
    return 1;
  }

  int count = 0;
  while(!ifs.eof()) {
    graphs.resize(graphs.size() + 1);
    Graph &g = graphs[graphs.size() - 1];
    if (g.read(ifs)) {
      cerr << "file read error" << endl;
      cerr << count << endl;
      char line[1024];
      while (true) {
	if (!ifs.getline(line, 1024)) break;
	if (strlen(line) == 0) break;
      }
      graphs.resize(graphs.size() - 1);
      continue;
    }
  }
  ifs.close();
  
  return 0;
}

void WLKernel::initialize() {
  s2c.clear();
  counter   = 0;
  iteration = 0;
}

void WLKernel::initialTransfer() {
  map<string, uint64_t>::iterator iter;
  for (size_t i = 0; i < graphs.size(); ++i) {
    Graph &g = graphs[i];
    for (size_t j = 1; j < g.size(); j++) {
      stringstream ss;
      ss << iteration << "_" << g[j].vlabel;
      if ((iter = s2c.find(ss.str())) == s2c.end()) {
	s2c[ss.str()]   = counter;
	g[j].transLabel = counter;
	counter++;
      } else {
	g[j].transLabel = iter->second;
      } 
    }
  }
  for (size_t i = 0; i < graphs.size(); i++) {
    Graph &g = graphs[i];
    for (size_t j = 0; j < g.size(); j++) 
      g[j].vlabel = g[j].transLabel;
  }
}

void WLKernel::transferLabels() {
  iteration++;
  vector<pair<uint64_t, uint32_t> > labels;
  map<string, uint64_t>::iterator iter;
  for (size_t i = 0; i < graphs.size(); i++) {
    Graph &g = graphs[i];
    for (size_t j = 1; j < g.size(); j++) {
      labels.clear();
      labels.push_back(make_pair(g[j].vlabel, 0));
      for (edgeIterator it = g[j].edge.begin(); it != g[j].edge.end(); it++)
	labels.push_back(make_pair(g[(*it).to].vlabel, it->elabel));

      sort(labels.begin() + 1, labels.end());
      stringstream ss;
      ss << iteration;
      for (size_t i = 0; i < labels.size(); i++) 
	ss << "_" << labels[i].first << "_" << labels[i].second; 
      if ((iter = s2c.find(ss.str())) == s2c.end()) {
	s2c[ss.str()]   = counter;
	g[j].transLabel = counter;
	counter++;
      } else {
	g[j].transLabel = iter->second;
      }
    }
  }
  for (size_t i = 0; i < graphs.size(); i++) {
    Graph &g = graphs[i];
    for (size_t j = 0; j < g.size(); j++) 
      g[j].vlabel = g[j].transLabel;
  }
}

int WLKernel::save(ostream &os) {
  os.write((const char*)(&counter), sizeof(uint64_t));
  os.write((const char*)(&iteration), sizeof(uint32_t));
  
  size_t size = s2c.size();
  os.write((const char*)(&size), sizeof(size_t));

  for (std::map<std::string, uint64_t>::iterator it = s2c.begin(); it != s2c.end(); it++) {
    uint32_t   len = it->first.size();
    const char *word  = it->first.c_str();
    uint64_t   val = it->second;
    //      cout << word << endl;
    os.write((const char*)(&len), sizeof(uint32_t));
    os.write((const char*)(&word[0]), sizeof(char)*len);
    os.write((const char*)(&val), sizeof(uint64_t));
  }
  return 0;
}
  
int WLKernel::load(istream &is) {
  is.read((char*)(&counter), sizeof(uint64_t));
  is.read((char*)(&iteration), sizeof(uint32_t));
    
  size_t size;
  is.read((char*)(&size), sizeof(size_t));

  uint32_t len;
  string   word;
  uint64_t val;
  for (size_t i = 0; i < size; i++) {
    is.read((char*)(&len), sizeof(uint32_t));
    word.clear();
    word.resize(len);
    is.read((char*)(&word[0]), sizeof(char)*len);
    is.read((char*)(&val), sizeof(uint64_t));
    s2c[word] = val;
  }
  return 0;
}
}
