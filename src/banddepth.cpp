#include <ctime>
#include <random>
#include "banddepth.hpp"

namespace random_helpers {
  random_helper::random_helper() {

  }

  double random_helper::draw_uniform(double scale) const {

  }

  double random_helper::draw_truncated_normal(double upper_limit, double mean, double std_dev) const {

  }

  double random_helper::draw_normal(double mean, double std_dev) const {

  }
}

namespace banddepth {

banddepth_K = 3;
samples = 20;

path::path(const vector<double>& values) : values(values), start_time(0), end_time(values.size() + start_time) { }

path::path(const vector<double>& values, size_t start_time) : values(values), start_time(start_time), end_time(values.size() + start_time) { }

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
  out << "{\n" << R"("upper : ")" << upper.path2json()
      << ",\n" << R"("lower : ")" << lower.path2json()
      << "\n}" << flush;
  return out.str();
}

ranked_path_vector rank(const vector<scored_path>& paths) {
  ranked_path_vector ranked;
}

ranked_path_vector subset(ranked_path_vector& paths, size_t max_idx) {
  if(fraction < 0 || fraction > 1) { throw runtime_error("in call to banddepth::subset, subset max_idx must be within path indices"); }
  ranked_path_vector ranked;
  for(size_t p = 0; p < paths.size(); p++) {
    ranked.push_back(paths[p]);
  }
  return ranked;
}

ranked_path_vector subset(ranked_path_vector& paths, double fraction) {
  if(fraction < 0 || fraction > 1) { throw runtime_error("in call to banddepth::subset, subset fraction must be in [0,1]"); }
  size_t max_idx = (paths.size() - 1) * fraction;
  return subset(paths, max_idx);
}

scored_path& depth_median(const ranked_path_vector& paths) {
  if(paths.size() == 0) { throw runtime_error("in call to banddepth::median(), paths vector is empty"); }
  return paths[0];
}

scored_path generate_sample(const vector<size_t>& times, const vector<double> values) {
  for(size_t i = 0; i < times.size(); i++) {
    v[times[i]] = draw_truncated_normal(v[times[i - 1]], values[i], std_dev);
  }

  for(size_t i = 0; i < times.size(); i++) {
    size_t t_0 = times[i];
    size_t t_1 = times[i + 1];
    for(size_t t = t_0 + 1; t < t_1; t++) {
      v[t] = v[t - 1] + draw_uniform(v[t] - v[t_1]);
    }
  }

  return path(v);
}

patient::patient(string name, vector<size_t> times, vector<double> values) : name(name), times(times), values(values) {
  for(size_t p = 0; p < samples; p++) {
    sampled_paths.push_back(generate_sample(times, values));
  }
  ranked_path_vector ranked_sampled_paths = rank(sampled_paths);
  mean = pointwise_mean(sample_paths);
  median = depth_median(sample_paths);
  for(size_t e = 0; e < envelope_levels.size(); e++) {
    envelopes.push_back(envelope(subset(sample_paths, envelope_levels.at(e))));
  }
}

string patient2json() const {
  stringstream out;
  out << "{\n" << R"("name" : )" << name
      << ",\n" << R"("median" : )" << path2json(median)
      << ",\n" << R"("mean" : )" << path2json(mean)
      << ",\n" << R"("envelopes" : [)" << flush;
  for(size_t e = 0; e < envelopes.size(); e++) {
    out << "\n{\n" << "R("upper" : [)";
    for(size_t t = 0; t < upper.size(); t++) {

    }
    out << "\b\n],"
        <<  "\n" << R"({ "lower" : [)" << flush;
    for(size_t t = 0; t < upper.size(); t++) {

    }
    out << "\b\n]\n},";
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
    for(size_t s = 0; s < patients.sample_paths.size(); s++) {
      aggregate_paths.push_back(patient.sample_paths[p][s]);
    }
  }
  ranked_path_vector ranked_paths = rank(aggregate_paths);
  path median = depth_median(ranked_paths);
  path mean = pointwise_mean(ranked_paths);
  envelope 50_percentile = envelope(subset(sample_paths, 0.5));
  envelope 95_percentile = envelope(subset(sample_paths, 0.95));

  out << "{"
      << "\n" << R"("mean" : )" << mean.path2json()
      << ",\n" << R"("mean" : )" << mean.path2json()
      << ",\n" << R"("50ile" : )" << envelope2json(50_percentile)
      << ",\n" << R"("95ile" : )" << envelope2json(95_percentile)
      << ",\n" << R"("patients" : [)" << flush;
  for(size_t p = 0; p < patients.size(); p++) {
    out << patient2json(patients.at(i)) << ",\n";
  }
  out << "\b\n]\n}" << flush;
}

} // namespace banddepth
