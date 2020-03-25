#include "key.h"
#include "mock_classical_session.h"
#include "experiments.h"
#include "options.h"
#include "random.h"
#include "report.h"
#include "reconciliation.h"
#include "series.h"
#include <deque>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// TODO: implement output_directory

static std::deque<Serie> series_queue;
static std::mutex series_mutex;
static std::mutex count_mutex;
static int total_nr_data_points = 0;
static int data_points_nr = 0;

void one_data_point_run(const std::string& algorithm, int key_size, double error_rate)
{
    Cascade::Key correct_key(key_size);
    Cascade::MockClassicalSession classical_session(correct_key);

    Cascade::Key noisy_key = correct_key;
    noisy_key.apply_noise(error_rate);

    Cascade::Reconciliation reconciliation(algorithm, classical_session, noisy_key, error_rate);
    reconciliation.reconcile();

    Cascade::Key& reconciled_key = reconciliation.get_reconciled_key();
    std::cout << "bit errors = " << correct_key.nr_bits_different(reconciled_key) << std::endl;//@@@
    assert(correct_key.nr_bits_different(reconciled_key) == 0);
}

void produce_one_data_point(const std::string& algorithm, int key_size, double error_rate, int runs)
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

    for(int run = 0; run < runs; ++run) {
        one_data_point_run(algorithm, key_size, error_rate);
    }

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

void produce_one_serie(const Serie& serie)
{
    std::cout << "produce_one_serie: serie.name = " << serie.name << std::endl; //@@@
    for (auto key_size: serie.key_sizes) {
        for (auto error_rate: serie.error_rates) {
            produce_one_data_point(serie.algorithm, key_size, error_rate, serie.runs);
        }
    }
}

void serie_worker()
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
        produce_one_serie(serie);
    }
}

void compute_total_nr_data_points(Series& series)
{
    total_nr_data_points = 0;
    for (Serie& serie: series.series) {
        total_nr_data_points += serie.key_sizes.size() * serie.error_rates.size();
    }
}

void run_all_series_multi_threaded(Series& series) {
    // Put all series on the queue. We don't need any locking because we have not started the
    // workers yet.
    for (Serie& serie: series.series) {
        series_queue.push_back(serie);
    }

    // Start the workers.
    std::vector<std::thread> worker_threads;
    int nr_workers = std::thread::hardware_concurrency();
    for (int worker_nr = 0; worker_nr < nr_workers; ++worker_nr) {
        std::thread worker_thread = std::thread(serie_worker);
        worker_threads.push_back(std::move(worker_thread));
    }

    // Wait for all of the workers to complete.
    for (auto& worker_thread: worker_threads) {
        worker_thread.join();
    }
}

void run_all_series_single_threaded(Series& series) {
    for (Serie& serie: series.series) {
        produce_one_serie(serie);
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
        run_all_series_multi_threaded(series);
    } else {
        run_all_series_single_threaded(series);
    }
    return 0;
}
