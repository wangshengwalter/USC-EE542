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
	{ 0xbf488bda, "register_netdevice" },
	{ 0x609f1c7e, "synchronize_net" },
	{ 0x31f01f94, "icmpv6_send" },
	{ 0x754d539c, "strlen" },
	{ 0x45e35089, "iptun_encaps" },
	{ 0x79aa04a2, "get_random_bytes" },
	{ 0xc7a4fbed, "rtnl_lock" },
	{ 0xc806fbe5, "dst_release" },
	{ 0x56470118, "__warn_printk" },
	{ 0xc696cb9f, "dst_cache_get_ip4" },
	{ 0x30991cfa, "neigh_destroy" },
	{ 0xdc20d93c, "__icmp_send" },
	{ 0x165b145c, "ex_handler_refcount" },
	{ 0x98dd17ec, "skb_scrub_packet" },
	{ 0x15ba50a6, "jiffies" },
	{ 0xc9ec4e21, "free_percpu" },
	{ 0xd41f5402, "cpumask_next" },
	{ 0x17de3d5, "nr_cpu_ids" },
	{ 0x2548c032, "__cpu_possible_mask" },
	{ 0x9fdecc31, "unregister_netdevice_many" },
	{ 0xc5850110, "printk" },
	{ 0x59bfe0b1, "ns_capable" },
	{ 0x62849ac7, "dev_valid_name" },
	{ 0xcbeb12e7, "free_netdev" },
	{ 0x5792f848, "strlcpy" },
	{ 0xe703ca99, "iptunnel_xmit" },
	{ 0xaf63303d, "gro_cells_receive" },
	{ 0xaddaf727, "init_net" },
	{ 0x3d301c65, "__dev_get_by_index" },
	{ 0xbd671048, "__alloc_percpu_gfp" },
	{ 0xe9474bde, "dst_cache_init" },
	{ 0xb601be4c, "__x86_indirect_thunk_rdx" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x9ef9fec2, "kfree_skb" },
	{ 0xada38766, "dst_cache_destroy" },
	{ 0x5f41f3ba, "alloc_netdev_mqs" },
	{ 0x2ea2c95c, "__x86_indirect_thunk_rax" },
	{ 0xbfdcb43a, "__x86_indirect_thunk_r11" },
	{ 0x47f90539, "eth_type_trans" },
	{ 0xd1d6d87d, "pskb_expand_head" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x149a0e3e, "netdev_state_change" },
	{ 0x9f54ead7, "gro_cells_destroy" },
	{ 0xfab21c27, "unregister_netdevice_queue" },
	{ 0x8bdc32d0, "ip_route_output_flow" },
	{ 0xf6ebc03b, "net_ratelimit" },
	{ 0xa5041ff8, "gro_cells_init" },
	{ 0x9cdfb3f7, "sysctl_fb_tunnels_only_for_init_net" },
	{ 0x69acdf38, "memcpy" },
	{ 0xe0855c59, "dst_cache_set_ip4" },
	{ 0x53569707, "this_cpu_off" },
	{ 0xe113bbbc, "csum_partial" },
	{ 0x85670f1d, "rtnl_is_locked" },
	{ 0xd542439, "__ipv6_addr_type" },
	{ 0x6e720ff2, "rtnl_unlock" },
	{ 0x67690ad3, "dev_set_mtu" },
	{ 0xe914e41e, "strcpy" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "3774C6A4F89E802F63CF567");
