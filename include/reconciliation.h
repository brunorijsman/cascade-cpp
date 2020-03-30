#ifndef RECONCILIATION_H
#define RECONCILIATION_H

#include "key.h"
#include "iteration.h"
#include "pending_item.h"
#include "stats.h"
#include <map>
#include <set>
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
    int get_nr_key_bits() const;
    Stats& get_stats();
    void reconcile();
    void schedule_try_correct(BlockPtr block, bool correct_right_sibling);
    void schedule_ask_correct_parity(BlockPtr block, bool correct_right_sibling);
    void correct_orig_key_bit(int orig_key_bit_nr, int triggering_iteration_nr, bool cascade);
private:
    void service_all_pending_work(bool cascade);
    void service_pending_try_correct(bool cascade);
    void service_pending_ask_correct_parity();
    double compute_efficiency(long reconciliation_bits) const;
    const Algorithm* algorithm;
    ClassicalSession& classical_session;
    double estimated_bit_error_rate;
    Key reconciled_key;
    int nr_key_bits;
    std::vector<IterationPtr> iterations;
    PendingItemQueue pending_ask_correct_parity_blocks;
    PendingItemQueue pending_try_correct_blocks;
    typedef std::set<BlockPtr> BlockSet;
    typedef std::vector<BlockSet> BlockSetVector;
    BlockSetVector key_bit_to_blocks;
    Stats stats;
};

} /* namespace Cascade */

#endif /* ifndef RECONCILIATION_H */
