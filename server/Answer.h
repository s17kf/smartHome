//
// Created by s17kf on 11.12.18.
//

#ifndef SERVER_ANSWER_H
#define SERVER_ANSWER_H


#include "Const.h"

class Answer {
    int socket_fd;
    short recipient_id;
    size_t msg_len;
    uchar *msg;
    bool last_msg_on_socket;

public:
    Answer(const int socket_fd, const short recipient_id, const size_t msg_len,
            const uchar *msg, bool last_msg_on_socket = false);
    ~Answer();

    int getSocket_fd() const;

    short getRecipient_id() const;

    size_t getMsg_len() const;

    uchar *getMsg() const;

    bool isLast_msg_on_socket() const;
};


#endif //SERVER_ANSWER_H
