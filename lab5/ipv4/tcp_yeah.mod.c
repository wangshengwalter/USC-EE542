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
	{ 0xaa5cbb68, "tcp_vegas_get_info" },
	{ 0x160b6de4, "tcp_reno_undo_cwnd" },
	{ 0xb53b03b2, "tcp_vegas_cwnd_event" },
	{ 0x4a6f7c2d, "tcp_vegas_state" },
	{ 0xeb1ad7b, "tcp_unregister_congestion_control" },
	{ 0xaca82706, "tcp_register_congestion_control" },
	{ 0x2cff789d, "tcp_vegas_init" },
	{ 0x7961f678, "tcp_cong_avoid_ai" },
	{ 0xae957a64, "tcp_slow_start" },
	{ 0xa9bab0af, "tcp_vegas_pkts_acked" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=tcp_vegas";


MODULE_INFO(srcversion, "89F9B46099E85F2C4DEE2BC");
