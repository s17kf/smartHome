//
// Created by s17kf on 11.12.18.
//

#include <cstring>
#include "Answer.h"

Answer::Answer(const int socket_fd, const short recipient_id, const size_t msg_len,
        const uchar *msg, bool last_msg_on_socket)
:socket_fd(socket_fd),recipient_id(recipient_id), msg_len(msg_len),
last_msg_on_socket(last_msg_on_socket){
    this->msg = new uchar[msg_len];
    memcpy(this->msg, msg, msg_len);
}

Answer::~Answer() {
    delete[] msg;
}

short Answer::getRecipient_id() const {
    return recipient_id;
}

size_t Answer::getMsg_len() const {
    return msg_len;
}

uchar *Answer::getMsg() const {
    return msg;
}

int Answer::getSocket_fd() const {
    return socket_fd;
}

bool Answer::isLast_msg_on_socket() const {
    return last_msg_on_socket;
}
