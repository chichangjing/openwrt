#include "eth_stats.h"
#include "ubus_call.h"
#include "device_info.h"
#define NUMBER_LEN 26

static void print_k(int length, const char *str){
	int sp_n, m, n;
	if(0 == strlen(str)%3){
		for(sp_n = length-strlen(str)-strlen(str)/3+1; sp_n>0; sp_n--)
			printf("%c", 0x20);
		sp_n = strlen(str) + strlen(str)/3-1;
		for(m=0; m<sp_n; m++){
			if(m>0 && 3 == m%4){
				printf("%c", ',');
			}else{
				printf("%c", *str);
				str++;
			}
		}
	}else{
		for(sp_n = length-strlen(str)-strlen(str)/3; sp_n>0; sp_n--)
			printf("%c", 0x20);
		sp_n = strlen(str) + strlen(str)/3;
		n = strlen(str)%3;
		for(m=0; m<sp_n; m++){
			if(n == m%4){
				printf("%c", ',');
			}else{
				printf("%c", *str);
				str++;
			}
		}
	}
}

static void recive_eth_stats_get_config_one_all_row(struct ubus_request * req, int type, struct blob_attr *msg){
	int rem[6];
	int i=0, j=0;
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	char * tbl[_PORT_MAX][_ETH_STATS_TYPE_MAX];
	memset(tbl, 0, sizeof(tbl));
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
        i=PORT_1;
        blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
        if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
            blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
            if (blobmsg_type(lvl[3]) ==  BLOBMSG_TYPE_ARRAY){
                j=TX_BYTES;
                blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
                if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
                    blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
                    if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
                        tbl[i][j] = blobmsg_get_string(lvl[5]);
                        j++;
                    }else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
                }else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
            }else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
        }else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}
        i++;
        }
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	for(i=PORT_1; i<_PORT_MAX; i+=2){
		HIGHT_LIGHT();
		CLEAR_ROW();
		_PRINT_INFO("Port Flow Statistics:%46s[%7s]", "", port_name[1][i])
		_PRINT_INFO("    Port Flow Statistics:%46s[%7s]\n", "", port_name[1][i+1])
		UN_HIGHT_LIGHT();
		print_row(156);
		fflush(stdout);
		for(j=TX_BYTES; j<_ETH_STATS_TYPE_MAX; j+=2){
			CLEAR_ROW();
			_PRINT_INFO("%-10s:", eth_stats_type[1][j])
			print_k(NUMBER_LEN, tbl[i][j]);
			_PRINT_INFO("  %-10s:", eth_stats_type[1][j+1])
			print_k(NUMBER_LEN, tbl[i][j+1]);
			_PRINT_INFO("    %-10s:", eth_stats_type[1][j])
			print_k(NUMBER_LEN, tbl[i+1][j]);
			_PRINT_INFO("  %-10s:", eth_stats_type[1][j+1])
			print_k(NUMBER_LEN, tbl[i+1][j+1]);
			printf("\n");
			fflush(stdout);
		}
	}
	print_row(156);
	for(i=0; i<1; i++){
		printf("\n");
		CLEAR_ROW();
	}
	fflush(stdout);
}

void eth_stats_get_all_all_row(void){
	int i, j;
	void * tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		for(i=PORT_1;	i<_PORT_MAX;	i++){
			tb[1] = blobmsg_open_table(&buf, NULL);
				tb[2] = blobmsg_open_array(&buf, port_name[2][i]);
				for (j = TX_BYTES; j < _ETH_STATS_TYPE_MAX; j++)
					blobmsg_add_string(&buf, NULL, eth_stats_type[0][j]);
				blobmsg_close_array(&buf, tb[2]);
			blobmsg_close_table(&buf, tb[1]);
		}
	blobmsg_close_array(&buf, tb[0]);

	const char * path = "eth_stats";
	const char * method = "eth_stats_get";

	ubus_call(path, method, &buf, &recive_eth_stats_get_config_one_all_row);
}

static void recive_eth_stats_one(struct ubus_request *req, int type, struct blob_attr *msg){
    int i = 0, rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
        blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
        if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
            blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
            if (blobmsg_type(lvl[3]) ==  BLOBMSG_TYPE_ARRAY){
                blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
                if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
                    blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
                    if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
                        _PRINT_INFO("%23s:", eth_stats_type[1][i])
                        print_k(NUMBER_LEN,  blobmsg_get_string(lvl[5]));
                        printf("\n");
                        i++;
                    }else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
                }else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
            }else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
        }else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    print_row(50);
    fflush(stdout);
}

void eth_stats_get_one(const char * port){
    int j;
    void * tb[3];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
            for (j = TX_BYTES; j < _ETH_STATS_TYPE_MAX; j++)
                blobmsg_add_string(&buf, NULL, eth_stats_type[0][j]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char * path = "eth_stats";
    const char * method = "eth_stats_get";

    ubus_call(path, method, &buf, &recive_eth_stats_one);
}

static void receive_stats_clear_statistics(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
        if(0 == strcmp(blobmsg_get_string(lvl[1]), "0"))
            _PRINT_INFO(" %% Clear statistics successfully.\n")
        else
            _PRINT_INFO(" %% Clear statistics failed:%s!\n", blobmsg_get_string(lvl[1]))
    }else{_PRINT_ERROR("Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    fflush(stdout);
}

void eth_stats_clear(void){
    void * tb[3];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            blobmsg_add_string(&buf, "clear", "2047");
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char * path = "eth_stats";
    const char * method = "eth_stats_set";

    ubus_call(path, method, &buf, &receive_stats_clear_statistics);
}
