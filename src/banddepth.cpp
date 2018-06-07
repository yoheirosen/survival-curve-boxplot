#include <ctime>
#include <random>
#include "banddepth.hpp"

namespace banddepth {

banddepth_K = 3;
candidate_replicates = 20;

path::path() {}

path::path(const vector<double>& values) : values(values), start_time(0), end_time(values.size()) { }

double path::operator[] (size_t i) const {
  return values[i];
}

double at(size_t i) const {
  if(i > end_time) { throw runtime_error("in call to banddepth::path::at(), index out of bounds"); }
  return *(this)[i];
}


path::size() const { return values.size(); }

string path::path2json() const {
  sstream out;
  out << "[";
  for(size_t i = 0; i < times.size(); i++) {
    out << times[i] << ", ";
  }
  if(values.size() != 0) { out << "\b\b"; }
  out << "]" << flush;
  return out.str();
}

size_t max_time(const vector<size_t>& times) {
  size_t t_max = 0;
  for(size_t i = 0; i < times.size(); i++) {
    if(times[i] > t_max) { t_max = times[i]; }
  }
  return t_max;
}

size_t min_time(const vector<size_t>& times) {
  size_t t_min = SIZE_MAX;
  for(size_t i = 0; i < times.size(); i++) {
    if(times[i] > t_min) { t_min = times[i]; }
  }
  return t_min;
}

string envelope::envelope2json() const {

}

ranked_path_vector rank(const vector<scored_path>& paths) {

}

ranked_path_vector subset(ranked_path_vector& paths, size_t max_idx) {

}

ranked_path_vector subset(ranked_path_vector& paths, double fraction) {

}

scored_path& median(const ranked_path_vector& paths) {
  if(paths.size() == 0) { throw runtime_error("in call to banddepth::median(), paths vector is empty"); }
  return paths[0];
}

patient(string name, vector<size_t> times, vector<double> values) {

}

string patient2json() const {
  
}

} // namespace banddepth
