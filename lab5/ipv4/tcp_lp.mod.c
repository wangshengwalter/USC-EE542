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
	{ 0x160b6de4, "tcp_reno_undo_cwnd" },
	{ 0x9bcdcaf4, "tcp_reno_ssthresh" },
	{ 0xeb1ad7b, "tcp_unregister_congestion_control" },
	{ 0xaca82706, "tcp_register_congestion_control" },
	{ 0x837b7b09, "__dynamic_pr_debug" },
	{ 0x464aae8a, "tcp_reno_cong_avoid" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "1135159624B0C793EBE49D2");
