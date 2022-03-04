#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader& tcp_header = seg.header();

    if (_connected == false) {
        if (tcp_header.syn == false) return;
        _connected = true;
        _ISN = tcp_header.seqno;
    }

    uint64_t ackno = _reassembler.stream_out().bytes_written() + 1;
    uint64_t seqno = unwrap(tcp_header.seqno, _ISN, ackno);
    uint64_t index = seqno - 1 + static_cast<uint64_t>(tcp_header.syn);
    _reassembler.push_substring(seg.payload().copy(), index, tcp_header.fin);
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (_connected == false) return nullopt;
    uint64_t ackno = _reassembler.stream_out().bytes_written() + 1;
    if (_reassembler.stream_out().input_ended())// 如果结束连接,返回的ack要算上sender发来的fin
        ++ackno;
    return WrappingInt32(_ISN.raw_value() + ackno);
}

size_t TCPReceiver::window_size() const {
    return _capacity - _reassembler.stream_out().buffer_size();
}
