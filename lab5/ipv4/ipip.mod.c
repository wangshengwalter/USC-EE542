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
	{ 0x15fb2909, "param_ops_bool" },
	{ 0xa0a41109, "ip_tunnel_get_link_net" },
	{ 0x28477b6b, "ip_tunnel_dellink" },
	{ 0x44b60279, "ip_tunnel_get_iflink" },
	{ 0x29dd9cfb, "ip_tunnel_get_stats64" },
	{ 0x31d67132, "ip_tunnel_change_mtu" },
	{ 0xd30de73e, "ip_tunnel_uninit" },
	{ 0xa8181adf, "proc_dointvec" },
	{ 0xc57c6d80, "unregister_net_sysctl_table" },
	{ 0x94bb977a, "unregister_pernet_device" },
	{ 0xe6905823, "xfrm4_tunnel_deregister" },
	{ 0x5525ef5e, "rtnl_link_unregister" },
	{ 0xf7428a76, "register_net_sysctl" },
	{ 0xaddaf727, "init_net" },
	{ 0x2817b8f4, "rtnl_link_register" },
	{ 0x62810141, "xfrm4_tunnel_register" },
	{ 0xb93e600b, "register_pernet_device" },
	{ 0xc5850110, "printk" },
	{ 0xa2dcece7, "ip_tunnel_newlink" },
	{ 0xc2eb7af6, "ip_tunnel_changelink" },
	{ 0x93c090fa, "ip_tunnel_encap_setup" },
	{ 0x19f462ab, "kfree_call_rcu" },
	{ 0xf7dd9f37, "kmem_cache_alloc_trace" },
	{ 0xe18e5460, "kmalloc_caches" },
	{ 0x4c161203, "ip_tunnel_xmit" },
	{ 0x86dff270, "__pskb_pull_tail" },
	{ 0xd624446b, "ip_md_tunnel_xmit" },
	{ 0xd2e020e, "iptunnel_handle_offloads" },
	{ 0xe23fa6c7, "__xfrm_policy_check" },
	{ 0xd10b08ea, "ip_tunnel_rcv" },
	{ 0x184ffb5b, "metadata_dst_alloc" },
	{ 0xb6603044, "__iptunnel_pull_header" },
	{ 0x9ef9fec2, "kfree_skb" },
	{ 0xf6acd52d, "ip_tunnel_init_net" },
	{ 0x56bc790d, "ip_tunnel_delete_nets" },
	{ 0xa99d147f, "ipv4_redirect" },
	{ 0x83e58dc6, "ipv4_update_pmtu" },
	{ 0x15ba50a6, "jiffies" },
	{ 0x69eda8d0, "ip_tunnel_lookup" },
	{ 0xc9eae493, "ip_tunnel_init" },
	{ 0xb44ad4b3, "_copy_to_user" },
	{ 0xbef77b3, "ip_tunnel_ioctl" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0x32d94065, "ip_tunnel_setup" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0xc4dac42f, "nla_put" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=ip_tunnel,tunnel4";


MODULE_INFO(srcversion, "ADFFDFEF349ABCF2D285142");
