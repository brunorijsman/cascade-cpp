#include "data_point.h"
#include "debug.h"
#include "experiments.h"
#include "key.h"
#include "mock_classical_session.h"
#include "options.h"
#include "random.h"
#include "report.h"
#include "reconciliation.h"
#include "series.h"
#include <boost/filesystem.hpp>
#include <cerrno>
#include <deque>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

static std::deque<Serie> series_queue;
static std::mutex series_mutex;
static std::mutex count_mutex;
static int total_nr_data_points = 0;
static int data_points_nr = 0;

void fatal_perror(const std::string& message)
{
    {
        // Take mutex to avoid error messages from multiple threads
        std::lock_guard<std::mutex> guard(series_mutex);
        std::perror(message.c_str());
        exit(1);
    }
}

void one_data_point_run(DataPoint& data_point, const std::string& algorithm, int key_size,
                        double error_rate)
{
    Cascade::Key correct_key(key_size);
    Cascade::MockClassicalSession classical_session(correct_key);

    Cascade::Key noisy_key = correct_key;
    noisy_key.apply_noise(error_rate);

    int actual_bit_errors = correct_key.nr_bits_different(noisy_key);
    data_point.actual_bit_errors.record_value(actual_bit_errors);
    double actual_bit_error_rate = double(actual_bit_errors) / double(key_size);
    data_point.actual_bit_error_rate.record_value(actual_bit_error_rate);

    Cascade::Reconciliation reconciliation(algorithm, classical_session, noisy_key, error_rate,
                                           &correct_key);
    reconciliation.reconcile();

    data_point.record_reconciliation_stats(reconciliation.get_stats());

    int remaining_bit_errors = correct_key.nr_bits_different(reconciliation.get_reconciled_key());
    // TODO: Report actual remaining bit errors
    DEBUG("remaining_bit_errors=" << remaining_bit_errors);
    data_point.remaining_bit_errors.record_value(remaining_bit_errors);
    double remaining_bit_error_rate = double(remaining_bit_errors) / double(key_size);
    data_point.remaining_bit_error_rate.record_value(remaining_bit_error_rate);
    if (remaining_bit_errors > 0)
        data_point.remaining_frame_error_rate.record_value(1.0);
    else
        data_point.remaining_frame_error_rate.record_value(0.0);
}

void produce_one_data_point(const std::string& algorithm, int key_size, double error_rate, int runs,
                            std::ofstream& data_file)
{
    {
        std::lock_guard<std::mutex> guard(count_mutex);
        data_points_nr += 1;
        REPORT(data_points_nr << "/" << total_nr_data_points
               << " algorithm=" << algorithm
               << " key_size=" << key_size
               << " error_rate=" << error_rate
               << " runs=" << runs);
    }

    DataPoint data_point(algorithm, key_size, error_rate);
    for(int run = 0; run < runs; ++run) {
        one_data_point_run(data_point, algorithm, key_size, error_rate);
    }

    data_file << data_point.to_json() << std::endl;
    if (!data_file.good())
        fatal_perror("Could write data point to file");
}

std::string serie_file_name(const Serie& serie)
{
    std::stringstream file_name("data__");
    file_name << "algorithm=" << serie.algorithm << ";";
    if (serie.key_sizes.size() == 1) {
        file_name << "key_size=" << serie.key_sizes[0] << ";";
    } else {
        file_name << "key_size=vary;";
    }
    if (serie.error_rates.size() == 1) {
        file_name << "error_rate=" << serie.error_rates[0];
    } else {
        file_name << "error_rate=vary";
    }
    return file_name.str();
}

void produce_one_serie(const Serie& serie, const Options& options)
{
    boost::filesystem::path data_file_name = "data__" + serie.name;
    if (!options.output_directory.empty())
        data_file_name = options.output_directory / data_file_name;
    std::ofstream data_file;
    data_file.open(data_file_name.string());
    if (!data_file.good()) {
        fatal_perror("Could not open file " + data_file_name.string() + " for writing");
    }
    for (auto key_size: serie.key_sizes) {
        for (auto error_rate: serie.error_rates) {
            produce_one_data_point(serie.algorithm, key_size, error_rate, serie.runs, data_file);
        }
    }
    data_file.close();
}

void serie_worker(const Options& options)
{
    while (true) {
        Serie serie;
        {
            std::lock_guard<std::mutex> guard(series_mutex);
            if (series_queue.empty()) {
                break;
            }
            serie = series_queue.front();
            series_queue.pop_front();
        }
        produce_one_serie(serie, options);
    }
}

void compute_total_nr_data_points(Series& series)
{
    total_nr_data_points = 0;
    for (Serie& serie: series.series) {
        total_nr_data_points += serie.key_sizes.size() * serie.error_rates.size();
    }
}

// TODO: Make Series& const
void run_all_series_multi_threaded(Series& series, const Options& options) {
    // Put all series on the queue. We don't need any locking because we have not started the
    // workers yet.
    for (Serie& serie: series.series) {
        series_queue.push_back(serie);
    }

    // Start the workers.
    std::vector<std::thread> worker_threads;
    int nr_workers = std::thread::hardware_concurrency();
    for (int worker_nr = 0; worker_nr < nr_workers; ++worker_nr) {
        std::thread worker_thread = std::thread(serie_worker, options);
        worker_threads.push_back(std::move(worker_thread));
    }

    // Wait for all of the workers to complete.
    for (auto& worker_thread: worker_threads) {
        worker_thread.join();
    }
}

void run_all_series_single_threaded(Series& series, const Options& options) {
    for (Serie& serie: series.series) {
        produce_one_serie(serie, options);
    }
}

int main(int argc, char** argv)
{
    Options options;
    options.parse(argc, argv);

    if (options.seed_is_set)
        Cascade::random_seed(options.seed);
    Experiments experiments(options.experiments_file);
    Series series(experiments, options.max_runs);
    compute_total_nr_data_points(series);
    if (options.multi_processing) {
        run_all_series_multi_threaded(series, options);
    } else {
        run_all_series_single_threaded(series, options);
    }
    return 0;
}
