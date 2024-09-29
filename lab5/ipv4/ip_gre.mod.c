#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

MODULE_INFO(intree, "Y");

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xa29affe8, "module_layout" },
	{ 0xc4f0da12, "ktime_get_with_offset" },
	{ 0xb93e600b, "register_pernet_device" },
	{ 0x29dd9cfb, "ip_tunnel_get_stats64" },
	{ 0x44b60279, "ip_tunnel_get_iflink" },
	{ 0xc806fbe5, "dst_release" },
	{ 0x184ffb5b, "metadata_dst_alloc" },
	{ 0x83e58dc6, "ipv4_update_pmtu" },
	{ 0x69eda8d0, "ip_tunnel_lookup" },
	{ 0x57564ece, "__ip_tunnel_change_mtu" },
	{ 0x15fb2909, "param_ops_bool" },
	{ 0xc696cb9f, "dst_cache_get_ip4" },
	{ 0xdc20d93c, "__icmp_send" },
	{ 0x4c161203, "ip_tunnel_xmit" },
	{ 0x15ba50a6, "jiffies" },
	{ 0x7c1370a4, "inetdev_by_index" },
	{ 0x86dff270, "__pskb_pull_tail" },
	{ 0xb44ad4b3, "_copy_to_user" },
	{ 0xb6603044, "__iptunnel_pull_header" },
	{ 0x32d94065, "ip_tunnel_setup" },
	{ 0x28477b6b, "ip_tunnel_dellink" },
	{ 0xd5ec2f80, "skb_checksum" },
	{ 0x9fdecc31, "unregister_netdevice_many" },
	{ 0x56bc790d, "ip_tunnel_delete_nets" },
	{ 0x67ad239f, "gre_add_protocol" },
	{ 0xa2dcece7, "ip_tunnel_newlink" },
	{ 0xc5850110, "printk" },
	{ 0x94bb977a, "unregister_pernet_device" },
	{ 0xd30de73e, "ip_tunnel_uninit" },
	{ 0xcbeb12e7, "free_netdev" },
	{ 0xc4dac42f, "nla_put" },
	{ 0x23274e1e, "gre_del_protocol" },
	{ 0xf4271b01, "skb_push" },
	{ 0xe703ca99, "iptunnel_xmit" },
	{ 0xc9eae493, "ip_tunnel_init" },
	{ 0xa0a41109, "ip_tunnel_get_link_net" },
	{ 0xf74c780f, "skb_pull" },
	{ 0x5525ef5e, "rtnl_link_unregister" },
	{ 0xbef77b3, "ip_tunnel_ioctl" },
	{ 0xc2eb7af6, "ip_tunnel_changelink" },
	{ 0x788058c7, "rtnl_configure_link" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x9ef9fec2, "kfree_skb" },
	{ 0xa99d147f, "ipv4_redirect" },
	{ 0xe0e3c7d8, "ip_mc_inc_group" },
	{ 0xd1d6d87d, "pskb_expand_head" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xba192380, "ether_setup" },
	{ 0xf6acd52d, "ip_tunnel_init_net" },
	{ 0xd2e020e, "iptunnel_handle_offloads" },
	{ 0x8bdc32d0, "ip_route_output_flow" },
	{ 0xc2b8d201, "rtnl_create_link" },
	{ 0x2c0972ec, "eth_validate_addr" },
	{ 0xcfb1fd08, "ip6_err_gen_icmpv6_unreach" },
	{ 0x7910fd7d, "___pskb_trim" },
	{ 0x2817b8f4, "rtnl_link_register" },
	{ 0xe0855c59, "dst_cache_set_ip4" },
	{ 0x93c090fa, "ip_tunnel_encap_setup" },
	{ 0xe113bbbc, "csum_partial" },
	{ 0x31d67132, "ip_tunnel_change_mtu" },
	{ 0x294d7f72, "eth_mac_addr" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0xada85f6b, "ip_mc_dec_group" },
	{ 0x57f5faa8, "gre_parse_header" },
	{ 0xd10b08ea, "ip_tunnel_rcv" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=ip_tunnel,gre";


MODULE_INFO(srcversion, "155D973D2C3746C2BF98AD9");
