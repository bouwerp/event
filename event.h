//
// Created by Pieter Bouwer on 2019-05-11.
//

#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum {
    EVENT_OK,
    EVENT_NO_TOPIC,
    EVENT_NOT_SUBSCRIBED,
    EVENT_NO_HUB,
    EVENT_DUPLICATE_TOPIC,
} event_return_code;

typedef struct binary_message_t {
    uint8_t *data;
    long length;
} binary_message;

typedef struct subscription_t {
    long id;
    int n_string_messages;
    int n_binary_messages;
    char* topic;
    char** string_messages;
    binary_message** binary_messages;
} subscription;

typedef struct event_hub_t {
    char** topics;
    int n_topics;
    subscription** subscriptions;
    int n_subscriptions;
} event_hub;

/**
 * Creates a new subscription to a topic and returns the new subscription ID.
 * @param hub A reference to the event hub instance.
 * @param topic The topic to which a subscription is to be created.
 * @return The new subscription ID.
 */
long event_subscribe(event_hub* hub, char* topic);
/**
 * Creates a new subscription to a topic and returns the new subscription ID.
 * @param hub A reference to the event hub instance.
 * @param topic The topic to which a subscription is to be created.
 * @return The new subscription ID.
 */
int event_unsubscribe(event_hub* hub, int id, char *topic);
/**
 * Returns message at the top of the topic and removes it.
 * @param hub A pointer to the message hub.
 * @param id The subscription ID returned by @code event_subscribe @endcode.
 * @param topic The topic for which to list messages.
 * @return The message at the top of the topic. The caller must call @code free @endcode on the returned string.
 */
char *event_read_string_message(event_hub *hub, int id, char *topic);
/**
 * Prints a list of all unread string messages for a topic, for a given subscriber ID.
 * @param hub A pointer to the message hub.
 * @param id The subscription ID returned by @code event_subscribe @endcode.
 * @param topic The topic for which to list messages.
 * @return Status code.
 */
int event_list_string_messages(event_hub *hub, int id, char *topic);
/**
 * Publishes a string message to a topic. All subscriptions to the provided topic will receive the string message.
 * @param hub A pointer to the message hub.
 * @param topic The topic for which to list messages.
 * @param message The message to be published.
 * @return Status code.
 */
int event_publish_string_message(event_hub *hub, char *topic, char *message);
/**
 * Publishes a binary message to a topic. All subscriptions to the provided topic will receive the binary message.
 * @param hub A pointer to the message hub.
 * @param topic The topic to which to publish the message.
 * @param message The message to be published.
 * @return Status code.
 */
binary_message *event_read_binary_message(event_hub *hub, int id, char *topic);
/**
 * Prints a list of all unread binary messages for a topic, for a given subscriber ID.
 * @param hub A pointer to the message hub.
 * @param id The subscription ID returned by @code event_subscribe @endcode.
 * @param topic The topic for which to list messages.
 * @return Status code.
 */
int event_list_binary_messages(event_hub *hub, int id, char *topic);
/**
 * Publishes a binary message to a topic. All subscriptions to the provided topic will receive the binary message.
 * @param hub A pointer to the message hub.
 * @param topic The topic to which to publish the message.
 * @param message The message to be published.
 * @return Status code.
 */
int event_publish_binary_message(event_hub *hub, char *topic, binary_message *message);
/**
 * Adds a new topic to which subscriptions can be created and messaged published.
 * @param hub A reference to the event hub instance.
 * @param topic The topic which is to be created.
 * @return A status code.
 */
int event_add_topic(event_hub* hub, char *topic);
/**
 * Destroys the event hub, and frees all dynamically allocated memory. This must be done when no more references
 * to the hub are maintained.
 * @param hub A pointer to the event hub.
 * @return Status code.
 */
int event_destroy(event_hub* hub);
/**
 * Lists all the topics of the event hub.
 * @param hub A pointer to the event hub.
 * @return
 */
int event_list_topics(event_hub* hub);
/**
 * Lists all the subscription of the event hub.
 * @param hub A pointer to the event hub.
 * @return
 */
int event_list_subscriptions(event_hub* hub);

#endif //EVENT_H
