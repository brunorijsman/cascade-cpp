#ifndef RECONCILIATION_H
#define RECONCILIATION_H

#include "key.h"
#include "iteration.h"
#include <map>
#include <deque>
#include <string>
#include <vector>

namespace Cascade {

class Algorithm;
class ClassicalSession;

class Reconciliation
{
public:
    Reconciliation(std::string algorithm_name,
                   ClassicalSession& classical_session,
                   const Key& noisy_key,
                   double estimated_bit_error_rate);
    ~Reconciliation();
    const Algorithm& get_algorithm() const;
    double get_estimated_bit_error_rate() const;
    Key& get_reconciled_key();
    void reconcile();
    void schedule_try_correct(BlockPtr block);
    void schedule_ask_correct_parity(BlockPtr block);
    void flip_orig_key_bit(size_t orig_key_bit_nr);
private:
    void service_all_pending_work(bool cascade);
    void service_pending_try_correct(bool cascade);
    void service_pending_ask_correct_parity();
    const Algorithm* algorithm;
    ClassicalSession& classical_session;
    double estimated_bit_error_rate;
    Key reconciled_key;
    std::vector<IterationPtr> iterations;
    std::deque<BlockPtr> pending_ask_correct_parity_blocks;
    std::deque<BlockPtr> pending_try_correct_blocks;
};

} /* namespace Cascade */

#endif /* ifndef RECONCILIATION_H */
