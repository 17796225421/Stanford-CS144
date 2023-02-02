#include "wrapping_integers.hh"

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) { return WrappingInt32{isn + static_cast<uint32_t>(n)}; }

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    // 32位的范围
    const constexpr uint64_t INT32_RANGE = 1l << 32;
    // 获取 n 与 isn 之间的偏移量（mod）
    // 实际的 absolute seqno % INT32_RANGE == offset
    uint32_t offset = n - isn;
    /// NOTE: 最大的坑点！如果 checkpoint 比 offset 大，那么就需要进行四舍五入
    /// NOTE: 但是!!! 如果 checkpoint 比 offset 还小，那就只能向上入了，即此时的 offset 就是 abs seqno
    if (checkpoint > offset) {
        // 加上半个 INT32_RANGE 是为了四舍五入
        uint64_t real_checkpoint = (checkpoint - offset) + (INT32_RANGE >> 1);
        uint64_t wrap_num = real_checkpoint / INT32_RANGE;
        return wrap_num * INT32_RANGE + offset;
    } else
        return offset;
}
