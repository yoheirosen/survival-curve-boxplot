#include "banddepth.hpp"

using namespace std;

int main()
{
    cout << "test path methods" << endl; {
        vector<double> values = {1, 2, 3};
        scored_path testPath = scored_path(values);
        cout << testPath.path2json() << endl;
    }
    
    cout << "test min/max times" << endl; {
        vector<double> values = {1, 2, 3};
        vector<size_t> times = {8, 4, 9, 3, 1};
        cout << min_time(times) << "-" << max_time(times) << endl;
        vector<path> paths = {path(values, 0), path(values, 1), path(values, 2)};
        cout << max_end_time(paths) << endl;
    }
    
    cout << "test min/max over paths" << endl; {
        path path1({0});
        path path2({1});
        path path3({2});
        vector<path> paths = {path1, path2, path3};
        cout << min_value(paths, 0) << "-" << max_value(paths, 0) 
             << "~" << mean_value(paths, 0) << endl;
    }
    
    cout << "test min/max envelopes over paths" << endl; {
        path path1({3,2,1});
        path path2({4,2,0});
        path path3({2,2});
        vector<path> paths = {path1, path2, path3};
        cout << "min \t" << pointwise_min(paths).path2json() << "\n" 
             << "max \t" << pointwise_max(paths).path2json() << "\n"
             << "mean \t" << pointwise_mean(paths).path2json() << endl;
    }
    
    cout << "test containment" << endl; {
        path query1({3,2.5,2});
        path query2({5,2,1});
        path query3({5,0,0});
        path upper({4,3,2});
        path lower({2,1,0.5});
        vector<path> paths = {upper, lower};
        envelope test_envelope(paths);
        cout << "envelope \t" << test_envelope.envelope2json() << endl;
        cout << "strict" << endl;
        cout << test_envelope.contains(query1) << "\t"
             << test_envelope.contains(query2) << "\t"
             << test_envelope.contains(query3) << endl;
        cout << "fuzzy all" << endl;
        cout << test_envelope.contains(query1, 1) << "\t"
             << test_envelope.contains(query2, 1) << "\t"
             << test_envelope.contains(query3, 1) << endl;
        cout << "fuzzy 1/2 coverage" << endl;
        cout << test_envelope.contains(query1, 0.5) << "\t"
             << test_envelope.contains(query2, 0.5) << "\t"
             << test_envelope.contains(query3, 0.5) << endl;
    }
    
    cout << "test sorting" << endl; {
        scored_path path1({1});
        scored_path path2({2});
        scored_path path3({3});
        scored_path path4({4});
        path1.depth = 1;
        path2.depth = 2;
        path3.depth = 3;
        path4.depth = 4;
        vector<scored_path> paths = {path4, path2, path1, path3};
        rank_paths(paths);
        for(size_t i = 0; i < paths.size(); i++) {
            cout << "path rank " << i << " is path" << paths[i][0] << endl;
        }
    }
    
    cout << "test subsampling" << endl; {
        scored_path path1({1});
        scored_path path2({2});
        scored_path path3({3});
        scored_path path4({4});
        vector<scored_path> paths = {path4, path2, path1, path3};
        vector<scored_path> subsample = random_subsample(paths, 5);
        
        for(size_t i = 0; i < subsample.size(); i++) {
            cout << "path rank " << i << " is path" << subsample[i][0] << endl;
        }
    }
    
    cout << "test path sampling" << endl; {
        vector<size_t> times = {0, 5, 8};
        vector<double> values = {30, 20, 3};
        scored_path sample = generate_sample(times, values);
        cout << sample.path2json() << endl;
    }
    
    cout << "test patient building" << endl; {
        vector<size_t> times = {0, 5, 8};
        vector<double> values = {30, 20, 3};
        patient test_patient("test", times, values);
        cout << test_patient.patient2json() << endl;
    }
    
    generate_statistics({"001", "002", "003"}, {{0,2,4}, {0,2,4}, {0,2,4}}, {{30,10,3}, {30,22,8}, {30,14,11}}, cout);
    
    return 0;
}