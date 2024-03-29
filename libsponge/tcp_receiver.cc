#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    DUMMY_CODE(seg);

    // These sequence numbers(seqnos) are transmitted
    // in the header of each TCP segment. (And, again, there are two
    // streams—one in each direction. Each stream has separate sequence
    // numbers and a different random ISN.)
    const TCPHeader &tcp_header = seg.header();

    if (tcp_header.syn && !_exist_syn) {
        _exist_syn = true;
        _isn = tcp_header.seqno;
    }

    _exist_fin = tcp_header.fin | _exist_fin;

    if (_exist_syn) {
        size_t check = _reassembler.stream_out().bytes_written() + 1;
        size_t str_seqno = unwrap(tcp_header.seqno, _isn, check);

        if (tcp_header.syn) {
            _reassembler.push_substring(seg.payload().copy(), str_seqno, tcp_header.fin);
        } else {
            _reassembler.push_substring(seg.payload().copy(), str_seqno - 1, tcp_header.fin);
        }
    }
    else {
        return;
    }

}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (_exist_syn) {
        size_t temp = _reassembler.stream_out().bytes_written() + 1;
        if (_exist_fin && _reassembler.stream_out().input_ended()) {
            temp++;
        }
        return wrap(temp, _isn);
    }
    else {
        return std::nullopt;
    }
}

size_t TCPReceiver::window_size() const {
    return _reassembler.stream_out().remaining_capacity();
}
