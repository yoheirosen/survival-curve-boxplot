#ifndef BAND_DEPTH_H
#define BAND_DEPTH_H

#include <vector>
#include <string>
#include <iostream>
#include <random>

using namespace std;

namespace random_helpers {
  struct random_helper {
    random_helper();
    double draw_uniform(double scale) const;
    double draw_truncated_normal(double upper_limit, double mean, double std_dev) const;
    double draw_normal(double mean, double std_dev) const;
  };
}

namespace banddepth {

static size_t banddepth_K;
static size_t samples;
static vector<double> envelope_levels;

struct path {
  const vector<double> values;
  const size_t start_time;
  const size_t end_time;
  path();
  path(const vector<double>& values);
  path(const vector<double>& values, size_t start_time);

  double operator[] (size_t i) const;
  double at(size_t i) const;
  size_t size() const;
  string path2json() const;
};

struct scored_path : public path {
  size_t depth = -1;
  void get_depth(const vector<path>& paths);
};

size_t max_time(const vector<size_t>& times);
size_t min_time(const vector<size_t>& times);

template<class pathType>
double max_time(const vector<pathType>& paths);
template<class pathType>
double min_time(const vector<pathType>& paths);

template<class pathType>
double max_value(const vector<pathType>& paths, size_t t);
template<class pathType>
double min_value(const vector<pathType>& paths, size_t t);
template<class pathType>
double mean_value(const vector<pathType>& paths, size_t t);

struct envelope {
  path upper;
  path lower;
  template<class pathType>
  envelope(const vector<pathType>& paths);
  template<class pathType>
  bool contains(pathType path);
  template<class pathType>
  bool contains(pathType path, double coverage);
  string envelope2json() const;
};

typedef vector<scored_path&> ranked_path_vector;

ranked_path_vector rank(const vector<scored_path>& paths);
ranked_path_vector subset(ranked_path_vector& paths, size_t max_idx);
ranked_path_vector subset(ranked_path_vector& paths, double fraction);

template<class pathType>
path pointwise_min(const vector<pathType>& paths);
template<class pathType>
path pointwise_max(const vector<pathType>& paths);
template<class pathType>
path pointwise_mean(const vector<pathType>& paths);

scored_path depth_median(const ranked_path_vector& paths);

static double measurement_error = 0;
static double starttime_error = 0;

template<class pathType>
vector<const pathType&> random_subsample(const vector<pathType>& paths, size_t exclude);

scored_path generate_sample(const vector<size_t>& times, const vector<double> values);

struct patient {
  string name;
  vector<size_t> times;
  vector<double> values;
  vector<scored_path> sampled_paths;
  vector<envelope> envelopes;
  path mean;
  scored_path median;
  patient(string name, vector<size_t> times, vector<double> values);
  string patient2json() const;
};

void generate_statistics(vector<string> names, vector<vector<size_t>> times, vector<vector<double>> values) {

}

template<class pathType>
size_t max_time(const vector<pathType>& paths) {
  size_t t_max = 0;
  for(size_t p = 0; p < paths.size(); p++) {
    if(paths[p].start_time > t_max) { t_min = paths[p].start_time; }
  }
  return t_max;
}

template<class pathType>
size_t min_time(const vector<pathType>& paths) {
  size_t t_min = SIZE_MAX;
  for(size_t p = 0; p < paths.size(); p++) {
    if(paths[p].end_time < t_min) { t_min = paths[p].end_time; }
  }
  return t_min;
}

template<class pathType>
double max_value(const vector<pathType>& paths, size_t t) {
  double v_max = 0;
  for(size_t p = 0; p < paths.size(); p++) {
    if(paths[p][t] > v_max) { v_max = paths[p][t]; }
  }
  return v_max;
}

template<class pathType>
double min_value(const vector<pathType>& paths, size_t t) {
  double v_min = max_value(paths, t);
  for(size_t p = 0; p < paths.size(); p++) {
    if(paths[p][t] < v_min) { v_min = paths[p][t]; }
  }
  return v_min;
}

template<class pathType>
double mean_value(const vector<pathType>& paths, size_t t) {
  double total = 0;
  for(size_t p = 0; p < paths.size(); p++) {
    total += paths[p][t];
  }
  return total / paths.size();
}

template<class pathType>
path pointwise_min(const vector<pathType>& paths) {

}

template<class pathType>
path pointwise_max(const vector<pathType>& paths) {

}

template<class pathType>
path pointwise_mean(const vector<pathType>& paths) {

}

template<class pathType>
envelope::envelope(const vector<pathType>& paths) {
  //TODO
}

template<class pathType>
bool envelope::contains(pathType path) {
  if(path.end_time > upper.end_time || path.start_time < upper.start_time) {
    throw runtime_error("in call to banddepth::envelope::contains(), path time domain not contained within envelope time domain");
  }
  for(size_t t = path.start_time; t <= path.end_time; t++) {
    if(path.at(t) < lower.at(t) || path.at(t) > upper.at(t)) {
      return false;
    }
  }
  return true;
}

template<class pathType>
bool envelope::contains(pathType path, double coverage) {
  if(coverage < 0 || coverage > 1) {
    throw runtime_error("in call to banddepth::envelope::contains(), coverage must be within [0,1]");
  }
  if(path.end_time > upper.end_time || path.start_time < upper.start_time) {
    throw runtime_error("in call to banddepth::envelope::contains(), path time domain not contained within envelope time domain");
  }
  size_t max_mismatches = (path.end_time - path.start_time) * coverage;
  size_t mismatches = 0;
  for(size_t t = path.start_time; t <= path.end_time; t++) {
    if(path.at(t) < lower.at(t) || path.at(t) > upper.at(t)) {
      mismatches++;
      if(mismatches > max_mismatches) { return false; }
    }
  }
  return true;
}

template<class pathType>
vector<const pathType&> random_subsample

void generate_statistics(vector<string> names, vector<vector<size_t>> times, vector<vector<double>> values) {

}(const vector<pathType>& paths, size_t exclude) {

}

} // namespace banddepth

#endif
