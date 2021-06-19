#if !defined (LWS_PLUGIN_STATIC)
#define LWS_DLL
#define LWS_INTERNAL
#include <libwebsockets.h>
#endif

#include <iostream>
#include <istream>
#include <streambuf>
#include <string>
#include <sstream>
#include <string.h>

/* one of these created for each message */

struct msg {
        void *payload; /* is malloc'd */
        size_t len;
};

/* one of these is created for each client connecting to us */

struct per_session_data__minimal {
        struct per_session_data__minimal *pss_list;
        struct lws *wsi;
        int last; /* the last message number we sent */
};

/* one of these is created for each vhost our protocol is used with */

struct per_vhost_data__minimal {
        struct lws_context *context;
        struct lws_vhost *vhost;
        const struct lws_protocols *protocol;

        struct per_session_data__minimal *pss_list; /* linked-list of live pss*/

        struct msg amsg; /* the one pending message... */
        int current; /* the current message number we are caching */
};

/* destroys the message when everyone has had a copy of it */

static void
__minimal_destroy_message(void *_msg)
{
        struct msg *msg = (struct msg*) _msg;

        free(msg->payload);
        msg->payload = NULL;
        msg->len = 0;
}

static int
callback_minimal(struct lws *wsi, enum lws_callback_reasons reason,
                        void *user, void *in, size_t len)
{
        struct per_session_data__minimal *pss =
                        (struct per_session_data__minimal *)user;
        struct per_vhost_data__minimal *vhd =
                        (struct per_vhost_data__minimal *)
                        lws_protocol_vh_priv_get(lws_get_vhost(wsi),
                                        lws_get_protocol(wsi));
        int m;

        switch (reason) {
        case LWS_CALLBACK_PROTOCOL_INIT:
                vhd = (struct per_vhost_data__minimal*) lws_protocol_vh_priv_zalloc(lws_get_vhost(wsi),
                                lws_get_protocol(wsi),
                                sizeof(struct per_vhost_data__minimal));
                vhd->context = lws_get_context(wsi);
                vhd->protocol = lws_get_protocol(wsi);
                vhd->vhost = lws_get_vhost(wsi);
                break;

        case LWS_CALLBACK_ESTABLISHED:
                /* add ourselves to the list of live pss held in the vhd */
                printf("connection established\n");
                lws_ll_fwd_insert(pss, pss_list, vhd->pss_list);
                pss->wsi = wsi;
                pss->last = vhd->current;
                break;

        case LWS_CALLBACK_CLOSED:
                /* remove our closing pss from the list of live pss */
                lws_ll_fwd_remove(struct per_session_data__minimal, pss_list,
                                  pss, vhd->pss_list);
                break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
                if (!vhd->amsg.payload)
                        break;

                if (pss->last == vhd->current)
                        break;

                /* notice we allowed for LWS_PRE in the payload already */
                m = lws_write(wsi, ((unsigned char *)vhd->amsg.payload) +
                              LWS_PRE, vhd->amsg.len, LWS_WRITE_TEXT);
                if (m < (int)vhd->amsg.len) {
                        lwsl_err("ERROR %d writing to ws\n", m);
                        return -1;
                }

                pss->last = vhd->current;
                break;

        case LWS_CALLBACK_RECEIVE:
            {
                if (vhd->amsg.payload)
                        __minimal_destroy_message(&vhd->amsg);

                std::string request((char *) in, (char *) in + len);
                std::istringstream ss(request);

                std::string requestType; 
                // use 5 get lines to parse the http request 
                std::string path; 
                std::string contentType; 
                std::string contentLenStr; 
                std::string empty; 
                std::string content; 
                
                std::getline(ss, path, '\n');
                int pathStart = path.find(" ") + 1; // don't want to include the space 
                int pathEnd = path.find(" ", pathStart);
                requestType = path.substr(0, pathStart-1); // don't want to include the space
                path = path.substr(pathStart, pathEnd - pathStart); 
                
                std::getline(ss, contentType, '\n');

                std::getline(ss, contentLenStr, '\n');

                std::getline(ss, empty, '\n');
                std::getline(ss, content, '\n');
                
                std::cout << content << "\n";
                vhd->amsg.len = content.length();
                /* notice we over-allocate by LWS_PRE */
                vhd->amsg.payload = malloc(LWS_PRE + content.length()+1);

                if (!vhd->amsg.payload) {
                        lwsl_user("OOM: dropping\n");
                        break;
                }
                
                memcpy((char *)vhd->amsg.payload + LWS_PRE, content.c_str(), content.length()+1);
                printf("returning: %.*s\n", content.length(), (char *) vhd->amsg.payload + LWS_PRE);
                vhd->current++;

                /*
                 * let everybody know we want to write something on them
                 * as soon as they are ready
                 */
                lws_start_foreach_llp(struct per_session_data__minimal **,
                                      ppss, vhd->pss_list) {
                        if ((path == "/new-user") || (path == "/message" && *ppss != pss)){
                                lws_callback_on_writable((*ppss)->wsi);
                        }
                        // lws_callback_on_writable((*ppss)->wsi);
                } lws_end_foreach_llp(ppss, pss_list);
                break;
            }

        default:
                break;
        }

        return 0;
}

#define LWS_PLUGIN_PROTOCOL_MINIMAL \
        { \
                "lws-minimal", \
                callback_minimal, \
                sizeof(struct per_session_data__minimal), \
                128, \
                0, NULL, 0 \
        }