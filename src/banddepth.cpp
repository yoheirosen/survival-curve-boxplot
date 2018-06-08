#include <chrono>
#include <random>
#include <algorithm>
#include "banddepth.hpp"

size_t patient::samples = 20;
double patient::standard_deviation = 1;
vector<double> patient::envelope_levels = {0.5, 0.75, 0.95};
double patient::upper_limit = 30;
size_t patient::banddepth_K = 3;

double draw_uniform(double scale) {
  default_random_engine generator;
  generator.seed(chrono::system_clock::now().time_since_epoch().count());
  uniform_real_distribution<double> uniform(0.0, 1.0);
  return uniform(generator) * scale;
}

double draw_truncated_normal(double upper_limit, double lower_limit, double mean, double std_dev) {
  default_random_engine generator;
  generator.seed(chrono::system_clock::now().time_since_epoch().count());
  normal_distribution<double> normal(mean, std_dev);
  while(true) {
    double drawn = normal(generator);
    if(drawn <= upper_limit && drawn >= lower_limit) { return drawn; }
  }
}

double draw_normal(double mean, double std_dev) {
  default_random_engine generator;
  generator.seed(chrono::system_clock::now().time_since_epoch().count());
  normal_distribution<double> normal(mean, std_dev);
  return normal(generator);
}

size_t random_index(size_t supremum, size_t exclude) {
  default_random_engine generator;
  generator.seed(chrono::system_clock::now().time_since_epoch().count());
  uniform_real_distribution<double> unit_uniform(0.0, 1.0);
  size_t draw = (size_t)(unit_uniform(generator) * supremum);
  if(draw == supremum) { draw = supremum - 1; }
  if(draw >= exclude) { draw++; }
  return draw;
}

path::path(const vector<double>& values) : values(values), start_time(0), end_time(values.size() - 1 + start_time) { }

path::path(const vector<double>& values, size_t start_time) : values(values), start_time(start_time), end_time(values.size() - 1 + start_time) { }

double path::operator[] (size_t i) const {
  return values[i];
}

double path::at(size_t i) const {
  if(i > end_time) { throw runtime_error("in call to banddepth::path::at(), index out of bounds"); }
  return values[i];
}

size_t path::size() const { return values.size(); }

string path::path2json() const {
  stringstream out;
  out << "{\n" << R"("values" : )";
  out << "[";
  for(size_t v = 0; v < values.size(); v++) {
    out << values[v] << ", ";
  }
  out << "\b\b],";
  out << "\n" << R"("start_time" : )" << start_time
      << ",\n" << R"("end_time" : )" << end_time
      << "\n}" << flush;
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
    if(times[i] < t_min) { t_min = times[i]; }
  }
  return t_min;
}

string envelope::envelope2json() const {
  stringstream out;
  out << "{\n" << R"("upper" : )" << upper.path2json()
      << ",\n" << R"("lower" : )" << lower.path2json()
      << "\n}" << flush;
  return out.str();
}

ranked_path_vector& rank_paths(vector<scored_path>& paths) {
  sort(paths.begin(), paths.end());
  return paths;
}

ranked_path_vector subset(ranked_path_vector& paths, size_t max_idx) {
  ranked_path_vector ranked;
  for(size_t p = 0; p <= max_idx; p++) {
    ranked.push_back(paths[p]);
  }
  return ranked;
}

ranked_path_vector subset(ranked_path_vector& paths, double fraction) {
  if(fraction < 0 || fraction > 1) { throw runtime_error("in call to banddepth::subset, subset fraction must be in [0,1]"); }
  size_t max_idx = (paths.size() - 1) * fraction;
  return subset(paths, max_idx);
}

scored_path depth_median(const ranked_path_vector& paths) {
  if(paths.size() == 0) { throw runtime_error("in call to banddepth::median(), paths vector is empty"); }
  return paths[0];
}

scored_path generate_sample(const vector<size_t>& times, const vector<double> values) {
  vector<double> v(times.back() + 1);
  v[times[0]] = draw_truncated_normal(patient::upper_limit, 0, values[0], patient::standard_deviation);
  for(size_t i = 1; i < times.size(); i++) {
    v[times[i]] = draw_truncated_normal(v[times[i - 1]], 0, values[i], patient::standard_deviation);
  }

  for(size_t i = 0; i < times.size() - 1; i++) {
    size_t t_0 = times[i];
    size_t t_1 = times[i + 1];
    for(size_t t = t_0 + 1; t < t_1; t++) {
      v[t] = v[t - 1] - draw_uniform(v[t - 1] - v[t_1]);
    }
  }

  return scored_path(v);
}

patient::patient(string name, vector<size_t> times, vector<double> values) : name(name), times(times), values(values) {
  for(size_t p = 0; p < samples; p++) {
    sampled_paths.push_back(generate_sample(times, values));
  }
  
  for(size_t p = 0; p < sampled_paths.size(); p++) {
    sampled_paths[p].depth = 0;
    for(size_t r = 0; r < samples * 4; r++) {
      vector<scored_path> band = random_subsample(sampled_paths, p);
      envelope band_envelope(band);
      sampled_paths[p].depth += band_envelope.contains(sampled_paths[p]);
    }
  }
  
  rank_paths(sampled_paths);
  mean = pointwise_mean(sampled_paths);
  median = depth_median(sampled_paths);
  for(size_t e = 0; e < envelope_levels.size(); e++) {
    envelopes.push_back(envelope(subset(sampled_paths, envelope_levels.at(e))));
  }
}

string patient::patient2json() const {
  stringstream out;
  out << "{\n" << R"("name" : )" << name
      << ",\n" << R"("median" : )" << median.path2json()
      << ",\n" << R"("mean" : )" << mean.path2json()
      << ",\n" << R"("envelopes" : [)" << flush;
  for(size_t e = 0; e < envelopes.size(); e++) {
    out << envelopes[e].envelope2json();
    out << "\n,";
  }
  out << "\b\n]\n}" << flush;
  return out.str();
}

void generate_statistics(vector<string> names, vector<vector<size_t>> times, vector<vector<double>> values, ostream& out) {
  if(names.size() != times.size() || times.size() != values.size()) {
    throw runtime_error("in call to banddepth::generate_statistics, input lengths mismatched");
  }
  if(names.size() == 0) { throw runtime_error("in call to banddepth::generate_statistics, empty input"); }
  for(size_t i = 0; i < names.size(); i++) {
    if(names[i].empty()) { names[i] += "empty_name"; names[i] += to_string(i); }
  }

  vector<patient> patients;
  patients.reserve(names.size());

  for(size_t p = 0; p < names.size(); p++) {
    patients.push_back(patient(names[p], times[p], values[p]));
  }

  vector<scored_path> aggregate_paths;
  for(size_t p = 0; p < patients.size(); p++) {
    for(size_t s = 0; s < patients[p].sampled_paths.size(); s++) {
      aggregate_paths.push_back(patients[p].sampled_paths[s]);
    }
  }
  
  for(size_t p = 0; p < aggregate_paths.size(); p++) {
    aggregate_paths[p].depth = 0;
    for(size_t r = 0; r < patient::samples * 4; r++) {
      vector<scored_path> band = random_subsample(aggregate_paths, p);
      envelope band_envelope(band);
      aggregate_paths[p].depth += band_envelope.contains(aggregate_paths[p]);
    }
  }
  
  ranked_path_vector ranked_paths = rank_paths(aggregate_paths);
  path median = depth_median(ranked_paths);
  path mean = pointwise_mean(ranked_paths);
  envelope p50 = envelope(subset(ranked_paths, 0.5));
  envelope p95 = envelope(subset(ranked_paths, 0.95));

  out << "{"
      << "\n" << R"("mean" : )" << mean.path2json()
      << ",\n" << R"("mean" : )" << mean.path2json()
      << ",\n" << R"("50ile" : )" << p50.envelope2json()
      << ",\n" << R"("95ile" : )" << p95.envelope2json()
      << ",\n" << R"("patients" : [)" << flush;
  for(size_t p = 0; p < patients.size(); p++) {
    out << patients.at(p).patient2json() << ",\n";
  }
  out << "\b\n]\n}" << flush;
}