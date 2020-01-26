#ifndef RECONCILIATION_H
#define RECONCILIATION_H

#include "key.h"
#include <map>
#include <string>
#include <vector>

namespace Cascade {

class Algorithm;

class Reconciliation
{
public:
    static void set_seed(uint64_t seed);
    Reconciliation(std::string algorithm_name,
                   const Key& noisy_key,
                   double estimated_bit_error_rate);
    void reconcile(void);
private:
    const Algorithm* algorithm;
    const Key& noisy_key;
    double estimated_bit_error_rate;
    Key reconciliated_key;
    std::vector<Key> iteration_keys;
    typedef std::map<unsigned, unsigned> BitMap;
    std::vector<BitMap> iteration_bit_maps;
    void iteration_common(unsigned iteration_nr);
    void shuffle_iteration_key(unsigned iteration_nr);
    void cascade_iteration(unsigned interation_nr);
    void biconf_iteration(unsigned interation_nr);
};

} /* namespace Cascade */

#endif /* ifndef RECONCILIATION_H */
