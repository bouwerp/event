/*
MIT License

Copyright (c) 2019 Pieter Bouwer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include "event.h"

// zero initialiser for hub
const struct event_hub_t NEW_EVENT_HUB = {
        NULL,
        0,
        NULL,
        0
};

// zero initialiser for subscription
const struct subscription_t NEW_SUBSCRIPTION = {
        0L,
        0,
        0,
        NULL,
        NULL,
        NULL,
};

// zero initialiser for binary message
const struct binary_message_t NEW_BINARY_MESSAGE = {
        NULL,
        0
};

int find_subscription_index(event_hub *hub, int id, const char *topic) {
    if (hub == NULL) {
        return -1;
    }
    if (hub->n_subscriptions == 0) {
        return -1;
    } else {
        for (int i=0; i<hub->n_subscriptions; i++) {
            if (strcmp(hub->subscriptions[i]->topic, topic) == 0 && hub->subscriptions[i]->id == id) {
                return i;
            }
        }
    }
    return -1;
}

long event_subscribe(event_hub *hub, const char *topic) {
    if (hub == NULL) {
        return EVENT_NO_HUB;
    }
    if (hub->n_subscriptions == 0) {
        hub->subscriptions = malloc(1*sizeof(subscription*));
        hub->subscriptions[0] = malloc(sizeof(subscription));
        *hub->subscriptions[0] = NEW_SUBSCRIPTION;
        for (int i=0; i<hub->n_topics; i++) {
            if (strcmp(topic, hub->topics[i]) == 0) {
                hub->subscriptions[0]->topic = hub->topics[i];
            }
        }
        if (hub->subscriptions[0]->topic == NULL) {
            return EVENT_NO_TOPIC;
        }
        hub->n_subscriptions = 1;
        hub->subscriptions[0]->id = 0;
        // terminate
        hub->subscriptions[1] = NULL;
        return hub->subscriptions[0]->id;
    } else {
        subscription** subscriptions = malloc((1+hub->n_subscriptions) * sizeof(subscription*));
        for (int i=0; i<hub->n_subscriptions; i++) {
            subscriptions[i] = malloc(sizeof(subscription));
            *subscriptions[i] = NEW_SUBSCRIPTION;
            subscriptions[i]->topic = hub->subscriptions[i]->topic;
            subscriptions[i]->id = hub->subscriptions[i]->id;
        }
        subscriptions[hub->n_subscriptions] = malloc(sizeof(subscription));
        subscriptions[hub->n_subscriptions]->id = hub->n_subscriptions;
        subscriptions[hub->n_subscriptions]->topic = NULL;
        for (int i=0; i<hub->n_topics; i++) {
            if (strcmp(topic, hub->topics[i]) == 0) {
                subscriptions[hub->n_subscriptions]->topic = hub->topics[i];
            }
        }
        if (subscriptions[hub->n_subscriptions]->topic == NULL) {
            return EVENT_NO_TOPIC;
        }
        hub->n_subscriptions++;
        hub->subscriptions = realloc(hub->subscriptions, (1+hub->n_subscriptions) * sizeof(subscription*));
        for (int i=0; i<hub->n_subscriptions; i++) {
            if (i == hub->n_subscriptions-1) {
                hub->subscriptions[i] = malloc(sizeof(subscription));
            } else {
                hub->subscriptions[i] = realloc(hub->subscriptions[i], sizeof(subscription));
            }
            hub->subscriptions[i]->id = subscriptions[i]->id;
            hub->subscriptions[i]->topic = subscriptions[i]->topic;
            free(subscriptions[i]);
        }
        free(subscriptions);
    }
    return 0;
}

int event_unsubscribe(event_hub *hub, int id, char *topic) {
    if (hub == NULL) {
        return EVENT_NO_HUB;
    }
    if (hub->n_subscriptions > 0) {
        if (hub->n_subscriptions == 1) {
            if (hub->subscriptions[0]->n_string_messages > 0) {
                for (int i=0; i<hub->subscriptions[0]->n_string_messages ; i++) {
                    free(hub->subscriptions[0]->string_messages[i]);
                }
            }
            free(hub->subscriptions[0]->string_messages);
            free(hub->subscriptions[0]);
            return EVENT_OK;
        } else {
            int idx = find_subscription_index(hub, id, topic);
            if (idx < 0) {
                return EVENT_NOT_SUBSCRIBED;
            }
            if (hub->subscriptions[idx]->n_string_messages > 0) {
                for (int i=0; i<hub->subscriptions[idx]->n_string_messages ; i++) {
                    free(hub->subscriptions[idx]->string_messages[i]);
                }
            }
            free(hub->subscriptions[idx]->string_messages);
            free(hub->subscriptions[idx]);
            subscription** subscriptions = malloc((hub->n_subscriptions-1) * sizeof(subscription*));
            for (int i=0; i<idx; i++) {
                subscriptions[i] = malloc(sizeof(subscription));
                subscriptions[i] = hub->subscriptions[i];
            }
            for (int i=idx+1; i<hub->n_subscriptions; i++) {
                subscriptions[i] = malloc(sizeof(subscription));
                subscriptions[i] = hub->subscriptions[i];
            }
            hub->subscriptions = realloc(hub->subscriptions, (--hub->n_subscriptions) * sizeof(subscription*));
            for (int i=0; i<hub->n_subscriptions; i++) {
                hub->subscriptions[i] = subscriptions[i];
            }
            free(subscriptions);
            return EVENT_OK;
        }
    } else {
        return EVENT_NOT_SUBSCRIBED;
    }

}

char *event_read_string_message(event_hub *hub, int id, char *topic) {
    if (hub == NULL) {
        return NULL;
    }

    if (hub->topics == NULL || hub->n_topics == 0) {
        return NULL;
    } else {
        // check if topic exists
        char *sub_topic = NULL;
        for (int i=0; i<hub->n_topics; i++) {
            if (strcmp(hub->topics[i], topic) == 0) {
                sub_topic = hub->topics[i];
            }
        }
        if (sub_topic == NULL) {
            return NULL;
        }
        if (hub->subscriptions == NULL || hub->n_subscriptions == 0) {
            return NULL;
        } else {
            int idx = find_subscription_index(hub, id, topic);
            if (idx < 0) {
                return NULL;
            }
            subscription* sub = hub->subscriptions[idx];
            if (sub->n_string_messages > 0) {
                char *message = malloc((1 + strlen(sub->string_messages[0])) * sizeof(char));
                strcpy(message, sub->string_messages[0]);
                if (sub->n_string_messages == 1) {
                    free(sub->string_messages[0]);
                    free(sub->string_messages);
                    sub->n_string_messages = 0;
                    return message;
                }
                char** messages = malloc((sub->n_string_messages-1) * sizeof(char*));
                for (int j=0; j<sub->n_string_messages-1; j++) {
                    messages[j] = malloc((1 + strlen(sub->string_messages[j+1])) * sizeof(char));
                    strcpy(messages[j], sub->string_messages[j+1]);
                }
                free(sub->string_messages[0]);
                sub->string_messages = realloc(sub->string_messages, --sub->n_string_messages * sizeof(char*));
                for (int j=0; j<sub->n_string_messages; j++) {
                    sub->string_messages[j] = malloc((1 + strlen(messages[j])) * sizeof(char));
                    strcpy(sub->string_messages[j], messages[j]);
                    free(messages[j]);
                }
                free(messages);
                return message;
            } else {
                return NULL;
            }
        }
    }
}

binary_message *event_read_binary_message(event_hub *hub, int id, const char *topic) {
    if (hub == NULL) {
        return NULL;
    }

    if (hub->topics == NULL || hub->n_topics == 0) {
        return NULL;
    } else {
        // check if topic exists
        char *sub_topic = NULL;
        for (int i=0; i<hub->n_topics; i++) {
            if (strcmp(hub->topics[i], topic) == 0) {
                sub_topic = hub->topics[i];
            }
        }
        if (sub_topic == NULL) {
            return NULL;
        }
        if (hub->subscriptions == NULL || hub->n_subscriptions == 0) {
            return NULL;
        } else {
            int idx = find_subscription_index(hub, id, topic);
            if (idx < 0) {
                return NULL;
            }
            subscription* sub = hub->subscriptions[idx];
            if (sub->n_binary_messages > 0) {
                binary_message *message = malloc(sizeof(binary_message));
                message->data = malloc(sub->binary_messages[0]->length * sizeof(uint8_t));
                message->length = sub->binary_messages[0]->length;
                memcpy(message->data, sub->binary_messages[0]->data, sub->binary_messages[0]->length);
                if (sub->n_binary_messages == 1) {
                    free(sub->binary_messages[0]->data);
                    free(sub->binary_messages[0]);
                    sub->n_binary_messages = 0;
                    return message;
                }
                binary_message** messages = malloc((sub->n_binary_messages-1) * sizeof(binary_message*));
                for (int j=0; j<sub->n_binary_messages-1; j++) {
                    messages[j] = malloc(sizeof(binary_message));
                    messages[j]->data = malloc(sub->binary_messages[j+1]->length * sizeof(uint8_t));
                    messages[j]->length = sub->binary_messages[j+1]->length;
                    memcpy(messages[j]->data, sub->binary_messages[j+1]->data, sub->binary_messages[j+1]->length);
                }
                sub->binary_messages = realloc(sub->binary_messages, --sub->n_binary_messages * sizeof(binary_message*));
                for (int j=0; j<sub->n_binary_messages; j++) {
                    sub->binary_messages[j]->data = realloc(sub->binary_messages[j]->data, messages[j]->length * sizeof(uint8_t));
                    sub->binary_messages[j]->length = messages[j]->length;
                    memcpy(sub->binary_messages[j]->data, messages[j]->data, messages[j]->length);
                    free(messages[j]->data);
                    free(messages[j]);
                }
                free(messages);
                return message;
            } else {
                return NULL;
            }
        }
    }
}

int event_publish_string_message(event_hub *hub, const char *topic, const char *message) {
    if (hub == NULL) {
        return EVENT_NO_HUB;
    }

    if (hub->topics == NULL || hub->n_topics == 0) {
        return EVENT_NO_TOPIC;
    } else {
        // check if topic exists
        char *sub_topic = NULL;
        for (int i=0; i<hub->n_topics; i++) {
            if (strcmp(hub->topics[i], topic) == 0) {
                sub_topic = malloc((1 + strlen(hub->topics[i]))*sizeof(char));
                strcpy(sub_topic, hub->topics[i]);
            }
        }
        if (sub_topic == NULL) {
            return EVENT_NO_TOPIC;
        }
        if (hub->subscriptions == NULL || hub->n_subscriptions == 0) {
            return EVENT_OK;
        } else {
            for (int i=0; i<hub->n_subscriptions; i++) {
                if (strcmp(hub->subscriptions[i]->topic, sub_topic) == 0) {
                    subscription* sub = hub->subscriptions[i];
                    // found subscription
                    if (sub->string_messages == NULL || sub->n_string_messages == 0) {
                        sub->string_messages = malloc(1*sizeof(char*));
                        sub->string_messages[0] = malloc((1 + strlen(message)) * sizeof(char));
                        strcpy(sub->string_messages[0], message);
                        sub->n_string_messages = 1;
                        continue;
                    } else {
                        sub->string_messages = realloc(sub->string_messages, ++sub->n_string_messages * sizeof(char*));
                        sub->string_messages[sub->n_string_messages-1] = malloc((1 + strlen(message)) * sizeof(char));
                        strcpy(sub->string_messages[sub->n_string_messages-1], message);
                    }
                }
            }
            free(sub_topic);
        }
    }
    return EVENT_OK;
}

int event_publish_binary_message(event_hub *hub, const char *topic, binary_message *message) {
    if (hub == NULL) {
        return EVENT_NO_HUB;
    }

    if (hub->topics == NULL || hub->n_topics == 0) {
        return EVENT_NO_TOPIC;
    } else {
        // check if topic exists
        char *sub_topic = NULL;
        for (int i=0; i<hub->n_topics; i++) {
            if (strcmp(hub->topics[i], topic) == 0) {
                sub_topic = malloc((1 + strlen(hub->topics[i]))*sizeof(char));
                strcpy(sub_topic, hub->topics[i]);
            }
        }
        if (sub_topic == NULL) {
            return EVENT_NO_TOPIC;
        }
        if (hub->subscriptions == NULL || hub->n_subscriptions == 0) {
            return EVENT_OK;
        } else {
            for (int i=0; i<hub->n_subscriptions; i++) {
                if (strcmp(hub->subscriptions[i]->topic, sub_topic) == 0) {
                    subscription* sub = hub->subscriptions[i];
                    // found subscription
                    if (sub->binary_messages == NULL || sub->n_binary_messages == 0) {
                        sub->binary_messages = malloc(1*sizeof(binary_message*));
                        sub->binary_messages[0] = malloc(sizeof(binary_message));
                        sub->binary_messages[0]->data = malloc(message->length * sizeof(uint8_t));
                        memcpy(sub->binary_messages[0]->data, message->data, message->length);
                        sub->binary_messages[0]->length = message->length;
                        sub->n_binary_messages = 1;
                        continue;
                    } else {
                        // add the new message
                        sub->binary_messages = realloc(sub->binary_messages, ++sub->n_binary_messages * sizeof(binary_message*));
                        sub->binary_messages[sub->n_binary_messages-1] = malloc(sizeof(binary_message));
                        sub->binary_messages[sub->n_binary_messages-1]->data = malloc(message->length * sizeof(uint8_t));
                        memcpy(sub->binary_messages[sub->n_binary_messages-1]->data, message->data, message->length);
                        sub->binary_messages[sub->n_binary_messages-1]->length = message->length;
                    }
                }
            }
            free(sub_topic);
        }
    }
    return EVENT_OK;
}

void event_print_binary_message(binary_message *message) {
    printf("{");
    if (message != NULL) {
        for (int j=0; j<message->length; j++) {
            if (j == message->length-1) {
                printf("%d", message->data[j]);
            } else {
                printf("%d, ", message->data[j]);
            }
        }
    }
    printf("}\n");
}

int event_list_binary_messages(event_hub *hub, int id, char *topic) {
    if (hub == NULL) {
        return EVENT_NO_HUB;
    }
    if (hub->subscriptions == NULL || hub->n_subscriptions == 0) {
        return EVENT_OK;
    } else {
        int idx = find_subscription_index(hub, id, topic);
        if (idx < 0) {
            return EVENT_NOT_SUBSCRIBED;
        }
        subscription* sub = hub->subscriptions[idx];
        if (sub == NULL) {
            return EVENT_OK;
        }
        if (sub->n_binary_messages > 0) {
            printf("found %d binary messages\n", sub->n_binary_messages);
            for (int i=0; i<sub->n_binary_messages; i++) {
                printf("binary message: {");
                event_print_binary_message(sub->binary_messages[i]);
            }
        }
    }
    return EVENT_OK;
}


int event_list_topics(event_hub *hub) {
    if (hub == NULL) {
        return EVENT_NO_HUB;
    }

    if (hub->topics == NULL || hub->n_topics == 0) {
        return EVENT_NO_TOPIC;
    } else {
        for (int i=0; i<hub->n_topics; i++) {
            printf("%s\n", hub->topics[i]);
        }
    }
    return EVENT_OK;
}

int event_add_topic(event_hub *hub, const char *topic) {
    if (hub == NULL) {
        return EVENT_NO_HUB;
    }
    if (hub->n_topics == 0) {
        hub->topics = malloc(sizeof(char*));
        hub->topics[0] = malloc((1 + strlen(topic)) * sizeof(char));
        strcpy(hub->topics[0], topic);
        hub->n_topics = 1;
    } else {
        // first check for duplicates
        for (int i=0; i<hub->n_topics; i++) {
            if (strcmp(hub->topics[i], topic) == 0) {
                return EVENT_DUPLICATE_TOPIC;
            }
        }
        // add the new topic
        char** topics = malloc((1+hub->n_topics) * sizeof(char*));
        for (int i=0; i<hub->n_topics; i++) {
            topics[i] = malloc((1 + strlen(hub->topics[i])) * sizeof(char));
            strcpy(topics[i], hub->topics[i]);
        }
        topics[hub->n_topics] = malloc((1 + strlen(topic)) * sizeof(char));
        strcpy(topics[hub->n_topics], topic);
        hub->topics = realloc(hub->topics, ++hub->n_topics * sizeof(char*));
        for (int i=0; i<hub->n_topics; i++) {
            if (i == hub->n_topics-1) {
                hub->topics[i] = malloc((1 + strlen(topics[i])) * sizeof(char));
            } else {
                hub->topics[i] = realloc(hub->topics[i], (1 + strlen(topics[i])) * sizeof(char));
            }
            strcpy(hub->topics[i], topics[i]);
            free(topics[i]);
        }
        free(topics);
    }
    return EVENT_OK;
}

int event_destroy(event_hub *hub) {
    if (hub == NULL) {
        return EVENT_NO_HUB;
    }
    if (hub->subscriptions != NULL && hub->n_subscriptions > 0) {
        for (int i=0; i<hub->n_subscriptions; i++) {
            if (hub->subscriptions[i]->string_messages != NULL && hub->subscriptions[i]->n_string_messages > 0) {
                for (int j=0; j<hub->subscriptions[i]->n_string_messages; j++) {
                    free(hub->subscriptions[i]->string_messages[j]);
                }
                free(hub->subscriptions[i]->string_messages);
            }
            if (hub->subscriptions[i]->binary_messages != NULL && hub->subscriptions[i]->n_binary_messages > 0) {
                for (int j=0; j<hub->subscriptions[i]->n_binary_messages; j++) {
                    if (hub->subscriptions[i]->binary_messages[j]->data != NULL) {
                        free(hub->subscriptions[i]->binary_messages[j]->data);
                    }
                    free(hub->subscriptions[i]->binary_messages[j]);
                }
                free(hub->subscriptions[i]->string_messages);
                free(hub->subscriptions[i]->binary_messages);
            }
            free(hub->subscriptions[i]);
        }
    }
    if (hub->topics != NULL && hub->n_topics > 0) {
        for (int i=0; i<hub->n_topics; i++) {
            free(hub->topics[i]);
        }
    }
    return EVENT_OK;
}

int event_list_subscriptions(event_hub *hub) {
    if (hub == NULL) {
        return EVENT_NO_HUB;
    }

    if (hub->subscriptions == NULL || hub->n_subscriptions == 0) {
        return EVENT_OK;
    } else {
        for (int i=0; i<hub->n_subscriptions; i++) {
            printf("%ld: %s\n", hub->subscriptions[i]->id, hub->subscriptions[i]->topic);
        }
    }
    return EVENT_OK;
}


int event_list_string_messages(event_hub *hub, int id, char *topic) {
    if (hub == NULL) {
        return EVENT_NO_HUB;
    }
    if (hub->subscriptions == NULL || hub->n_subscriptions == 0) {
        return EVENT_OK;
    } else {
        subscription* sub = NULL;
        for (int i=0; i<hub->n_subscriptions; i++) {
            if (strcmp(hub->subscriptions[i]->topic, topic) == 0 && hub->subscriptions[i]->id == id) {
                sub = hub->subscriptions[i];
            }
        }
        if (sub == NULL) {
            return EVENT_OK;
        }
        if (sub->n_string_messages > 0) {
            printf("found %d string_messages\n", sub->n_string_messages);
            for (int i=0; i<sub->n_string_messages; i++) {
                printf("message: '%s'\n", sub->string_messages[i]);
            }
        }
    }
    return EVENT_OK;
}
