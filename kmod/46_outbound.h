struct sk_buff *out_skb;             //inbound packet
struct iphdr *out_hdr;             //IP header of inbound packet
struct in_addr *s_464_addr;

// On NetFilter hook triggered
unsigned int on_nf_hook_out(unsigned int hooknum, struct sk_buff **skb, const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *));