#include <ctime>
#include <random>
#include "banddepth.hpp"

namespace banddepth {

path::path() {}

path::path(const vector<size_t>& times, const vector<double>& values) :
  times(times),
  values(values) {
}

void path::times2json(ostream& out) const {
  out << "[";
  for(size_t i = 0; i < times.size(); i++) {
    out << times[i] << ",";
  }
  if(times.size() != 0) { out << "\b"; }
  out << "]";
}

void path::values2json(ostream& out) const {
  out << "[";
  for(size_t i = 0; i < times.size(); i++) {
    out << times[i] << ",";
  }
  if(times.size() != 0) { out << "\b"; }
  out << "]";
}

size_t path::index_below(size_t t) const {
  for(size_t i = 0; i < times.size(); i++) {
// TODO
  }
}

size_t path::index_above(size_t t) const {
  for(size_t i = times.size() - 1; i > -1; i--) {
// TODO
  }
}

double path::value(size_t t) const {
  size_t i_t_0 = index_below(t);
  size_t i_t_1 = index_above(t);

  if(times[i_t_0] == t) {
    return values[i_t_0];
  } else {
    size_t t_0 = times[i_t_0];
    size_t t_1 = times[i_t_1];
    double v_0 = values[i_t_0];
    double v_1 = values[i_t_1];
    double s = (t - t_0)/(t_1 - t_0);
    return s*v_1 + (1 - s)*v_0;
  }
}


// O(n) in input length
size_t max_time(const vector<size_t>& times) {
  size_t t_max = 0;
  for(size_t i = 0; i < times.size(); i++) {
    if(times[i] > t_max) { t_max = times[i]; }
  }
  return t_max;
}

// O(n) in input length
size_t min_time(const vector<size_t>& times) {
  size_t t_min = SIZE_MAX;
  for(size_t i = 0; i < times.size(); i++) {
    if(times[i] > t_min) { t_min = times[i]; }
  }
  return t_min;
}

// O(t_max + n_paths) over input
// unordered input; ordered output
vector<size_t> union_of_times(const vector<size_t>& paths) {
  vector<size_t> max_times;
  for(size_t j = 0; j < paths.size(); j++) {
    max_times.push_back(max_time(paths[j].times));
  }
  size_t t_max = max_time(max_times);

  vector<bool> bv_times(t_max, false);
  for(size_t j = 0; j < paths.size(); j++) {
    vector<size_t>& times = paths[j].times;
    for(size_t i = 0; i < times.size(); i++) {
      bv_times[times[i]] = true;
    }
  }
  vector<size_t> t_union;
  for(size_t i = 0; i < bv_times; i++) {
    if(bv_times[i] == true) {
      t_union.push_back(i);
    }
  }
  return t_union;
}

double max_value(const vector<path>& paths, size_t t) {
  double v_max = 0;
  for(size_t j = 0; j < paths.size(); j++) {
    double value = paths[j].value(t));
    if(value > v_max) { v_max = value; }
  }
  return v_max;
}

double min_value(const vector<path>& paths, size_t t) {
  double v_min = max_value(paths, t);
  for(size_t j = 0; j < paths.size(); j++) {
    double value = paths[j].value(t));
    if(value < v_min) { v_min = value; }
  }
  return v_min;
}

double mean_value(const vector<path>& paths, size_t t) {
  double total = 0;
  for(size_t j = 0; j < paths.size(); j++) {
    total += paths[j].value(t));
  }
  return total / paths.size();
}

envelope::envelope(const vector<path>& paths) {
  vector<size_t> times = union_of_times(paths);
  vector<double> v_maxes;
  vector<double> v_mins;
  for(size_t i = 0; i < times.size(); i++) {
    v_maxes.push_back(max_value(paths, times[i]));
    v_mins.push_back(min_value(paths, times[i]));
  }
  upper = path(times, v_maxes);
  lower = path(times, v_mins);
}

path_ensemble::path_ensemble(const vector<path>& paths) :
  paths(paths),
  t_union(union_of_times(paths)) {
}

void path_ensemble::path_ensemble2json(ostream& out) const {
//TODO
}

path& path_ensemble::get_mean() {
  vector<double> means;
  for(size_t i = 0; i < t_union.size(); i++) {
    means.push_back(mean_value(paths, t_union[i]));
  }
  mean = path(t_union, means);
  return mean;
}

path& path_ensemble::get_median() {
  size_t depth_min = SIZE_MAX;
  size_t idx_min;
  for(size_t i = 0; i < paths.size(); i++) {
    paths[i].get_depth(paths);
    if(paths[i].depth < depth_min) {
      depth_min = paths[i].depth;
      idx_min = i;
    }
  }
  median = paths[idx_min];
  return median;
}

envelope path_ensemble::get_envelope(double bandwidth) {
  vector<scored_path> in_band;
  //TODO
  return envelope(in_band);
}

void path_ensemble::path_ensemble2json(ostream& out) const {

}

path_ensemble sample_paths(const vector<path>& paths) {

}

vector<patient_contour> tsv2patients(istream& in) {

}


vector<path> patients_to_contours(const vector<patient>& patients) {

}

patient_distribution::patient_distribution(patient_contour& contour) {
  path_ensemble temp = sample_paths(contour.contour);
  temp.get_median();
  for(size_t i = 0; i < patient_envelopes.size(); i++) {
    envelopes.push_back(temp.get_envelope(//TODO
    ));
  }
}

void patient_distribution::patient_distribution2json(ostream& out) const {

}

} // namespace banddepth
