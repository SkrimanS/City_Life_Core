#include "clc/sim/Storage.hpp"

#include <cstdlib>
#include <iostream>
#include <limits>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(1);
    }
}

} // namespace

int main() {
    clc::sim::ResourceStorage source;
    clc::sim::ResourceStorage target;

    require(!source.add("grain", 0).ok(), "storage add should reject zero amount");
    require(source.empty(), "zero add should not mutate storage");

    require(source.add("grain", 10).ok(), "source should accept grain");
    require(target.add("grain", 5).ok(), "target should accept grain");

    const auto zero_transfer = clc::sim::transfer(source, target, "grain", 0);
    require(!zero_transfer.ok(), "transfer should reject zero amount");
    require(source.amount("grain") == 10, "zero transfer should not debit source");
    require(target.amount("grain") == 5, "zero transfer should not credit target");

    const auto transfer_report = clc::sim::transfer(source, target, "grain", 4);
    require(transfer_report.ok(), "valid transfer should succeed");
    require(source.amount("grain") == 6, "valid transfer should debit source");
    require(target.amount("grain") == 9, "valid transfer should credit target");

    const auto failed_insufficient = clc::sim::transfer(source, target, "grain", 7);
    require(!failed_insufficient.ok(), "transfer should reject insufficient source amount");
    require(source.amount("grain") == 6, "failed insufficient transfer should not debit source");
    require(target.amount("grain") == 9, "failed insufficient transfer should not credit target");

    clc::sim::ResourceStorage overflow_source;
    clc::sim::ResourceStorage overflow_target;
    require(overflow_source.add("wood", 10).ok(), "overflow source should accept wood");
    require(overflow_target.add("wood", std::numeric_limits<std::uint64_t>::max() - 5).ok(), "overflow target should accept near max wood");

    const auto failed_overflow = clc::sim::transfer(overflow_source, overflow_target, "wood", 10);
    require(!failed_overflow.ok(), "transfer should reject destination overflow");
    require(overflow_source.amount("wood") == 10, "overflow transfer should not debit source");
    require(overflow_target.amount("wood") == std::numeric_limits<std::uint64_t>::max() - 5, "overflow transfer should not mutate target");

    clc::sim::ResourceStorage self_storage;
    require(self_storage.add("stone", 3).ok(), "self storage should accept stone");
    const auto self_transfer = clc::sim::transfer(self_storage, self_storage, "stone", 3);
    require(self_transfer.ok(), "self transfer with enough resources should be a no-op success");
    require(self_storage.amount("stone") == 3, "self transfer should preserve amount");

    const auto failed_self_transfer = clc::sim::transfer(self_storage, self_storage, "stone", 4);
    require(!failed_self_transfer.ok(), "self transfer should still reject insufficient resources");
    require(self_storage.amount("stone") == 3, "failed self transfer should preserve amount");

    const auto empty_resource_transfer = clc::sim::transfer(source, target, "", 1);
    require(!empty_resource_transfer.ok(), "transfer should reject empty resource id");
    require(source.amount("grain") == 6, "empty resource transfer should not mutate source");
    require(target.amount("grain") == 9, "empty resource transfer should not mutate target");

    return 0;
}
