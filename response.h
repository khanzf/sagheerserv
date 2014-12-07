#ifndef _RESPONSE_H
#define _RESPONSE_H

#include "config.h"
#include "modules.h"
#include "parse.h"

void sendreply(struct clientdata_h *clientdata);
void send_get_reply(struct clientdata_h *clientdata);
void sendserverinfo(struct clientdata_h *clientdata);
void sendcontenttype(struct clientdata_h *clientdata);
void sendcontentlength(struct clientdata_h *clientdata);
void sendbasiccontent(struct clientdata_h *clientdata, FILE *filedes);
void send_get_reply_dir(struct clientdata_h *clientdata);
void send_error_page(struct clientdata_h *clientdata);
void send_501(struct clientdata_h *clientdata, char *line1);
void send_400(struct clientdata_h *clientdata, char *line1);
void senddirlisting(struct clientdata_h *clientdata);
void sendpage(struct clientdata_h *clientdata, FILE *filedes);

#endif
