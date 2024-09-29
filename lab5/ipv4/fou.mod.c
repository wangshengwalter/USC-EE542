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
	{ 0xe18e5460, "kmalloc_caches" },
	{ 0x1ed8b599, "__x86_indirect_thunk_r8" },
	{ 0xb93e600b, "register_pernet_device" },
	{ 0x4155d49c, "genl_register_family" },
	{ 0x14ee27bc, "genl_unregister_family" },
	{ 0x19f462ab, "kfree_call_rcu" },
	{ 0xdf566a59, "__x86_indirect_thunk_r9" },
	{ 0xa6093a32, "mutex_unlock" },
	{ 0xaff67ad8, "inet6_offloads" },
	{ 0x99de6a95, "skb_trim" },
	{ 0x1051e209, "udp_sock_create6" },
	{ 0xc6809797, "setup_udp_tunnel_sock" },
	{ 0x86dff270, "__pskb_pull_tail" },
	{ 0xbc2c9209, "udp_tunnel_sock_release" },
	{ 0x30f68e2, "ip_tunnel_encap_del_ops" },
	{ 0x9a76f11f, "__mutex_init" },
	{ 0xc5850110, "printk" },
	{ 0x449ad0a7, "memcmp" },
	{ 0x94bb977a, "unregister_pernet_device" },
	{ 0xc4dac42f, "nla_put" },
	{ 0xf4271b01, "skb_push" },
	{ 0x41aed6e7, "mutex_lock" },
	{ 0x78210a0f, "netlink_unicast" },
	{ 0xafb515fa, "__skb_get_hash" },
	{ 0xb601be4c, "__x86_indirect_thunk_rdx" },
	{ 0x1a4fda3c, "__alloc_skb" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x9ef9fec2, "kfree_skb" },
	{ 0x2ea2c95c, "__x86_indirect_thunk_rax" },
	{ 0xd1d6d87d, "pskb_expand_head" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xf7dd9f37, "kmem_cache_alloc_trace" },
	{ 0xac3201b0, "udp_flow_hashrnd" },
	{ 0xd2e020e, "iptunnel_handle_offloads" },
	{ 0xbac816ef, "inet_protos" },
	{ 0x2c2cb3e1, "inet_offloads" },
	{ 0x37a0cba, "kfree" },
	{ 0xbdda9168, "udp_set_csum" },
	{ 0x5ab79800, "genlmsg_put" },
	{ 0x51e3e688, "udp_sock_create4" },
	{ 0xe113bbbc, "csum_partial" },
	{ 0xdb223fc8, "inet_get_local_port_range" },
	{ 0x3c64a2fd, "ip_tunnel_encap_add_ops" },
	{ 0x50ee4e6, "__skb_checksum_complete" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=ip6_udp_tunnel,udp_tunnel,ip_tunnel";


MODULE_INFO(srcversion, "FEC305A4379808A88D64862");
