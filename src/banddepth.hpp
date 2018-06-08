#ifndef BAND_DEPTH_H
#define BAND_DEPTH_H

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <random>

using namespace std;

double draw_uniform(double scale);
double draw_truncated_normal(double upper_limit, double lower_limit, double mean, double std_dev);
double draw_normal(double mean, double std_dev);
size_t random_index(size_t supremum, size_t exclude);

namespace banddepth {

struct path {
  vector<double> values;
  size_t start_time;
  size_t end_time;
  path(const vector<double>& values);
  path(const vector<double>& values, size_t start_time);

  double operator[] (size_t i) const;
  double at(size_t i) const;
  size_t size() const;
  string path2json() const;
};

struct scored_path : public path {
  size_t depth = -1;
  scored_path(const vector<double>& values) : path(values) {}
  scored_path(const vector<double>& values, size_t start_time) : path(values, start_time) {}
  bool operator==(const scored_path& other) const { return depth == other.depth; }
  bool operator<(const scored_path& other) const { return depth < other.depth; }
  void get_depth(const vector<path>& paths);
};

size_t max_time(const vector<size_t>& times);
size_t min_time(const vector<size_t>& times);

template<class pathType>
size_t max_end_time(const vector<pathType>& paths);
template<class pathType>
size_t min_start_time(const vector<pathType>& paths);

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

typedef vector<scored_path> ranked_path_vector;

ranked_path_vector& rank_paths(vector<scored_path>& paths);
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
size_t max_end_time(const vector<pathType>& paths) {
  size_t t_max = 0;
  for(size_t p = 0; p < paths.size(); p++) {
    if(paths[p].end_time > t_max) { t_max = paths[p].end_time; }
  }
  return t_max;
}

template<class pathType>
size_t min_start_time(const vector<pathType>& paths) {
  size_t t_min = SIZE_MAX;
  for(size_t p = 0; p < paths.size(); p++) {
    if(paths[p].start_time < t_min) { t_min = paths[p].start_time; }
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
  size_t t_0 = min_start_time(paths);
  size_t t_1 = max_end_time(paths);
  vector<double> points(t_1 - t_0 + 1, 0);
  double v_max = max_value(paths, 0);
  for(size_t t = t_0; t <= t_1; t++) {
    points[t] = v_max;
    for(size_t p = 0; p < paths.size(); p++) {
      if(t >= paths[p].start_time && t <= paths[p].end_time) {
        if(paths[p][t] < points[t]) { points[t] = paths[p][t]; }
      }
    }
  }
  return path(points, t_0);
}

template<class pathType>
path pointwise_max(const vector<pathType>& paths) {
  size_t t_0 = min_start_time(paths);
  size_t t_1 = max_end_time(paths);
  vector<double> points(t_1 - t_0 + 1, 0);
  for(size_t t = t_0; t <= t_1; t++) {
    points[t] = 0;
    for(size_t p = 0; p < paths.size(); p++) {
      if(t >= paths[p].start_time && t <= paths[p].end_time) {
        if(paths[p][t] > points[t]) { points[t] = paths[p][t]; }
      }
    }
  }
  return path(points, t_0);
}

template<class pathType>
path pointwise_mean(const vector<pathType>& paths) {
  size_t t_0 = min_start_time(paths);
  size_t t_1 = max_end_time(paths);
  vector<double> points(t_1 - t_0 + 1, 0);
  for(size_t t = t_0; t <= t_1; t++) {
    size_t n = 0;
    for(size_t p = 0; p < paths.size(); p++) {
      if(t >= paths[p].start_time && t <= paths[p].end_time) {
        n++;
        points[t] += paths[p].at(t);
      }
    }
    points[t] = points[t] / n;
  }
  return path(points, t_0);
}

template<class pathType>
envelope::envelope(const vector<pathType>& paths) : upper(pointwise_max(paths)), lower(pointwise_min(paths)) {}

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
  size_t max_mismatches = ((path.end_time - path.start_time) * coverage) + 1;
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
vector<pathType> random_subsample(const vector<pathType>& paths, size_t exclude) {
  size_t sampled = 0;
  vector<size_t> draws;
  while(sampled < banddepth_K) {
    size_t draw = random_index(paths.size() - 1, exclude);
    bool already_drawn = false;
    for(size_t d = 0; d < draws.size(); d++) {
      if(draw == draws[d]) { already_drawn = true; }
    }
    if(already_drawn == false) {
      draws.push_back(draw);
      ++sampled;
    }
  }
  vector<pathType> subsample;
  for(size_t p = 0; p < draws.size(); p++) {
    subsample.push_back(paths[draws[p]]);
  }
  return subsample;
}

void generate_statistics(vector<string> names, vector<vector<size_t>> times, vector<vector<double>> values) {

}

} // namespace banddepth

#endif
