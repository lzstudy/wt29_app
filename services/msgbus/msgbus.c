#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "slist.h"
#include "msgbus.h"
#include "msgbus_type.h"
#include "msgbus_conf.h"
#include "common.h"

/* 遍历所有topic */
#define list_for_each_topic(topic, head)   \
            list_for_each_entry(topic, head, msgbus_topic_t, list)

/* 遍历所有client */
#define list_for_each_client(client, head)   \
            list_for_each_entry(client, head, msgbus_client_t, list)

typedef struct _msgbus_service {
    struct list_head head;                                                      /**@ topic管理链表头 */
}msgbus_service_t;

static msgbus_service_t msgbus_service;                                         /**# 消息总线服务 */


/**************************************************************************************************
 * @brief  : 查找主题并添加client
 * @param  : None
 * @return : None
**************************************************************************************************/
static msgbus_topic_t *find_topic(const char *name)
{
    msgbus_topic_t *topic;
    msgbus_service_t *service = &msgbus_service;

    /* 遍历每个topic */
    list_for_each_topic(topic, &service->head)
    {
        if(strcmp(name, topic->name) == 0)
            return topic;
    }

    return NULL;
}

/**************************************************************************************************
 * @brief  : detect_location生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static int msgbus_init(void)
{
    msgbus_service_t *service = &msgbus_service;
    list_init(&service->head);
    LOG_I("msgbus service init.");
    return 0;
}

/**************************************************************************************************
 * @brief  : 注册topic
 * @param  : None
 * @return : None
**************************************************************************************************/
static int msgbus_register_topic(const char *name)
{
    msgbus_topic_t *topic;
    msgbus_service_t *service = &msgbus_service;
    
    /* 如果易经存在返回即可 */
    topic = find_topic(name);
    if(topic) {
        LOG_I("%s : topic [%s] already exist.", __func__, name);
        return 1;
    }
    
    topic = calloc(sizeof(*topic), 1);

    /* 初始化topic */
    list_init(&topic->list);
    list_init(&topic->head_client);
    strncpy(topic->name, name, sizeof(topic->name));

    /* 插入topic管理链表 */
    list_add_before(&service->head, &topic->list);
    return 0;
}

/**************************************************************************************************
 * @brief  : 注册topic
 * @param  : None
 * @return : None
**************************************************************************************************/
static void msgbus_unregister_topic(const char *name)
{
    msgbus_topic_t *topic;

    

    /* 查找topic */
    topic = find_topic(name);
    if(!topic) {
        LOG_I("%s : topic [%s] notexist", __func__, name);
        return;
    }

#if 0
    /* 遍历链表删除client */
    list_for_each_client(client, &topic->head_client) {
        list_del(&client->list);
        free(client);
    }
#endif

    /* 删除topic */
    list_del(&topic->list);
    free(topic);
}

/**************************************************************************************************
 * @brief  : 向topic发布内容
 * @param  : None
 * @return : None
**************************************************************************************************/
static void msgbus_publish_topic(const char *name, void *data, uint16_t len)
{
    msgbus_topic_t *topic;
    msgbus_client_t *client;

    /* 查找主题 */
    topic = find_topic(name);
    if(!topic) {
        LOG_I("%s : topic [%s] not exist", __func__, name);
        return;
    }

    /* 寻找client */
    list_for_each_client(client, &topic->head_client)
    {
        if(client->cb)
            client->cb(data, len, client->priv);
    }
}

/**************************************************************************************************
 * @brief  : 消息总线订阅主题
 * @param  : None
 * @return : None
**************************************************************************************************/
static msgbus_client_t *msgbus_subscribe_topic(const char *name, msgbus_cb_t cb, void *priv)
{
    msgbus_client_t *client;
    msgbus_topic_t *topic = NULL;

    /* 如果topic不存在返回 */
    topic = find_topic(name);
    if(!topic) return NULL;

    /* 初始化client */
    client = calloc(sizeof(*client), 1);
    list_init(&client->list);
    client->topic = topic;
    client->cb = cb;
    client->priv = priv;

    /* 添加节点 */
    list_add_before(&topic->head_client, &client->list);
    return client;
}

/**************************************************************************************************
 * @brief  : 取消订阅
 * @param  : None
 * @return : None
**************************************************************************************************/
static int msgbus_unsubscribe_topic(msgbus_client_t *client)
{
    list_del(&client->list);
    free(client);
    return 0;
}

/**************************************************************************************************
 * @brief  : 取消订阅
 * @param  : None
 * @return : None
**************************************************************************************************/
static void msgbus_set_client_attr(msgbus_client_t *client, msgbus_client_attr_t attr, void *data)
{
    switch (attr)
    {
    case MBC_ATTR_NAME:
        client->name = data;
        break;
    
    default:
        break;
    }
}

/* 消息总线ops */
static const msgbus_ops_t msgbus_ops = {
    .init = msgbus_init,
    .register_topic = &msgbus_register_topic,
    .unregister_topic = msgbus_unregister_topic,
    .publish_topic = msgbus_publish_topic,
    .subscribe_topic = msgbus_subscribe_topic,
    .unsubscribe_topic = msgbus_unsubscribe_topic,
    .set_client_attr = msgbus_set_client_attr,
};

/**************************************************************************************************
 * @brief  : 消息总线实例
 * @param  : None
 * @return : None
**************************************************************************************************/
const msgbus_ops_t *msgbus(void)
{
    return &msgbus_ops;
}


/*============================================== 命令行相关 ==============================================*/
#include "lsh.h"
#include "ctype.h"

/**************************************************************************************************
 * @brief  : 打印用法
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void print_usage(void)
{
    LOG_I("Usage:");
    LOG_I("  topic ");
    LOG_I("  topic <name>");
    LOG_I("  topic -s <name>");
    LOG_I("  topic -d <name>");
}

/**************************************************************************************************
 * @brief  : 打印主题信息
 * @param  : None
 * @return : None
**************************************************************************************************/
static void print_topic_info(void)
{
    msgbus_topic_t *topic;
    msgbus_service_t *service = &msgbus_service;

    /* 遍历topic */
    printf("\n");
    list_for_each_topic(topic, &service->head)
    {
        printf("%s/ \n", topic->name);
    }

    printf("\n");
}

/**************************************************************************************************
 * @brief  : 打印client信息
 * @param  : None
 * @return : None
**************************************************************************************************/
static void print_client_info(const char *name)
{
    msgbus_topic_t *topic;
    msgbus_client_t *client;
    
    /* 查找topic */
    topic = find_topic(name);
    if(!topic) {
        LOG_I("topic [%s] not exist", name);
        return;
    }

    /* 遍历client */
    printf("\n");
    list_for_each_client(client, &topic->head_client)
    {
        LOG_I("%s ", client->name);
    }
    printf("\n");
}

/**************************************************************************************************
 * @brief  : 设置主题
 * @param  : None
 * @return : None
**************************************************************************************************/
static void topic_cmd_set(int argc, char *argv[])
{
    if(argc < 2)
        return;
    
    msgbus_register_topic(argv[2]);
}

/**************************************************************************************************
 * @brief  : 删除主题
 * @param  : None
 * @return : None
**************************************************************************************************/
static void topic_cmd_delete(int argc, char *argv[])
{
    if(argc < 2)
        return;

    msgbus_unregister_topic(argv[2]);
}

static cmd_opt_map_t topic_map_tbl[] = {
    {"-s",  &topic_cmd_set},
    {"-d",  &topic_cmd_delete},
    {NULL,  NULL}
};

/**************************************************************************************************
 * @brief  : 快速操作
 * @param  : None
 * @return : None
**************************************************************************************************/
static void lsh_publish_topic(const char *name, const char *val)
{
    int data;

    /* 按字符串发布  */
    if(!isdigit(val[0])) {
        msgbus_publish_topic(name, (void *)val, strlen(val));
        return;
    }

    /* 按数字发布 */
    data = strtoul(val, NULL, 0);
    msgbus_publish_topic(name, &data, sizeof(data));
}

/**************************************************************************************************
 * @brief  : 快速操作
 * @param  : None
 * @return : None
**************************************************************************************************/
static void topic_quick_cmd(int argc, char const *argv[])
{
    if(argc == 2) {
        print_client_info(argv[1]);
        return;
    }

    if(argc == 3) {
        lsh_publish_topic(argv[1], argv[2]);
        return;
    }
}

/**************************************************************************************************
 * @brief  : 调试命令
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_topic(int argc, char *argv[])
{
    if(argc <= 1) {
        print_topic_info();
        return;
    }

    /* 打印帮助信息 */
    if(!strcmp(argv[1], "-h")) {
        print_usage();
        return;
    }

    /* 匹配指令表 */
    if(match_opt_table(topic_map_tbl, argv[1], argc, argv))
        return;

    /* 快速指令 */
    topic_quick_cmd(argc, argv);
}