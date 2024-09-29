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
	{ 0x9744b7c8, "inet_diag_unregister" },
	{ 0x91bdce1f, "inet_diag_register" },
	{ 0xf6b3e67b, "inet_diag_bc_sk" },
	{ 0x49c41a57, "_raw_spin_unlock_bh" },
	{ 0xb3635b01, "_raw_spin_lock_bh" },
	{ 0x95a67b07, "udp_table" },
	{ 0xd4d1983c, "udplite_table" },
	{ 0xc5850110, "printk" },
	{ 0x9ef9fec2, "kfree_skb" },
	{ 0x9e9ad7bf, "__udp4_lib_lookup" },
	{ 0x5536fc57, "__udp6_lib_lookup" },
	{ 0x78210a0f, "netlink_unicast" },
	{ 0xbecce6d, "inet_sk_diag_fill" },
	{ 0xddff305b, "netlink_net_capable" },
	{ 0x1a4fda3c, "__alloc_skb" },
	{ 0x5f17ffe4, "sk_free" },
	{ 0x165b145c, "ex_handler_refcount" },
	{ 0xe297de3b, "sock_diag_check_cookie" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=inet_diag";


MODULE_INFO(srcversion, "613F50E0AE7DE5AF31EC7E9");
