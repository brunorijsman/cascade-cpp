#ifndef RECONCILIATION_H
#define RECONCILIATION_H

#include <string>

namespace Cascade {

class Algorithm;
class Key;

class Reconciliation
{
public:
    Reconciliation(std::string algorithm_name, const Key& noisy_key, double bit_error_rate);
private:
    const Algorithm* algorithm;
    const Key& noisy_key;
    double bit_error_rate;
};

} /* namespace Cascade */

#endif /* ifndef RECONCILIATION_H */
