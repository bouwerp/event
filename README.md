# Event

A simple library implementation of an event hub in C. Topics are created to which 
subscribers can subscribe, and to which messages can be published.

Each subscription handles string and binary messages separately. The list of messages for
each subscription is regarded as a queue, i.e. FIFO. When read, the oldest message in the
list is returned, and removed from the list.

# Usage

A few usage examples are given in the snippet below:

```c
    event_hub hub = NEW_EVENT_HUB;

    event_add_topic(&hub, "topic1");
    event_list_topics(&hub);

    int s0 = event_subscribe(&hub, "topic1");
    int s1 = event_subscribe(&hub, "topic1");
    int s2 = event_subscribe(&hub, "topic1");
    
    event_list_subscriptions(&hub);
    
    event_unsubscribe(&hub, s2, "topic1");
    
    event_list_subscriptions(&hub);

    event_publish_string_message(&hub, "topic1", "message1");
    event_publish_string_message(&hub, "topic1", "message2");

    event_list_string_messages(&hub, s0, "topic1");
    
    char *m = event_read_string_message(&hub, s0, "topic1");
    printf("message '%s'\n", m);
    free(m);
    
    m = event_read_string_message(&hub, s0, "topic1");
    printf("read message '%s'\n", m);
    free(m);
    
    event_list_string_messages(&hub, s0, "topic1");
    
    event_list_string_messages(&hub, s1, "topic1");

    binary_message d0 = NEW_BINARY_MESSAGE;
    d0.length = 11;
    uint8_t data0[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110};
    d0.data = data0;
    event_publish_binary_message(&hub, "topic1", &d0);
    
    binary_message d1 = NEW_BINARY_MESSAGE;
    d1.length = 5;
    uint8_t data1[] = {1, 2, 3, 4, 5};
    d1.data = data1;
    event_publish_binary_message(&hub, "topic1", &d1);

    binary_message *rb0 = event_read_binary_message(&hub, 0, "topic1");
    event_print_binary_message(rb0);
    free(rb0);

    binary_message *rb1 = event_read_binary_message(&hub, 0, "topic1");
    event_print_binary_message(rb1);
    free(rb1);

    binary_message *rb2 = event_read_binary_message(&hub, 0, "topic1");
    event_print_binary_message(rb2);
    free(rb2);

    event_list_binary_messages(&hub, 1,"topic1");

    // always destroy the hub
    event_destroy(&hub);
```