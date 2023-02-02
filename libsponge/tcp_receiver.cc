#include "tcp_receiver.hh"

#include <cassert>

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

/**
 *  \brief 当前 TCPReceiver 大体上有三种状态， 分别是
 *      1. LISTEN，此时 SYN 包尚未抵达。可以通过 _set_syn_flag 标志位来判断是否在当前状态
 *      2. SYN_RECV, 此时 SYN 抵达。只能判断当前不在 1、3状态时才能确定在当前状态
 *      3. FIN_RECV, 此时 FIN 抵达。可以通过 ByteStream end_input 来判断是否在当前状态
 */

void TCPReceiver::segment_received(const TCPSegment &seg) {
    // 判断是否是 SYN 包
    const TCPHeader &header = seg.header();
    if (!_set_syn_flag) {
        // 注意 SYN 包之前的数据包必须全部丢弃
        if (!header.syn)
            return;
        _isn = header.seqno;
        _set_syn_flag = true;
    }
    uint64_t abs_ackno = _reassembler.stream_out().bytes_written() + 1;
    uint64_t curr_abs_seqno = unwrap(header.seqno, _isn, abs_ackno);

    //! NOTE: SYN 包中的 payload 不能被丢弃
    //! NOTE: reassember 足够鲁棒以至于无需进行任何 seqno 过滤操作
    uint64_t stream_index = curr_abs_seqno - 1 + (header.syn);
    _reassembler.push_substring(seg.payload().copy(), stream_index, header.fin);
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    // 判断是否是在 LISTEN 状态
    if (!_set_syn_flag)
        return nullopt;
    // 如果不在 LISTEN 状态，则 ackno 还需要加上一个 SYN 标志的长度
    uint64_t abs_ack_no = _reassembler.stream_out().bytes_written() + 1;
    // 如果当前处于 FIN_RECV 状态，则还需要加上 FIN 标志长度
    if (_reassembler.stream_out().input_ended())
        ++abs_ack_no;
    return WrappingInt32(_isn) + abs_ack_no;
}

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size(); }
