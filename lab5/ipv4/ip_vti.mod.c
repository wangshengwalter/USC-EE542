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
	{ 0xa0a41109, "ip_tunnel_get_link_net" },
	{ 0x28477b6b, "ip_tunnel_dellink" },
	{ 0x44b60279, "ip_tunnel_get_iflink" },
	{ 0x29dd9cfb, "ip_tunnel_get_stats64" },
	{ 0x31d67132, "ip_tunnel_change_mtu" },
	{ 0xd30de73e, "ip_tunnel_uninit" },
	{ 0x5525ef5e, "rtnl_link_unregister" },
	{ 0x94bb977a, "unregister_pernet_device" },
	{ 0xf1f00d08, "xfrm4_protocol_deregister" },
	{ 0xe6905823, "xfrm4_tunnel_deregister" },
	{ 0x2817b8f4, "rtnl_link_register" },
	{ 0x62810141, "xfrm4_tunnel_register" },
	{ 0x2789f3, "xfrm4_protocol_register" },
	{ 0xb93e600b, "register_pernet_device" },
	{ 0xf1e395a7, "xfrm_input" },
	{ 0xe23fa6c7, "__xfrm_policy_check" },
	{ 0xdc20d93c, "__icmp_send" },
	{ 0x53569707, "this_cpu_off" },
	{ 0xc5850110, "printk" },
	{ 0x98dd17ec, "skb_scrub_packet" },
	{ 0x31f01f94, "icmpv6_send" },
	{ 0x86dff270, "__pskb_pull_tail" },
	{ 0x9ef9fec2, "kfree_skb" },
	{ 0x2ea2c95c, "__x86_indirect_thunk_rax" },
	{ 0xc806fbe5, "dst_release" },
	{ 0x3c4b6de9, "xfrm_lookup" },
	{ 0x664757b0, "__xfrm_decode_session" },
	{ 0xf6acd52d, "ip_tunnel_init_net" },
	{ 0x56bc790d, "ip_tunnel_delete_nets" },
	{ 0x83e58dc6, "ipv4_update_pmtu" },
	{ 0xf647b45c, "__xfrm_state_destroy" },
	{ 0x165b145c, "ex_handler_refcount" },
	{ 0xa99d147f, "ipv4_redirect" },
	{ 0x7bc9a3d1, "xfrm_state_lookup" },
	{ 0x69eda8d0, "ip_tunnel_lookup" },
	{ 0xc9eae493, "ip_tunnel_init" },
	{ 0xb44ad4b3, "_copy_to_user" },
	{ 0xbef77b3, "ip_tunnel_ioctl" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0x32d94065, "ip_tunnel_setup" },
	{ 0xa2dcece7, "ip_tunnel_newlink" },
	{ 0xc2eb7af6, "ip_tunnel_changelink" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0xc4dac42f, "nla_put" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=ip_tunnel,tunnel4";


MODULE_INFO(srcversion, "4370C4EC2107EB6B6516C15");
