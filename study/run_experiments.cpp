#include "key.h"
#include "mock_classical_session.h"
#include "experiments.h"
#include "options.h"
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
static std::mutex report_mutex;
static int total_nr_data_points = 0;
static int data_points_nr = 0;

void produce_one_data_point(const std::string& algorithm, int key_size, double error_rate, int runs)
{
    // Cascade::Key correct_key(10000);
    // Cascade::MockClassicalSession classical_session(correct_key);

    // Cascade::Key noisy_key = correct_key;
    // noisy_key.apply_noise(error_rate);

    // Cascade::Reconciliation reconciliation(algorithm, classical_session, noisy_key, error_rate);
    // reconciliation.reconcile();

    // Cascade::Key& reconciled_key = reconciliation.get_reconciled_key();
    // ASSERT_EQ(correct_key.nr_bits_different(reconciled_key), 0);



    {
        std::lock_guard<std::mutex> guard(report_mutex);
        data_points_nr += 1;
        std::cout << data_points_nr << "/" << total_nr_data_points
                  << " algorithm=" << algorithm
                  << " key_size=" << key_size
                  << " error_rate=" << error_rate
                  << " runs=" << runs << std::endl;
    }
}

void produce_one_serie(const Serie& serie)
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
    {
        std::lock_guard<std::mutex> guard(report_mutex);
        std::cout << "file_name = " << file_name.str() << std::endl;
    }
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

void run_all_series(Series& series) {
    // Put all series on the queue. We don't need any locking because we have not started the
    // workers yet.
    for (Serie& serie: series.series) {
        series_queue.push_back(serie);
        total_nr_data_points += serie.key_sizes.size() * serie.error_rates.size();
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

    // Consume all the results.
    // TODO
}

int main(int argc, char** argv)
{
    Options options;
    options.parse(argc, argv);
    Experiments experiments(options.experiments_file);
    Series series(experiments, options.max_runs);
    run_all_series(series);
    return 0;
}
