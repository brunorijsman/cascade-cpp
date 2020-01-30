#include "block.h"
#include "iteration.h"
#include "reconciliation.h"

#include <iostream> //@@@
#pragma GCC diagnostic ignored "-Wunused-private-field"   // TODO    

using namespace Cascade;

const int Block::unknown_parity = -1;

Block::Block(Iteration& iteration, unsigned block_nr, size_t start_bit_nr, size_t end_bit_nr):
    iteration(iteration),
    block_nr(block_nr),
    start_bit_nr(start_bit_nr),
    end_bit_nr(end_bit_nr),
    correct_parity(Block::unknown_parity)
{
    std::cout << "Create block " << this->get_name() << std::endl;
}

Block::~Block()
{
    std::cout << "Destroy block " << this->get_name() << std::endl;
}

std::string Block::get_name() const
{
    std::string name;
    if (this->iteration.get_biconf()) {
        name = "b:";
    } else {
        name = "c:";
    }
    name += std::to_string(this->iteration.get_iteration_nr()) + ":" + 
            std::to_string(this->block_nr);
    return name;
}

size_t Block::get_nr_bits() const
{
    return this->end_bit_nr - this->start_bit_nr + 1;
}

unsigned Block::get_iteration_nr() const
{
    return this->iteration.get_iteration_nr();
}

int Block::compute_current_parity() const
{
    const Key& shuffled_key = this->iteration.get_shuffled_key();
    return shuffled_key.compute_range_parity(this->start_bit_nr, this->end_bit_nr);
}

int Block::compute_parity_for_key(const Key& correct_key) const
{
    return correct_key.compute_range_parity(this->start_bit_nr, this->end_bit_nr);
}

void Block::set_correct_parity(int parity)
{
    this->correct_parity = parity;
}

int Block::compute_error_parity() const
{
    if (this->correct_parity == Block::unknown_parity) {
        return Block::unknown_parity;
    }
    if (this->correct_parity == this->compute_current_parity()) {
        return 0;
    } else {
        return 1;
    }
}

bool Block::try_correct(bool cascade)
{
    std::cout << "Try correct " << this->get_name() << " (cascade=" << cascade << ")" << std::endl;

    Reconciliation& reconciliation = this->iteration.get_reconciliation();

    // If we don't know the correct parity of the block, we cannot make progress on this block
    // until Alice has told us what the correct parity is.
    if (!this->correct_parity_is_know_or_can_be_inferred()) {
        reconciliation.schedule_try_correct(BlockPtr(this));
        return false;
    }

    // If there is an even number of errors in this block, we don't attempt to fix any errors
    // in this block. But if asked to do so, we will attempt to fix an error in the right
    // sibling block.
    int error_parity = this->compute_error_parity();
    assert(error_parity != Block::unknown_parity);
    if (error_parity == 0) {
        // TODO: Add this
        //  if correct_right_sibling:
        //      return self._try_correct_right_sibling_block(block, cascade)
        return false;
    }

    // If this block contains a single bit, we have finished the recursion and found an error.
    // Correct the error by flipping the key bit that corresponds to this block.
    if (this->get_nr_bits() == 1) {
        const Shuffle& shuffle = this->iteration.get_shuffle();
        size_t orig_key_bit_nr = shuffle.shuffle_to_orig(this->start_bit_nr);
        reconciliation.flip_orig_key_bit(orig_key_bit_nr);
        std::cout << "Correct bit " << orig_key_bit_nr << std::endl;
    }

    // if block.get_size() == 1:
    //     self._flip_key_bit_corresponding_to_single_bit_block(block, cascade)
    //     return 1    

    return false;
}

bool Block::correct_parity_is_know_or_can_be_inferred()
{
    if (this->correct_parity != Block::unknown_parity) {
        return true;
    }
    // TODO: Implement correct parity inference
    return false;
}
