#include <linux/stddef.h>
//#include <linux/hashtable.h>
//#include <linux/crc32.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/inet.h>

/*
 * Included in 464_tables.h
 * 
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <uapi/linux/in.h>
#include <uapi/linux/in6.h>
*/

#include "464_tables.h"

#define VERBOSE_464P2P

/********************************
 * Init functions for XLAT table
 ********************************/

struct xlat_entry out_addr_cache;
struct xlat_entry in_addr_cache;

struct in_addr *in4;
struct in6_addr *in6;

// setup pton buffers
char buf[4];
long lval;
uint32_t wrk;
int pton_i;



//DECLARE_HASHTABLE(xlat_46, 16);
//DECLARE_HASHTABLE(xlat_64, 16);
LIST_HEAD(xlat_local);
LIST_HEAD(xlat_remote);

int init_tables()
{
    //hash_init(xlat_46);
    //hash_init(xlat_64);
    

    return 0;
    
}

int cleanup_tables()
{
    /*if(ctl_table_header!=NULL){
        unregister_sysctl_table(ctl_table_header);
    }*/
    return 0;
}

/********************************
 * XLAT Tables
 ********************************/
//###########hgHJGKG########
/**
 * Function to parse XLAT Entry from string
 */
/*int xlat_add(char xlat_str[]){
    
    char addr_4[16];
    char addr_6[40];
    
    const char *end;
    
    strncpy(addr_6,xlat_str,39);
    
    in6 = kzalloc(sizeof(struct in6_addr),GFP_ATOMIC);
    
    //int in6_pton(const char *src, int srclen,u8 *dst,int delim, const char **end)
    // http://lxr.free-electrons.com/source/net/core/utils.c#L186
    //if(in6_pton(addr_6,in6)!=0){
    if(in6_pton(xlat_str,39,in6,':',*end)!=1){
        kfree(in6);
        return -1;
    }
    
    
    strncpy(addr_4,xlat_str+40,15);
    
    in4 = kzalloc(sizeof(struct in_addr),GFP_ATOMIC);
    
    //int in4_pton(const char *src, int srclen, u8 *dst, int delim, const char **end)
    // http://lxr.free-electrons.com/source/net/core/utils.c#L120
    //if(in4_pton(addr_4,in4)!=0){
    if(in4_pton(xlat_str+40,15,in4,'.',*end)!=1){
        kfree(in6);
        kfree(in4);
        return -2;
    }
    
    remote_xlat_add(in6, in4);
    return 0;
    
}*/

int xlat_add(void){
    
    //char stat_addr_6[] = {0x76,0x28,0x35,0x9e,0xfa,0x67,0x44,0x25,0x00,0x00,0x25,0xc9,0xb2,0x6d,0x16,0xfd}; // Little Endian Byte Ordered
    //char stat_addr_6[] = {0x9e,0x35,0x28,0x76,0x25,0x44,0x67,0xfa,0xc9,0x25,0x00,0x00,0xfd,0x16,0x6d,0xb2}; // Little Endian Word Ordered
    //char stat_addr_6[] = {0xfd,0x16,0x6d,0xb2,0xc9,0x25,0x00,0x00,0x25,0x44,0x67,0xfa,0x9e,0x35,0x28,0x76}; // Network Order <== WORKS
    char stat_addr_6[] = {0xfd,0x16,0x6d,0xb2,0xc9,0x25,0x00,0x00,0x25,0x44,0x67,0xfa,0x9e,0x35,0x28,0x74}; // Alt Address
    
    in6 = kzalloc(sizeof(struct in6_addr),GFP_ATOMIC);
    memcpy(in6,stat_addr_6,16);
    
    //char stat_addr_4[] = {1,254,168,192}; // Little Endian Byte Ordered
    //char stat_addr_4[] = {192,168,254,001}; // Network Order <== WORKS
    char stat_addr_4[] = {192,168,254,002}; // Alt Address
    
    in4 = kzalloc(sizeof(struct in_addr),GFP_ATOMIC);
    memcpy(in4,stat_addr_4,4);
    
    remote_xlat_add(in6, in4);
    return 0;
    
}


/**
 * Function to add entry to the remote xlat table
 */
int remote_xlat_add(struct in6_addr *remote_6_addr, struct in_addr *remote_4_addr){
    
    /*struct xlat_entry new_entry_46 = {
        .in6 = remote_6_addr,
        .in4 = remote_4_addr,
        .hash_list_data = 0 // initialised when added to hashtable
    };
    
    //hash_add(xlat_46, &new_entry_46.hash_list_data, *new_entry_46.in4);
    hash_add(xlat_46, &new_entry_46.hash_list_data, crc32(0, new_entry_46.in4, 16));
    
    struct xlat_entry new_entry_64 = {
        .in6 = remote_6_addr,
        .in4 = remote_4_addr,
        .hash_list_data = 0 // initialised when added to hashtable
    };
    
    hash_add(xlat_64, &new_entry_64.hash_list_data, *new_entry_64.in6);
    */
    
    struct host_entry *new_entry = kzalloc(sizeof(struct host_entry),GFP_ATOMIC);
    new_entry->in6 = remote_6_addr;
    new_entry->in4 = remote_4_addr;
    INIT_LIST_HEAD(&new_entry->linked_list_data);
    
    list_add(&new_entry->linked_list_data,&xlat_remote);
    
    return 0;
    
}


/**
 * Function to add entry to the local xlat table
 */
int local_xlat_add(struct in6_addr *local_6_addr, struct in_addr *local_4_addr){
    
    struct host_entry *new_entry = kzalloc(sizeof(struct host_entry),GFP_ATOMIC);
    new_entry->in6 = local_6_addr;
    new_entry->in4 = local_4_addr;
    INIT_LIST_HEAD(&new_entry->linked_list_data);
    
    list_add(&new_entry->linked_list_data,&xlat_local);
    
    return 0;
    
}


/********************************
 * Lookup Functions
 ********************************/


/**
 * Function to translate local IPv6 Address to its local IPv4 address
 */
struct in_addr * local_64_xlat(struct in6_addr *local_6_addr){
    
    //list lookup
    struct host_entry *current_host; // Pointer to current position in XLAT list
    list_for_each_entry(current_host, &xlat_local, linked_list_data){
        
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] IN; My Addr %pI6 , Pkt Addr %pI6 .\n",current_host->in6,local_6_addr);
        #endif
        
        // If match return pointer to corresponding IPv4 address
        if(!memcmp(current_host->in6,local_6_addr,sizeof(struct in6_addr))){
            //TODO Move to head
            return current_host->in4;
        }
    }
    
    return NULL;  //No match found
}

/**
 * Function to translate local IPv6 Address to its local IPv4 address
 */
struct in6_addr * local_46_xlat(struct in_addr *local_4_addr){
    //list lookup
    struct host_entry *current_host; // Pointer to current position in XLAT list
    list_for_each_entry(current_host, &xlat_local, linked_list_data){
        
        //#ifdef VERBOSE_464P2P
        //    printk(KERN_INFO "[464P2P] 46X; %p=%x, %p=%x, %p\n", current_host->in4, *(current_host->in4), local_4_addr, *local_4_addr, current_host->linked_list_data.next);
        //#endif
        
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] OUT; My Addr %pI4 , Pkt Addr %pI4 .\n",current_host->in4,local_4_addr);
        #endif
        
        // If match return pointer to corresponding IPv4 address
        if(!memcmp(current_host->in4,local_4_addr,sizeof(struct in_addr))){
            //TODO Move to head
            return current_host->in6;
        }
    }
    
    return NULL;  //No match found
}

/**
 * Function to translate remote IPv6 Address to a corresponding remote IPv4 address
 */
/*struct in_addr * remote_64_xlat(struct in6_addr *remote_6_addr){
    
    //hash lookup
    struct xlat_entry *current_host; // Pointer to current position in XLAT table
    hash_for_each_possible(xlat_64, current_host, hash_list_data, *remote_6_addr){
        
        // If match return pointer to corresponding IPv4 address
        if(!memcmp(current_host->in6,remote_6_addr,sizeof(struct in6_addr))){
            //TODO Move to head
            return current_host->in4;
        }
    }
    
    return NULL;  //No match found
}*/

/**
 * Function to translate remote IPv6 Address to a corresponding remote IPv4 address
 */
/*struct in6_addr * remote_46_xlat(struct in_addr *remote_4_addr){
    
    //hash lookup
    struct xlat_entry * current_host; // Pointer to current position in XLAT table
    hash_for_each_possible(xlat_46, current_host, hash_list_data, *remote_4_addr){
        
        // If match return pointer to corresponding IPv4 address
        if(!memcmp(current_host->in4,remote_4_addr,sizeof(struct in_addr))){
            //TODO Move to head
            return current_host->in6;
        }
    }
    
    return NULL;  //No match found
    
    //struct in_addr *remote_4_addr;
    //in4_pton("192.168.5.1",remote_4_addr);
    //return remote_4_addr;
}*/

/**
 * Function to translate remote IPv6 Address to its remote IPv4 address
 */
struct in_addr * remote_64_xlat(struct in6_addr *remote_6_addr){
    
    //list lookup
    struct host_entry *current_host; // Pointer to current position in XLAT list
    list_for_each_entry(current_host, &xlat_remote, linked_list_data){
        
        //#ifdef VERBOSE_464P2P
        //    printk(KERN_INFO "[464P2P] 64X; %p=%x%x%x%x, %p=%x%x%x%x, %p\n", current_host->in6, current_host->in6[0],current_host->in6[1],current_host->in6[2],current_host->in6[3], remote_6_addr, remote_6_addr[0],remote_6_addr[1],remote_6_addr[2],remote_6_addr[3], current_host->linked_list_data.next);
        //#endif
        
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] IN; My Addr %pI6 , Pkt Addr %pI6 .\n",current_host->in6,remote_6_addr);
        #endif
        
        // If match return pointer to corresponding IPv4 address
        if(!memcmp(current_host->in6,remote_6_addr,sizeof(struct in6_addr))){
            //TODO Move to head
            return current_host->in4;
        }
    }
    
    return NULL;  //No match found
}

/**
 * Function to translate remote IPv4 Address to its remote IPv6 address
 */
struct in6_addr * remote_46_xlat(struct in_addr *remote_4_addr){
    
    //list lookup
    struct host_entry * current_host; // Pointer to current position in XLAT list
    list_for_each_entry(current_host, &xlat_remote, linked_list_data){
        
        //#ifdef VERBOSE_464P2P
        //    printk(KERN_INFO "[464P2P] 46X; %p=%x, %p=%x, %p\n", current_host->in4, *(current_host->in4), remote_4_addr, *remote_4_addr, current_host->linked_list_data.next);
        //#endif
        
        #ifdef VERBOSE_464P2P
            printk(KERN_INFO "[464P2P] OUT; My Addr %pI4 , Pkt Addr %pI4 .\n",current_host->in4,remote_4_addr);
        #endif
        
        // If match return pointer to corresponding IPv4 address
        if(!memcmp(current_host->in4,remote_4_addr,sizeof(struct in_addr))){
            //TODO Move to head
            return current_host->in6;
        }
    }
    
    return NULL;  //No match found
}


/********************************
 * pton Functions
 ********************************/


/**
 * Function to convert dotted decimal IPv4 address string to in_addr structure
 *
int in4_pton(char *str, struct in_addr *target_addr){
    
    //Test for proper addr len
    if(strlen(str)!=15){
        return 1;  //Unable to convert
    }
    
    // setup buffers
    lval = 0;
    wrk = 0;
    
    // convert octet d
    strncpy(buf,&str[12],3);
    kstrtol(buf,10,&lval);
    if (lval<0){
        lval=0;
    }
    wrk += (uint32_t) lval;
     wrk <<= 8; // shift octet over to make room for next one
    
        // convert octet c
    strncpy(buf,&str[8],3);
    kstrtol(buf,10,&lval);
    if (lval<0){
        lval=0;
    }
    wrk += (uint32_t) lval;
    wrk <<= 8;
    
        // convert octet b
    strncpy(buf,&str[4],3);
    kstrtol(buf,10,&lval);
    if (lval<0){
        lval=0;
    }
    wrk += (uint32_t) lval;
    wrk <<= 8;
    
        // convert octet a
    strncpy(buf,&str[0],3); //extract octet chars from string
    kstrtol(buf,10,&lval); // convert to long
    if (lval<0){ // if did not convert, set octet to 0
        lval=0;
    }
    wrk += (uint32_t) lval; // add to working addr
    

    
    target_addr->s_addr = wrk;
    return 0;
    
}

/**
 * Function to convert colon-separated hexadecimal IPv6 address string to in6_addr structure
 *
int in6_pton(char *str, struct in6_addr *target_addr){
    
    //Test for proper addr len
    if(strlen(str)!=39){
        return 1;  //Unable to convert
    }

    // setup buffers
    lval = 0;
    wrk = 0;

    for(pton_i=0; pton_i<8; pton_i++){ // for each 16 bit group
        
        //top 2 bytes of group
        strncpy(buf,&str[0+(5*pton_i)],2); // extract hex chars from str
        kstrtol(buf,16,&lval); // convert to long
        if (lval<0){ // if did not convert, set byte to 0
            lval=0;
        }
        target_addr->s6_addr[(2*pton_i)] = (char) lval; // Copy byte to dest struct.

        // repeat for bottom 2 bytes of group
        strncpy(buf,&str[2+(5*pton_i)],2);
        kstrtol(buf,16,&lval);
        if (lval<0){
            lval=0;
        }
        target_addr->s6_addr[((2*pton_i)+1)] = (char) lval;

    }    

    return 0;
}*/