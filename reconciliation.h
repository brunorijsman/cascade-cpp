#ifndef RECONCILIATION_H
#define RECONCILIATION_H

#include <string>

namespace Cascade {

class Algorithm;
class Key;

class Reconciliation
{
public:
    Reconciliation(std::string algorithm_name,
                   const Key& noisy_key,
                   double estimated_bit_error_rate);
    void reconciliate(void);
private:
    const Algorithm* algorithm;
    const Key& noisy_key;
    double estimated_bit_error_rate;
};

} /* namespace Cascade */

#endif /* ifndef RECONCILIATION_H */
