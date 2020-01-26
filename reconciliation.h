#ifndef RECONCILIATION_H
#define RECONCILIATION_H

#include "key.h"
#include <memory>
#include <string>
#include <vector>

namespace Cascade {

class Algorithm;

class Reconciliation
{
public:
    Reconciliation(std::string algorithm_name,
                   const Key& noisy_key,
                   double estimated_bit_error_rate);
    void reconcile(void);
private:
    const Algorithm* algorithm;
    const Key& noisy_key;
    double estimated_bit_error_rate;
    Key reconciliated_key;
    void cascade_iteration(unsigned interation_nr);
    void biconf_iteration(unsigned interation_nr);
};

} /* namespace Cascade */

#endif /* ifndef RECONCILIATION_H */
