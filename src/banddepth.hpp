#ifndef BAND_DEPTH_H
#define BAND_DEPTH_H

#include <vector>
#include <string>
#include <iostream>

using namespace std;

namespace banddepth {

static size_t banddepth_replicates;
static size_t candidate_replicates;

struct path {
  vector<size_t> times;
  vector<double> values;
  path();
  path(const vector<size_t>& times, const vector<double>& values);
  void times2json(ostream& out) const;
  void values2json(ostream& out) const;
  size_t index_below(size_t t) const;
  size_t index_above(size_t t) const;
  double value(size_t t) const;
};

struct scored_path : public path {
  size_t depth;
  void get_depth(const vector<path>& paths);
};

struct envelope {
  path upper;
  path lower;
  envelope(const vector<path>& paths);
};

size_t max_time(const vector<size_t>& times);
size_t min_time(const vector<size_t>& times);
vector<size_t> union_of_times(const vector<path>& paths);

double max_value(const vector<path>& paths, size_t t);
double min_value(const vector<path>& paths, size_t t);
double mean_value(const vector<path>& paths, size_t t);

struct path_ensemble {
  vector<scored_path> paths;
  vector<size_t> t_union;
  path mean;
  path median;
  path_ensemble(const vector<path>& paths);

  path& get_mean();
  path& get_median();
  envelope get_envelope(double bandwidth = 0.5);
  void path_ensemble2json(ostream& out) const;
};

static double measurement_error = 0;
static double starttime_error = 0;

path_ensemble sample_paths(const vector<path>& paths);

vector<patient_contour> tsv2patients(istream& in);

struct patient {
  string id;
  path contour;
};

vector<path> patients_to_contours(const vector<patient>& patients);

struct patient_contour : public patient {

};

static size_t patient_envelopes;

struct patient_distribution : public patient {
  vector<envelope> envelopes;
  patient_distribution(patient_contour& contour);
  void patient_distribution2json(ostream& out) const;
};

} // namespace banddepth

#endif
