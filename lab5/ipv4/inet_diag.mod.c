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
	{ 0x5550adbf, "sock_diag_put_meminfo" },
	{ 0x754d539c, "strlen" },
	{ 0x12baa3da, "from_kuid_munged" },
	{ 0xf79c402d, "sock_diag_unregister" },
	{ 0xb3635b01, "_raw_spin_lock_bh" },
	{ 0xa6093a32, "mutex_unlock" },
	{ 0x165b145c, "ex_handler_refcount" },
	{ 0x1e398e2, "sock_i_ino" },
	{ 0x315c76b0, "pv_ops" },
	{ 0x15ba50a6, "jiffies" },
	{ 0x99de6a95, "skb_trim" },
	{ 0x8c4b63d4, "sock_diag_register" },
	{ 0x37befc70, "jiffies_to_msecs" },
	{ 0xc5850110, "printk" },
	{ 0x449ad0a7, "memcmp" },
	{ 0xcd279169, "nla_find" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0xc4dac42f, "nla_put" },
	{ 0x41aed6e7, "mutex_lock" },
	{ 0x78210a0f, "netlink_unicast" },
	{ 0x4c619ebc, "nla_reserve_64bit" },
	{ 0xf665f74f, "sock_load_diag_module" },
	{ 0x1a4fda3c, "__alloc_skb" },
	{ 0x49c41a57, "_raw_spin_unlock_bh" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0xe338bf0a, "sock_diag_unregister_inet_compat" },
	{ 0x9ef9fec2, "kfree_skb" },
	{ 0x2ea2c95c, "__x86_indirect_thunk_rax" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x1c9bf2e2, "inet6_lookup" },
	{ 0x203917b0, "sock_gen_put" },
	{ 0xf7dd9f37, "kmem_cache_alloc_trace" },
	{ 0xdbf17652, "_raw_spin_lock" },
	{ 0x37a0cba, "kfree" },
	{ 0xe297de3b, "sock_diag_check_cookie" },
	{ 0x8871138f, "sock_diag_save_cookie" },
	{ 0x743fafa1, "__inet_lookup_listener" },
	{ 0xdd03251c, "sock_diag_register_inet_compat" },
	{ 0xbc7c4aaf, "__nlmsg_put" },
	{ 0xff4383b7, "sock_i_uid" },
	{ 0x122904b0, "__netlink_dump_start" },
	{ 0xddff305b, "netlink_net_capable" },
	{ 0xf330148, "__inet_lookup_established" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "D74606BB2C0FBFC6DFE2306");
