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
	{ 0xfb578fc5, "memset" },
	{ 0x5f60d4a8, "nla_reserve" },
	{ 0xfb69e8a4, "inet_diag_dump_icsk" },
	{ 0xc5c4be8e, "inet_diag_dump_one_icsk" },
	{ 0xa8d78bfc, "tcp_hashinfo" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x127dab3b, "tcp_get_info" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=inet_diag";


MODULE_INFO(srcversion, "9EA4C76911EBA01C3ACD6B8");
