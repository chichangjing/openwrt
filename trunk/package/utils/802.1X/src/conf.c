#include "conf.h"
#include "radius/radius_client.h"


int conf_radius_server(struct radius_server **servers,int *num_server,const char *ip,int def_port,struct radius_server **curr_serv)
{
    struct radius_server *nserv;
    
    nserv = realloc(*servers,(*num_server + 1) * sizeof(struct radius_server));
    if(nserv == NULL)
        return -1;

    *servers = nserv;
    nserv = &nserv[*num_server];
    (*num_server)++;
    *curr_serv = nserv;

    memset(nserv,0,sizeof(struct radius_server));
    nserv->port = def_port;
    if(parse_ip_addr(ip, &nserv->addr))
        return -1;

    return 0;
}
